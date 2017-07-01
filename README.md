# modular_synthesizer

This repository hosts the code (schematics, and circuit board designs (made in Kicad) will be uploaded shortly) for a hardware digital modular synthesizer. The audio is programmed in Pure Data and the hardware control in the Arduino language.
This synthesizer is supposed to run on a Raspberry Pi 3 and a Teensy3.2. You're welcome to use any other hardware you like, but this specific hardware is supported and explained here.


## Note about the Arduino code

Go to the Arduino_code directory and read the README.txt file first. That should give you enough information about the Arduino code.


## Note about the Pd patches

The heart of this project is the modular_synthesizer.pd patch. Open it and you'll see some other abstractions in there. Go into the [pd modules] sub-patch and look inside.
Each module must have its own abstraction. There are four tutorial module abstractions (one of them used twice) which shows you how to build your own. Open them and read them carefully.

There's also a debug.pd patch which helps with debugging by showing input from the Teensy and printing various message concerning module connections and LED states.

### External libraries used

This project uses the following external libraries:  
zexy  
comport  
iemmatrix  
iemguts

The first three libraries are being installed with "apt-get" and iemguts is installed through Deken (In Pd got to Help -> Find externals and type iemguts, the highlighted version(s) is good for your system).


## General note

I'll upload the circuit designs and create a wiki with instructions for creating the hardware of a module and the whole system. It's in my TODO list, which will hopefully happen in the near future.
