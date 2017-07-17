/***********************************************************************************
 * This Arduino sketch is written for a physical digital modular synthesizer       *
 * The control part of this synthesizer runs on a Teensy3.2 which is programmed in *
 * the Arduino language using Teensyduino.                                         *
 * The audio part of the synthesizer runs on an embedded computer (Raspberry Pi 3) *
 * which is programmed in Pure Data. In order for this                             *
 * code to run, you'll need to combine it with the Pd patch that comes with        *
 * this sketch.                                                                    *
 ***********************************************************************************/

// inculde the SPI library to control the shift registers
#include <SPI.h>

// various MARCO definitions that need to be adjusted according to the synthesizer setup

// define the number of modules you're using
#define NUM_OF_MODULES 5
// define a speed for the SPI library so that it can work properly
// daisy chaining many modules can result in a long distance that needs to be convered
// by the SPI pins. a 8000000 speed worked for me for 12 modules, after that
// I needed to drop it to 4000000. trial and error will probably work here
// if you get funny results, you'll probably need to lower the speed
#define SPISPEED 8000000
// if using a board wih an analog pin resolution greater than 10-bits
// uncomment the line below and set the correct resolution
#define ANALOG_RESOLUTION 13
// clip analog readings for a more unified result
#define CLIP 8100
// set the filter coefficient for smoothing out the potentiometer readings
// the value should be between 0 and 1. the greater the value, the more the smoothing
// but the less responsive the potentiometers will be, plus the more jitter you'll
// get the first time you read a pot
#define FILTER_COEFF 0.5


/******************************* global variables **********************************/

/******************** variables that change according to setup *********************/


/****************************** output shift registers *****************************/

// array to hold number of signal outputs (banana terminals) of each module
byte outputPins[NUM_OF_MODULES] = { 0, 4, 8, 4, 4 };


/******************************* input shift registers *****************************/

// array to hold binary numbers of signal inputs (banana terminals) of each module
// a 1 is a pin with a banana terminal and a 0 is a pin with no banana terminal
// for example, a module with two signal inputs should get the value B00000011
// signal inputs should be wired from the first input of the shift register and should not skip pins
byte bananaPins[NUM_OF_MODULES] = { B00000011, B00001111, B00001111, B00001111, B00001111 };
// array to hold binary numbers of switched of each module
// a 1 is a pin with a switch and a 0 is a pin with no switch
// for example, a module that has three switches (and two signal inputs, like the example above) should get the value B00011100
// switches should be wired after the signal inputs (banana terminals) and should not skip pins
byte switchPins[NUM_OF_MODULES] = { B00001100, B01110000, B00000000, B00110000, B00110000  };


/*************************************** multiplexers ******************************/

// master multiplexers (on power buses) controlling the multiplexers of each module
// if your setup has more than 16 modules (which is the number of channels of the CD4067 multiplexer) you'll need more than one
const int numOfMasterMux = 1;
// number of modules (slave multiplexers) sending potentiometer data to each master multiplexer
int numOfSlaveMux[numOfMasterMux] = { 5 };
// 1D or 2D array (according to the number of numOfMasterMux) to hold number of potentiometers on each module
// rows = numOfMasterMux, columns = 16, since the master multiplexers have 16 channels
// even if it's a 1D array, you should still write it as a 2D array, like the example below
int numOfPots[numOfMasterMux][16] = { { 2, 8, 4, 4, 4 } };


/************** end of variables that change according to setup ********************/

/****** no need to further edit the code, unless I've done something wrong... ******/


// variables and MACRO definitions that are constant, regardless of setup

#define BAUDRATE 57600
// the following two interrupt pins are used to detect whether we're shutting down the system
#define CONST_HIGH 16
#define INTERRUPT_PIN 17
// volatile boolean changed by the interrupt signal
volatile bool sendShutDownSignal = false;

