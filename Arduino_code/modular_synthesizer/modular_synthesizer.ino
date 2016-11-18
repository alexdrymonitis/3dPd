// modular synthesizer code based on Pd, Teensy 3.1 and Odroid
// goes with modular_synth Pd patch
// written by Alexandros Drymonitis

#include <SPI.h>

#define NUM_OF_MODULES 2
//#define SMOOTH 20
#define BAUDRATE 57600
// define a speed for the SPI library, as a cable that's too long might give strange results with a high speed
// with a 2 meter ribbon cable 8000000 seems to work fine, trial and error might help here...
#define SPISPEED 8000000
// if using a board wih an analog pin resolution greater than 10-bits
// uncomment the line below and set the correct resolution
#define ANALOG_RESOLUTION 12


// global variables


// variables that change according to setup

// output shift registers

// array to hold number of output chip pins with banana terminals wired to them (used in the loop() function)
byte output_pins[NUM_OF_MODULES] = { 2, 2 };
// array to hold the position of the first LED controlled directly by a switch of each module, starting from 0
// if an LED is controlled over serial, exclude it
byte first_led[NUM_OF_MODULES] = { 0, 0 };

// input shift registers

// array to hold binary numbers of input chips pins with banana terminals wired to them
// for previous and current states comparison
// a 0 is a pin with no banana terminal and a 1 is a pin with a banana terminal
byte banana_pins[NUM_OF_MODULES] = { B00000111, B00000111 };
// array to hold binary numbers of input chips pins with switches wired to them
// for previous and current states comparison
// a 0 is a pin with no switch and a 1 is a pin with a switch
byte switch_pins[NUM_OF_MODULES] = { B00011000, B00011000 };
// array to hold number of switches on each module that control LEDs directly
// switches that control LEDs via serial should be excluded
byte num_of_switches[NUM_OF_MODULES] = { 0, 0 };
// this byte represents the pin the patch update switch is attached to
// and it's used in the check_switches() function
byte patch_update_switch = B00010000;
// this is the index of the module where the patch update switch is attached to
int patch_update_switch_module = 0;

// multiplexers

// master multiplexers controlling the multiplexers of each module
const byte num_of_master_mux = 1;
// number of slave multiplexers for each master multiplexer
int num_of_slave_mux[num_of_master_mux] = { 2 };
// constant to hold the greatest number in the num_of_slave_mux array
// it will be needed to allocate memory for the multiple_pots array which is created further down
// this constant will be used in a for loop to accumulate the total number of potentiometers
const int greatest_num_of_slave_mux = 2;
// two dimensional array to hold number of pins used on each slave multiplexer
// rows = num_of_master_mux, columns = greatest_num_of_slave_mux
int num_of_pots[num_of_master_mux][greatest_num_of_slave_mux] = { { 3, 3 } };


// variables that are constant, regardless of setup

// global index for connection, switch and potentiometer functions and loops
int ndx;
// maximum number of bytes to be transfered to Pure Data over serial
// 99% of the times only part of this will be transferred to Pd
// it's size will be calculated in the setup function
int num_of_total_data;
// buffer to hold data transfered to Pure Data over serial
// it's size will be allocated in the setup function because the total_pots variable hasn't been calculated yet
byte *transfer_data;


// output shift registers

const byte output_latch = 10;
// byte array to transfer data to output chips via SPI
byte output_data[NUM_OF_MODULES] = { 0 };
// use a variable to hold the sum of the elements of the output_pins array
// it will be calculated in the setup function
int sum_of_output_pins = 0;
// boolean to exit the output pins loop when a new connection has been detected
boolean terminate_bananas;


// input shift registers

