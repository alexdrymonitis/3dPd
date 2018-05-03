# 3dPd

This repository hosts the code, schematics, and circuit board designs made in Kicad, for a hardware digital modular synthesizer. The audio is programmed in Pure Data and the hardware control in the Arduino language.
This synthesizer is supposed to run on a Raspberry Pi 3 and a Teensy3.2. You're welcome to use any other hardware you like, but this specific hardware is supported and explained here.


## Note about the Arduino code

Go to the Arduino_code directory and read the README.txt file first. That should give you enough information about the Arduino code. Do that before you start reading the Pd patches.


## Note about the Pd patches

The heart of this project is the modular_synthesizer.pd patch. Open it and you'll see some other abstractions in there. Go into the [pd modules] sub-patch and look inside.
Each module must have its own abstraction. There are some tutorial module abstractions which show you how to build your own. Open them and read them carefully.

There's also a debug.pd patch which helps with debugging by showing input from the Teensy and printing various message concerning module connections and LED states.

### Dependencies

This project uses the following external libraries:  
zexy  
comport  
iemmatrix  
iemguts  
ggee  
list-abs  
tabreceive_mult~  

The first five libraries are being installed with "apt-get" and the last two are installed through Deken (In Pd got to Help -> Find externals and type these two names -one at a time-, the highlighted version(s) is good for your system).

Pd-0.47.0 or higher is required.


## General note

I'll upload a wiki with instructions for creating the hardware of a module and the whole system. It's in my TODO list, which will hopefully happen in the near future.
You can watch a demo video of the synthesizer here https://www.youtube.com/watch?v=HD13bDPyBWA