// global index for connection, switch and potentiometer functions and loops
// maximum number of bytes to be transfered to Pure Data over serial
// 99% of the times only part of this will be transferred to Pd
// it's size will be calculated in the setup function
int numOfTotalData;
// buffer to hold data transfered to Pure Data over serial
// it's size will be allocated in the setup function because the total_pots variable hasn't been calculated yet
byte *transferData;
// variable to hold analog pin resolution which will be set in the setup() function
// depending on whether the ANALOG_RESOLUTION macro has been declared or not
// defaults to 10, written analogue here to avoid name collision with Arduino's
// Built-in function analogReadResolution()
byte analogueReadResolution = 10;


/***************************** output shift registers *******************************/

const byte outputLatch = 10;
// byte array to transfer data to output chips via SPI
byte outputData[NUM_OF_MODULES] = { 0 };
// use a variable to hold the sum of the elements of the outputPins array
// it will be calculated in the setup function
int totalNumOutputs = 0;
// boolean to exit the output pins loop when a new connection has been detected
boolean terminateBananas;


/****************************** input shift registers *******************************/

const byte inputLatch = 9;
// array to receive data from input chips via SPI
byte inputData[NUM_OF_MODULES] = { 0 };
// this array will hold the number of total banana terminals on each module which is needed in Pd
// its elements will be calculated in the setup function
byte totalInputBananas[NUM_OF_MODULES];
// this byte will hold the total number of inputs in the whole system and will be sent to Pd so that the matrix can be created
byte totalNumInputs;
// this array will hold the number of total switches on each module which is needed in Pd
// its elements will be calculated in the setup function
byte totalNumSwitches[NUM_OF_MODULES];
// variable to store the pin of the first switch of each module
// its elements will be calculated in the setup function
byte firstSwitch[NUM_OF_MODULES] = { 0 };
// two-dimensional array to hold the states of banana plugs
// rows = totalNumOutpus, columns = NUM_OF_MODULES
// it's size will be allocated in the setup function because the totalNumOutputs variable hasn't been calculated yet
byte **bananaStates;
// array to store states of switches
byte switchStates[NUM_OF_MODULES];
// variable to determine whether to store connection states to activeModules or backUpArray
int patchUpdate = 0;


/********************************** multiplexers ************************************/

// number of control pins the master multiplexers have (no accumulation for more than one mux used)
// for a 16-channel mux it's 4 (2^4 = 16)
const int numMasterCtlPins = 4;
// the next two arrays will have their elements calculated in the setup function
// and they will be used in the readPots() function to set the control pins of the master multiplexers
int masterCtlPins[numMasterCtlPins];
int masterCtlValues[numMasterCtlPins];
// number of control pins the slave multiplexers have (no accumulation for more than one mux used)
// for an 8-channel mux it's 3 (2^3 = 8)
const int numSlaveCtlPins = 3;
// the next two arrays will have their elements calculated in the setup function
// and they will be used in the readPots() function to set the control pins of the slave multiplexers
int slaveCtlPins[numSlaveCtlPins];
int slaveCtlValues[numSlaveCtlPins];
// variable to hold the total number of potentiometers which will be calculated in the setup function
int totalPots = 0;
// pointer to create an array with values that will be used for smoothing
// its size is calculated in the setup() function and it contains one element per potentiometer
float *smoothedVals;
// arrays to store and back up activity of modules (back up if we're not updating the patching)
// the potentiometers of each module will be either read or not read according to their activity
byte activeModules[NUM_OF_MODULES] = { 0 };
byte backUpModules[NUM_OF_MODULES] = { 0 };
// and this boolean to set whether this switch has just changed its position
// both this information will come from reading serial data from Pd
boolean patchUpdateChanged = false;
// lastly a boolean to set whether a CLIP macro has been defined or not, which is used in the readPots() function
boolean clip = false;


/********************************* Custom functions **********************************/

// function to transfer data to the output shift registers via SPI
void refreshOutput() {
  digitalWrite(outputLatch, LOW);
  for(int i = NUM_OF_MODULES - 1; i >= 0; i--){
    // send a byte to the next register
    SPI.transfer(outputData[i]);
  }
  digitalWrite(outputLatch, HIGH);
}

// function to receive data from the input shift registers via SPI
void refreshInput() {
  digitalWrite(inputLatch, LOW);
  digitalWrite(inputLatch, HIGH);
  for(int i = 0; i < NUM_OF_MODULES; i++){
    // send a value of 0 to read the first byte returned
    inputData[i] = SPI.transfer(0);
  }
}