const byte input_latch = 9;
// array to receive data from input chips via SPI
byte input_data[NUM_OF_MODULES] = { 0 };
// this array will hold the number of total banana terminals on each module which is needed in Pd
// its elements will be calculated in the setup function
byte total_input_bananas[NUM_OF_MODULES];
// this array will hold the number of total switches on each module which is needed in Pd
// its elements will be calculated in the setup function
byte total_num_switches[NUM_OF_MODULES];
// variable to store the pin of the first switch of each module
// its elements will be calculated in the setup function
byte first_switch[NUM_OF_MODULES] = { 0 };
// two-dimensional array to hold the states of banana plugs
// rows = sum_of_output_pins, columns = NUM_OF_MODULES
// it's size will be allocated in the setup function because the sum_of_output_pins variable hasn't been calculated yet
byte **banana_states;
// array to store states of switches
byte switch_states[NUM_OF_MODULES];
// boolean to determine whether to store connection states to active_modules or back_up array
boolean patch_update = false;
// we need this variable to test if the patch update switch has changed
int old_patch_update_switch = 0;
// and this boolean to set whether this switch has just changed its position
boolean patch_update_switch_changed = false;


// multiplexers

// number of control pins the master multiplexers have (no accumulation for more than one mux used)
// for a 16-channel mux it's 4 (2^4 = 16)
const int num_master_ctl_pins = 4;
// the next two arrays will have their elements calculated in the setup function
// and they will be used in the read_pots() function to set the control pins of the master multiplexers
int master_ctl_pins[num_master_ctl_pins];
int master_ctl_values[num_master_ctl_pins];
// number of control pins the slave multiplexers have (no accumulation for more than one mux used)
// for an 8-channel mux it's 3 (2^3 = 8)
const int num_slave_ctl_pins = 3;
// the next two arrays will have their elements calculated in the setup function
// and they will be used in the read_pots() function to set the control pins of the slave multiplexers
int slave_ctl_pins[num_slave_ctl_pins];
int slave_ctl_values[num_slave_ctl_pins];
// variable to hold the total number of potentiometers which will be calculated in the setup function
int total_pots = 0;
float *smoothedVals;
// arrays to store and back up activity of modules (back up if we're not updating the patching)
// the potentiometers of each module will be either read or not read according to their activity
byte active_modules[NUM_OF_MODULES] = { 0 };
byte back_up_modules[NUM_OF_MODULES] = { 0 };


// Custom functions

// function to transfer data to the output shift registers via SPI
void refresh_output()
{
  digitalWrite(output_latch, LOW);
  for(int i = NUM_OF_MODULES - 1; i >= 0; i--){
    // send a byte to the next register
    SPI.transfer(output_data[i]);
  }
  digitalWrite(output_latch, HIGH);
}

// function to receive data from the input shift registers via SPI
void refresh_input()
{
  digitalWrite(input_latch, LOW);
  digitalWrite(input_latch, HIGH);
  for(int i = 0; i < NUM_OF_MODULES; i++){
    // send a value of 0 to read the first byte returned
    input_data[i] = SPI.transfer(0);
  }
}

