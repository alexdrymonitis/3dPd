# modular_synthesizer

This repository hosts the code, schematics, and circuit board designs (made in Eagle freeware) for a hardware digital modular synthesizer. The audio is programmed in Pure Data and the hardware control in the Arduino language. A Teensy 3.2 is intended to be used as a micro-controller, but it is generally referred to as an Arduino).


## Note about the Arduino code

Open the modular_synthesizer.ino sketch (located in Arduino_code/modular_synthesizer/). In the Arduino IDE (or your favorite IDE) you'll need to change some of the pre-processor defines from line 7 to 12. You'll also need to change some of the variables from line 22 to 59. There are comments explaining which variables are subject to change, according to each synth setup, and what each variable does.
Once you've done that, upload the sketch to your board.


## Note about the Pd patches

The heart of this project is the modular_synthesizer.pd patch. Open it and you'll see some other abstractions in there. In the top level patch you'll probably need to change the arguments to the [arduino_data] and [matrix_connections] abstractions. Then go into the [pd modules] sub-patch and read the comments.
There's a [template_module~] abstraction (twice) which shows you how you should make a module abstraction. There are plenty of comments there and it covers all the necessary data a module abstraction needs to have.
Note: I know that using [declare] inside abstractions is not a good idea, since the paths that will be searched will be eventually called by the top level patch, but using it helps me work with abstractions without needing to open them through the main modular_synthesizer.pd patch.

In the [pd print_connections] subpatch you can enable the printing of the connections you make in your synthesizer, for debugging. The way this project is coded, it lets you use a switch (in a module of your choice, I put it in the ADC/DAC module) in order to control whether the patching will be updated instantly or whether it will be stored in a matrix. In the second case, when you change the position of the switch, the matrix will be sent to [mtx_*~] (which is the heart of the audio of the synth). This is why [pd print_connections] destinguishes between an "element" message and a whole matrix. Check the Arudino code and the [pd update_patching] sub-patch (which is in the [matrix_connections] abstraction) to see how this works.


## Note about the circuits

In the circuits directory there are four directories, each containing a schematic and a board design Eagle file. The "module_generic_top_board" is the board where an input shift register, an output shift register, and an eight-channel multiplexer (called a slave multiplexer) are soldered. This is also where all the potentiometers, banana terminals, switches and LEDs are soldered. The "module_generic_bottom_board" is the board where the previous one mounts on. This board has two IDC connectors and two 4-position DIP switches. The orientation of these boards should be with the IDCs facing left and right, as you see the board (and the actual physical module) from above. These connectors are used to daisy chain the shift registers and to pass the control pins for the multiplexers. The DIP switches set the direction on the data. If the data comes in from the right side of the module, and leaves from the left, the DIP switch on the side of the IDC facing right, should have its position like this: on-off-on-off. The other DIP switch should have its positions like this: off-on-off-on. If data comes from the left side, the position of the switches should be inverted.
The "power_bus" board receives 5V in its clamp, and drops the voltage to 3.3V with a voltage regulator. This is because the Teensy works at 3.3V. This board also includes a 16-channel multiplexer. This multiplexer is called a "master multiplexer". This module provides power to all modules, and receives data from the slave multiplexters, through the JST connectors on it. Through its IDC connector, it sends the data of the slave multiplexers (through the master multiplexer) to the Arduino. With one power bus, a synthesizer can have up to 16 modules (as many as the channels of the master multiplexer). You can use a second power bus if you want to use more modules. Check the Arduino code for information.
The "Teensy_mother_board" is a board where the Teensy will mount on. Through the 10-pin IDC connector of this board, the Teensy will send and receive data to/from the shift registers and will control all the slave multiplexers. Through the two 6-pin IDC connectors, it will connect to two power buses. Note that the output of one master multiplexer, arriving to one the the two 6-pin IDC connectors, connects to the first analog pin of the Teensy, and the other one connects to the second analog pin.