void checkConnections(int pin, int module) {
  int localIndex = 1;

  // store the first altered state of input bytes to the transfer array
  for(int i = 0; i < NUM_OF_MODULES; i++){
    // mask the input byte to exclude any possible switches on the module and check if changed
    byte maskedBanana = inputData[i] & bananaPins[i];
    // check if the input byte has changed and only then send the corresponding data to Pd
    if(maskedBanana != bananaStates[pin][i]){
      // store generic data index (0) and number of expected bytes beyond that which are
      // secondary index for generic data (check Pd patch)
      // input chip byte (split in two), number of input pins of current chip
      // number of output pin, and total number of input pins
      transferData[localIndex++] = 0;
      transferData[localIndex++] = 6;

      // data that are not diffused to modules need a secondary index where 1 is the index for the connections data
      transferData[localIndex++] = 1;

      // then we split the input byte
      transferData[localIndex++] = maskedBanana & 0x7f;
      transferData[localIndex++] = maskedBanana >> 7;

      transferData[localIndex++] = totalInputBananas[i];

      // [mtx_*~] in Pd starts counting from 1
      transferData[localIndex++] = pin + 1;

      // zero last byte of this group first
      transferData[localIndex] = 0;
      // and then check if we're not in the first module
      if(i){
        // if we're not in the first module accumulate the number of previous modules' input pins
        // and increment localIndex
        for(int j = 0; j < i; j++) transferData[localIndex] += totalInputBananas[j];
        localIndex++;
      }
      else{
        // otherwise just increment localIndex
        localIndex++;
      }
      updateModules(maskedBanana, pin, module, i);
      // update the bananaStates array
      bananaStates[pin][i] = maskedBanana;
      // set the loop termination boolean to true, update the global index and exit loop
      terminateBananas = true;
      break;
    }
  }
  if(localIndex > 1) Serial.write(transferData, localIndex);
  // update the activeModules array if the patch update switch has changed to on
  if(patchUpdate && patchUpdateChanged){
    for(int i = 0; i < NUM_OF_MODULES; i++) activeModules[i] = backUpModules[i];
    patchUpdateChanged = false;
  }
}

void updateModules(byte maskedBanana, int pin, int module, int i) {
  // check if the current module is being activated or not
  // whenever a patch cord is plugged in, the input byte will increase no matter which bit reads HIGH
  // whenever a patch cord is plugged out, the input byte will decrease
  if(maskedBanana > bananaStates[pin][i]){
    if(module == i){
      backUpModules[i] += 1;
      if(patchUpdate) activeModules[i] += 1;
    }
    else{
      backUpModules[i] += 1;
      backUpModules[module] += 1;
      if(patchUpdate){
        activeModules[i] += 1;
        activeModules[module] += 1;
      }
    }
  }
  else{
    if(module == i){
      backUpModules[i] -= 1;
      if(backUpModules[i] < 0) backUpModules[i] = 0;
      if(patchUpdate){
        activeModules[i] -= 1;
        if(activeModules[i] < 0) activeModules[i] = 0;
      }
    }
    else{
      backUpModules[i] -= 1;
      backUpModules[module] -= 1;
      if(backUpModules[i] < 0) backUpModules[i] = 0;
      if(backUpModules[module] < 0) backUpModules[module] = 0;
      if(patchUpdate){
        activeModules[i] -= 1;
        activeModules[module] -= 1;
        if(activeModules[i] < 0) activeModules[i] = 0;
        if(activeModules[module] < 0) activeModules[module] = 0;
      }
    }
  }
}