void check_connections(int pin, int module)
{
  int local_index = ndx;
  // store the first altered state of input bytes to the transfer array
  for(int i = 0; i < NUM_OF_MODULES; i++){
    // mask the input byte to exclude any possible switches on the module and check if changed
    byte masked_banana = input_data[i] & banana_pins[i];
    if(masked_banana != banana_states[pin][i]){
      // store other_data index, number of expected bytes, index within the other_data (check Pd patch)
      // input chip byte (split in two), number of input pins of current chip
      // number of output pin, and total number of input pins
      transfer_data[local_index++] = NUM_OF_MODULES;
      transfer_data[local_index++] = 6;
      // data that are not diffused to modules need an extra index where 1 is the index for the connections data
      transfer_data[local_index++] = 1;
      transfer_data[local_index++] = masked_banana & 0x7f;
      transfer_data[local_index++] = masked_banana >> 7;
      transfer_data[local_index++] = total_input_bananas[i];
      // [mtx_*~] in Pd starts counting from 1
      transfer_data[local_index++] = pin + 1;
      // zero last byte of this group first
      transfer_data[local_index] = 0;
      // and then check if we're not in the first module
      if(i){
        // if we're not in the first module accumulate the number of previous modules' input pins
        for(int j = 0; j < i; j++) transfer_data[local_index] += total_input_bananas[j];
        local_index++;
      }
      else{
        // otherwise just increment the local index counter
        local_index++;
      }

      // check if the current module is being activated or not
      // whenever a patch cord is plugged in, the input byte will increase no matter which bit reads HIGH
      // whenever a patch cord is plugged out, the input byte will decrease
      if(masked_banana > banana_states[pin][i]){
        if(module == i){
          back_up_modules[i] += 1;
          if(patch_update) active_modules[i] += 1;
        }
        else{
          back_up_modules[i] += 1;
          back_up_modules[module] += 1;
          if(patch_update){
            active_modules[i] += 1;
            active_modules[module] += 1;
          }
        }
      }
      else{
        if(module == i){
          back_up_modules[i] -= 1;
          if(back_up_modules[i] < 0) back_up_modules[i] = 0;
          if(patch_update){
            active_modules[i] -= 1;
            if(active_modules[i] < 0) active_modules[i] = 0;
          }
        }
        else{
          back_up_modules[i] -= 1;
          back_up_modules[module] -= 1;
          if(back_up_modules[i] < 0) back_up_modules[i] = 0;
          if(back_up_modules[module] < 0) back_up_modules[module] = 0;
          if(patch_update){
            active_modules[i] -= 1;
            active_modules[module] -= 1;
            if(active_modules[i] < 0) active_modules[i] = 0;
            if(active_modules[module] < 0) active_modules[module] = 0;
          }
        }
      }
      // update the banana_states array
      banana_states[pin][i] = masked_banana;
      // set the loop termination boolean to true, update the global index and exit loop
      terminate_bananas = true;
      ndx = local_index;
      break;
    }
  }
  // update the active_modules array if the patch update switch has changed to on
  if(patch_update && patch_update_switch_changed){
    for(int i = 0; i < NUM_OF_MODULES; i++) active_modules[i] = back_up_modules[i];
    patch_update_switch_changed = false;
  }
}

void check_switches()
{
  int local_index = ndx;
  for(int i = 0; i < NUM_OF_MODULES; i++){
    // mask the input byte according to the position of the switches and check if changed
    byte masked_switch = input_data[i] & switch_pins[i];
    if(masked_switch != switch_states[i]){
      // if changed, store module index, num of bytes Pd should expect
      // module data index (1 for switches, 0 for potentiometers)
      // input chip byte split in two, and pin number of the first switch of the current module
      transfer_data[local_index++] = i;
      transfer_data[local_index++] = 5;
      // 1 indicates that the data received in a module abstraction concern its switches
      transfer_data[local_index++] = 1;
      transfer_data[local_index++] = masked_switch & 0x7f;
      transfer_data[local_index++] = masked_switch >> 7;
      transfer_data[local_index++] = total_num_switches[i];
      transfer_data[local_index++] = first_switch[i];
      // check if we're in the dac_module and read the patch update switch
      if(i == patch_update_switch_module){
        // check if it has changed
        if((masked_switch & patch_update_switch) != old_patch_update_switch){
          // and check if it's in the on position
          if(masked_switch & patch_update_switch){
            patch_update = true;
            patch_update_switch_changed = true;
            old_patch_update_switch = masked_switch & patch_update_switch;
          }
          else{
            patch_update = false;
            patch_update_switch_changed = true;
            old_patch_update_switch = masked_switch & patch_update_switch;
          }
        }
      }
      // control LEDs of current module
      int switch_pos = first_switch[i];
      int led_pos = first_led[i];
      // read the switches of the current chip and set LEDs accordingly
      for(int j = 0; j < num_of_switches[i]; j++){
        int switch_state = bitRead(input_data[i], switch_pos++);
        bitWrite(output_data[i], led_pos++, switch_state);
      }
      // update the switch_states array and the global index and exit loop
      switch_states[i] = masked_switch;
      ndx = local_index;
      break;
    }
  }
}