void checkSwitches() {
  int localIndex = 1;

  for(int i = 0; i < NUM_OF_MODULES; i++){
    // mask the input byte according to the position of the switches and check if changed
    byte maskedSwitch = inputData[i] & switchPins[i];
    if(maskedSwitch != switchStates[i]){
      // if changed, store module index (starting from 1) and num of bytes Pd should expect beyond that which are
      // module data index (1 for switches, 0 for potentiometers)
      // input chip byte split in two, number of switches of current module
      // and pin number of the first switch of the current module
      transferData[localIndex++] = i + 1;
      transferData[localIndex++] = 5;

      // 1 indicates that the data received in a module abstraction concern its switches
      transferData[localIndex++] = 1;

      // split the input byte
      transferData[localIndex++] = maskedSwitch & 0x7f;
      transferData[localIndex++] = maskedSwitch >> 7;
      // write the number of switches of this module
      transferData[localIndex++] = totalNumSwitches[i];

      // write the pin number of the first switch of the current module
      transferData[localIndex++] = firstSwitch[i];
      // update the switchStates array and the global index and exit loop
      switchStates[i] = maskedSwitch;
      break;
    }
  }
  if(localIndex > 1) Serial.write(transferData, localIndex);
}

// function to read potentiometers of active modules
void readPots() {
  // set local index to global index's current value
  int localIndex = 1;
  // set row index for 2D array multiple pots (potIndex)
  int potIndex = 0;
  int moduleIndex = 0;
  //static byte smoothIndex = 0;
  // run througn all master multiplexers
  for(int masterMux = 0; masterMux < numOfMasterMux; masterMux++){
    // run through all slave multiplexers
    for(int slaveMux = 0; slaveMux < numOfSlaveMux[masterMux]; slaveMux++){
      // if this module is active, store its index (starting from 1) and the number of bytes to expect which are
      // the extra index 0 to denote that the following values are potentiometer values
      // Note: activity of modules is being stored in the checkConnections function
      if(activeModules[moduleIndex]){
        transferData[localIndex++] = moduleIndex + 1;
        // pot values are being split in two, and we need an extra byte for the pot index
        transferData[localIndex++] = (numOfPots[masterMux][slaveMux] * 2) + 1;

        // then send the pot index
        transferData[localIndex++] = 0;
        // set the control pins of the current master multiplexer
        for(int i = 0; i < numMasterCtlPins; i++)
          digitalWrite(masterCtlPins[i], (slaveMux & masterCtlValues[i]) >> i);
      }
      // run through the pins used on each slave multiplexer
      // the active module test needs to be separate here in order to be able
      // to increment the potIndex variable at every iteration of the loop below
      for(int pot = 0; pot < numOfPots[masterMux][slaveMux]; pot++){
        // if this module is active store the values of its potentiometers
        if(activeModules[moduleIndex]){
          // set the control pins of the current slave multiplexer
          for(int i = 0; i < numSlaveCtlPins; i++)
            digitalWrite(slaveCtlPins[i], (pot & slaveCtlValues[i]) >> i);
          // add a short delay to establish a better reading of the analog input
          delayMicroseconds(10);
          // read the corresponding analog pin of the Teensy
          int potVal = analogRead(masterMux);
          // smooth out the analog reading
          int smoothed = smooth(potVal, potIndex);
          // clip the values for a more unified result if a CLIP macro has been defined
          // clip to CLIP - 1 for correct table reading in Pd
          if(clip) if(smoothed >= CLIP) smoothed = CLIP - 1;

          // and store the smoothed value split in two to the transferData array
          transferData[localIndex++] = smoothed & 0x007f;
          transferData[localIndex++] = smoothed >> 7;
        }
        potIndex++; // update the 2D array row index anyway
      }
      // the module index is independent of activity as well, it should always increment
      moduleIndex++;
    }
  }
  if(localIndex > 1) Serial.write(transferData, localIndex);
}

int smooth(int valToSmooth, int index) {
  smoothedVals[index] = (valToSmooth * (1 - FILTER_COEFF)) + (smoothedVals[index]  *  FILTER_COEFF);

  return (int)smoothedVals[index];
}

void resetConnections() {
  for(int i = 0; i < totalNumOutputs; i++){
    for(int j = 0; j < NUM_OF_MODULES; j++){
      bananaStates[i][j] = 0;
      activeModules[j] = 0;
      backUpModules[j] = 0;
    }
  }
}

void resetSwitches() {
  for(int i = 0; i < NUM_OF_MODULES; i++)
    switchStates[i] = 0;
}