// function to read potentiometers of active modules
void read_pots(){
  // set local index to global index's current value
  int local_index = ndx;
  // set row index for 2D array multiple pots
  int pot_index = 0;
  int module_index = 0;
  //static byte smooth_index = 0;
  // run througn all master multiplexers
  for(int master_mux = 0; master_mux < num_of_master_mux; master_mux++){
    // run through all slave multiplexers
    for(int slave_mux = 0; slave_mux < num_of_slave_mux[master_mux]; slave_mux++){
      // if this module is active, store its index, the number of bytes to expect
      // and a 0 to denote that the following values are potentiometer values
      // and route its output to its master multiplexer
      // activity of modules is being stored in the check_connections function
      if(active_modules[module_index]){
        transfer_data[local_index++] = module_index;
        transfer_data[local_index++] = (num_of_pots[master_mux][slave_mux] * 2) + 1;
        transfer_data[local_index++] = 0;
        // set the control pins of the current master multiplexer
        for(int i = 0; i < num_master_ctl_pins; i++)
          digitalWrite(master_ctl_pins[i], (slave_mux & master_ctl_values[i]) >> i);
      }
      // run through the pins used on each slave multiplexer
      // the active module test needs to be separate here in order to be able
      // to increment the row_index variable at every iteration of the loop below
      for(int pot = 0; pot < num_of_pots[master_mux][slave_mux]; pot++){
        // if this module is active store the values of its potentiometers
        if(active_modules[module_index]){
          // set the control pins of the current slave multiplexer
          for(int i = 0; i < num_slave_ctl_pins; i++)
            digitalWrite(slave_ctl_pins[i], (pot & slave_ctl_values[i]) >> i);
          // read the corresponding analog pin of the Teensy
          int pot_val = analogRead(master_mux);
          // smooth out the analog reading
          int smoothed = smooth(pot_val, pot_index);
          // and store the smoothed value to the transfer_data array
          transfer_data[local_index++] = smoothed & 0x007f;
          transfer_data[local_index++] = smoothed >> 7;
        }
        pot_index++; // update the 2D array row index anyway
      }
      // the module index is independent of activity as well, it should always increment
      module_index++;
    }
  }

  ndx = local_index;
}

int smooth(int val_to_smooth, int index){
  float filterVal = 0.95;
  smoothedVals[index] = (val_to_smooth * (1 - filterVal)) + (smoothedVals[index]  *  filterVal);

  return (int)smoothedVals[index];
}

void reset_connections()
{
  for(int i = 0; i < sum_of_output_pins; i++){
    for(int j = 0; j < NUM_OF_MODULES; j++)
      banana_states[i][j] = 0;
  }
}

void reset_switches()
{
  for(int i = 0; i < NUM_OF_MODULES; i++)
    switch_states[i] = 0;
}

void read_serial_data()
{
  if(Serial.available()){
    byte in_byte = Serial.read();
    static int serial_val;
    static int which_module;
    static int which_pin;
    static int pin_val;
    if((in_byte >= '0') && (in_byte <= '9'))
      serial_val = serial_val * 10 + in_byte - '0';
    else{
      if(in_byte == 'm'){
        which_module = serial_val;
        serial_val = 0;
      }
      else if(in_byte == 'p'){
        which_pin = serial_val;
        serial_val = 0;
      }
      else if(in_byte == 'v'){
        pin_val = serial_val;
        bitWrite(output_data[which_module], which_pin, pin_val);
        serial_val = 0;
      }
      else if(in_byte == 'r'){
        reset_connections();
        reset_switches();
      }
    }
  }
}


// Built-in functions