void sendNumInputsOutputs() {
  int localIndex = 1;

  // store generic data index
  transferData[localIndex++] = 0;

  // tell Pd how many more bytes to expect beyond the first index
  // one for the extra data index, one for the number of inputs and one for the number of outputs
  transferData[localIndex++] = 3;

  // secondary index 2 indicates number of inputs and outputs data
  transferData[localIndex++] = 2;

  transferData[localIndex++] = totalNumInputs;
  transferData[localIndex++] = totalNumOutputs;

  Serial.write(transferData, localIndex);
}

void sendAnalogReadResolution() {
  int localIndex = 1;

  // store generic data index
  transferData[localIndex++] = 0;

  // after this, we should expect the generic data secondary index and the analogRead resolution split in two
  transferData[localIndex++] = 3;

  // as a secondary index a 0 denotes the analog resolution data
  transferData[localIndex++] = 0;
  transferData[localIndex++] = analogueReadResolution & 0x7f;
  transferData[localIndex++] = analogueReadResolution >> 7;

  Serial.write(transferData, localIndex);
}

// interrupt and shut-down functions

void shutDown() {
  sendShutDownSignal = true;
}

void sendShutDown() {
  int localIndex = 1;

  // store the generic data index
  transferData[localIndex++] = 0;

  // let Pd know how many more bytes to expect; one byte is the extra index which will result in a bang after [route]
  transferData[localIndex++] = 1;

  // secondary index denotes shut down data
  transferData[localIndex++] = 3;
  sendShutDownSignal = false;

  Serial.write(transferData, localIndex);
}

void readSerialData() {
  if(Serial.available()){
    byte inByte = Serial.read();
    static int serialVal;
    static int whichModule;
    static int whichPin;
    static int pinVal;
    if(isDigit(inByte))
      serialVal = serialVal * 10 + inByte - '0';
    else{
      if(inByte == 'm'){
        // module indexes in the Pd patch start from 0, but arrays start from 0
        // hence the subtraction by one below
        whichModule = serialVal - 1;
        serialVal = 0;
      }
      else if(inByte == 'p'){
        whichPin = serialVal;
        serialVal = 0;
      }
      else if(inByte == 'v'){
        pinVal = serialVal;
        bitWrite(outputData[whichModule], whichPin, pinVal);
        serialVal = 0;
      }
      else if(inByte == 'u'){
        patchUpdate = serialVal;
        patchUpdateChanged = true;
        serialVal = 0;
      }
      else if(inByte == 'i'){
        sendNumInputsOutputs();
      }
      else if(inByte == 'a'){
        sendAnalogReadResolution();
      }
      else if(inByte == 'r'){
        resetConnections();
        resetSwitches();
      }
    }
  }
}


/******************************** Built-in functions ********************************/

void setup() {
  // initialize the SPI library
  SPI.begin();
  // and set the speed, according to cable length
  SPI.beginTransaction(SPISettings(SPISPEED, MSBFIRST, SPI_MODE0));

  // allocate memory for all arrays that don't have their sizes calculated yet

  // bananaStates 2D array
  for(int i = 0; i < NUM_OF_MODULES; i++) totalNumOutputs += outputPins[i];
  bananaStates = new byte* [totalNumOutputs];
  for(int i = 0; i < totalNumOutputs; i++) bananaStates[i] = new byte[NUM_OF_MODULES];

  // totalPots and smoothedVals array
  for(int i = 0; i < numOfMasterMux; i++){
    for(int j = 0; j < numOfSlaveMux[i]; j++)
      totalPots += numOfPots[i][j];
  }
  smoothedVals = new float [totalPots];

  // transferData array
  // since each function writes separately, the maximum size is totalPots * 2 (the pot values split in two)
  // plus total number of modules * 2
  // one for sending the module index, one for sending the nr of pots of each module
  numOfTotalData = (totalPots * 2) + (NUM_OF_MODULES * 2);
  transferData = new byte [numOfTotalData];
  // write the start character to the transfer_data array
  transferData[0] = 0xff;


  // set multiplexers control pins and values
  for(int i = 0; i < numMasterCtlPins; i++){
    // master multiplexer control values are 1, 3, 7, 15
    masterCtlValues[i] = (1 << (i + 1)) - 1;
    // master multiplexers have their control pins wired to digital pins 2 to 5
    masterCtlPins[i] = (numMasterCtlPins - i) + 1;
    pinMode(masterCtlPins[i], OUTPUT);
  }

  for(int i = 0; i < numSlaveCtlPins; i++){
    // slave multiplexer control values are 1, 3, 7
    slaveCtlValues[i] = (1 << (i + 1)) - 1;
    // slave multiplexers have their control pins wired to digital pins 6 to 8
    slaveCtlPins[i] = (numSlaveCtlPins - i) + 5;
    pinMode(slaveCtlPins[i], OUTPUT);
  }


  // set the total number of input banana terminals and switches
  for(int i = 0; i < NUM_OF_MODULES; i++){
    for(int j = 0; j < 8; j++) totalInputBananas[i] += bitRead(bananaPins[i], j);
    totalNumInputs += totalInputBananas[i];
  }

  for(int i = 0; i < NUM_OF_MODULES; i++){
    for(int j = 0; j < 8; j++) totalNumSwitches[i] += bitRead(switchPins[i], j);
  }

  // set the pin number the first switch is attached to on each module
  for(int i = 0; i < NUM_OF_MODULES; i++){
    for(int j = 0; j < 8; j++){
      byte foundSwitch = bitRead(switchPins[i], j);
      if(foundSwitch){
        firstSwitch[i] = j;
        break;
      }
    }
  }


  // initialize the pins used to control the shift registers
  pinMode(inputLatch, OUTPUT);
  pinMode(outputLatch, OUTPUT);
  digitalWrite(inputLatch, HIGH);
  digitalWrite(outputLatch, HIGH);


  // set the interrupt stuff
  pinMode(CONST_HIGH, OUTPUT);
  pinMode(INTERRUPT_PIN, INPUT);
  digitalWrite(CONST_HIGH, HIGH);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), shutDown, CHANGE);


  //initialize the banana sockets states 2D array
  resetConnections();

  // initialize the switch states array
  resetSwitches();

  // open the serial port
  Serial.begin(BAUDRATE);
  // the Teensy needs the line below in order to write the analog pins resolution to the serial line
  // this was suggested by el_supremo (Pete) in Teensy's forum
  while(!Serial);

  // if analog resolution is other than 10 set it
  #ifdef ANALOG_RESOLUTION
    analogReadResolution(ANALOG_RESOLUTION);
    analogueReadResolution = pow(2, ANALOG_RESOLUTION) - 1;
  #endif

  // if a CLIP value has been defined, use that for the analogueReadResolution
  #ifdef CLIP
    // use CLIP - 1 since 0 is included (this results in better table reading in Pd)
    analogueReadResolution = CLIP - 1;
    clip = true;
  #endif
}

void loop() {
  // control LEDs from Pd
  readSerialData();

  // write to output shift registers and read input shift registers
  // first set terminate boolean to false
  terminateBananas = false;
  // pin number variable
  int pin = 0;
  for(int i = 0; i < NUM_OF_MODULES; i++){
    // add number of pins of previous chip
    if(i) pin += outputPins[i - 1];
    // then go through the banaba plug pins
    for(int j = 0; j < outputPins[i]; j++){
      int localPin = pin;
      // set the current pin HIGH
      bitSet(outputData[i], j);
      refreshOutput();
      // give some time to the shift registers to do their job
      delayMicroseconds(1);
      refreshInput();
      // give some time to the shift registers to do their job
      delayMicroseconds(1);
      // add current and previous pins of current chip
      localPin += j;
      checkConnections(localPin, i);
      // reset the pin LOW
      bitClear(outputData[i], j);
      // check if a new connection has been detected and if so, exit loop
      if(terminateBananas) break;
    }
    if(terminateBananas) break;
  }

  checkSwitches();

  readPots();

  if(sendShutDownSignal){
    // if the interrupt pin received a signal change
    sendShutDown();
  }
}