void setup()
{
  // initialize the SPI library
  SPI.begin();
  // and set the speed, according to cable length
  SPI.beginTransaction(SPISettings(SPISPEED, MSBFIRST, SPI_MODE0));

  // allocate memory for all arrays that don't have their sizes calculated yet

  // banana_states 2D array
  for(int i = 0; i < NUM_OF_MODULES; i++) sum_of_output_pins += output_pins[i];
  banana_states = new byte* [sum_of_output_pins];
  for(int i = 0; i < sum_of_output_pins; i++) banana_states[i] = new byte[NUM_OF_MODULES];

  // total_pots and smoothVals array
  for(int i = 0; i < num_of_master_mux; i++){
    for(int j = 0; j < greatest_num_of_slave_mux; j++)
      total_pots += num_of_pots[i][j];
  }
  smoothedVals = new float [total_pots];

  // transfer_data array
  // its size is totalPots * 2
  // plus total number of modules * 2 (one for sending the module index and one for sending the nr of pots of each module)
  // plus one for the potentiometer data index
  // plus seven for the connections function
  // plus seven for the switches function
  // plus one for denoting the beginning of data
  num_of_total_data = (total_pots * 2) + (NUM_OF_MODULES * 2) + 16;
  transfer_data = new byte [num_of_total_data];
  // write the start character to the transfer_data array
  transfer_data[0] = 0xff;


  // set multiplexers control pins and values
  for(int i = 0; i < num_master_ctl_pins; i++){
    master_ctl_values[i] = pow(2, (i + 1)) - 1;
    // master multiplexers have their control pins wired to digital pins 2 to 5
    master_ctl_pins[i] = (num_master_ctl_pins - i) + 1;
    pinMode(master_ctl_pins[i], OUTPUT);
  }

  for(int i = 0; i < num_slave_ctl_pins; i++){
    slave_ctl_values[i] = pow(2, (i + 1)) - 1;
    // slave multiplexers have their control pins wired to digital pins 6 to 8
    slave_ctl_pins[i] = (num_slave_ctl_pins - i) + 5;
    pinMode(slave_ctl_pins[i], OUTPUT);
  }


  // set the total number of input banana terminals and switches
  for(int i = 0; i < NUM_OF_MODULES; i++){
    for(int j = 0; j < 8; j++) total_input_bananas[i] += bitRead(banana_pins[i], j);
  }

  for(int i = 0; i < NUM_OF_MODULES; i++){
    for(int j = 0; j < 8; j++) total_num_switches[i] += bitRead(switch_pins[i], j);
  }

  // set the pin number the first switch is attached to on each module
  for(int i = 0; i < NUM_OF_MODULES; i++){
    for(int j = 0; j < 8; j++){
      byte found_switch = bitRead(switch_pins[i], j);
      if(found_switch){
        first_switch[i] = j;
        break;
      }
    }
  }


  // initialize the pins used to control the shift registers
  pinMode(input_latch, OUTPUT);
  pinMode(output_latch, OUTPUT);
  digitalWrite(input_latch, HIGH);
  digitalWrite(output_latch, HIGH);


  //initialize the banana sockets states two-dimensional array
  reset_connections();

  // initialize the switch states array
  reset_switches();

  // open the serial port
  Serial.begin(BAUDRATE);
  // the Teensy needs the line below in order to write the analog pins resolution to the serial line
  // this was suggested by el_supremo (Pete) in Teensy's forum
  while(!Serial);

  // if analog resolution is other than 10 set it and send it to Pd
  #ifdef ANALOG_RESOLUTION
    analogReadResolution(ANALOG_RESOLUTION);
    int local_index = 1;
    // data that don't route to modules get the number of modules as their first index
    transfer_data[local_index++] = NUM_OF_MODULES;
    // after this, we should expect the other_data index and one byte only in Pd
    transfer_data[local_index++] = 2;
    // as a second index a 0 denotes the analog resolution data
    transfer_data[local_index++] = 0;
    transfer_data[local_index++] = ANALOG_RESOLUTION;
    Serial.write(transfer_data, local_index);
  #endif
}

void loop()
{
  // index offset for the transfer_data array
  ndx = 1;

  // control LEDs from Pd
  read_serial_data();

  // write to output shift registers and read input shift registers
  // first set terminate boolean to false
  terminate_bananas = false;
  // pin number variable
  int pin = 0;
  for(int i = 0; i < NUM_OF_MODULES; i++){
    // add number of pins of previous chip
    if(i) pin += output_pins[i - 1];
    // then go through the banaba plug pins
    for(int j = 0; j < output_pins[i]; j++){
      int local_pin = pin;
      // set the current pin HIGH
      bitSet(output_data[i], j);
      refresh_output();
      // give some time to the shift registers to do their job
      delayMicroseconds(1);
      refresh_input();
      // give some time to the shift registers to do their job
      delayMicroseconds(1);
      // add current and previous pins of current chip
      local_pin += j;
      check_connections(local_pin, i);
      // reset the pin LOW
      bitClear(output_data[i], j);
      // check if a new connection has been detected and if so, exit loop
      if(terminate_bananas) break;
    }
    if(terminate_bananas) break;
  }

  check_switches();

  read_pots();

  // write data to the serial port, if there is any activity
  if(ndx > 1) Serial.write(transfer_data, ndx);
}
