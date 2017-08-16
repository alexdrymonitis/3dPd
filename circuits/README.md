# Circuit designs, schematics, and gerber files directory

This README file provides only very basic information about the circuits. I'm planning on releasing a wiki on how to build hardware modules in the next few days.

## Generic note about this directory

In this directory you'll find all the circuits used in this modular synthesizer. It's six circuits in total, each in its own directory. They have been designed in Kicad and all the necessary files are here. Each directory includes a README file which states which extra built-in libraries have been used. The ones I have made especially for these circuits are not mentioned in the README since their directories (library/ and .pretty/) are included here. The directories also include the gerber files for sending them to a manufacturer, plus a .png file for each side (in case of double-sided boards) in case you want to DIY etch it, and a .pdf with the schematic.

If you've never used Kicad before and want to modify any of the circuits, I recommend to read some of the tutorials provided from Kicad's website. This tutorial was very useful for me http://kicad-pcb.org/help/documentation/#_getting_started. If you just want to have them manufactured, you can send the gerber files straight to a manufacturer.

## Note about the circuits

### Parts

There's a parts list in every circuit directory with all the parts of that circuit. Where I thought it was necessary, I provide a link to the datasheet of that part. Parts are identified by their annotation from the schematic. Consult the board design (Pcbnew) to make sure which part goes where.

### Orientation of parts

The orientation of the ICs on the "module_generic_top_board" is not defined by the shape of the footprint part but by the pin numbering. If you're not sure, open Pcbnew, go to View -> 3D Viewer and there you can see the notches of the ICs which indicate their orientation.
The orientation of the IDC connectors can be defined by the pin numbering. On an actual IDC connector, you can see a small arrow at the left side of the notch. This arrow indicates that the left-most pin on the side of the notch, is pin number 1.

### Concerning manufacturing

The boards "module_generic_top_board" and "module_generic_bottom_board" are needed for every module. I would suggest to order these from a manufacturer instead of etching them your self many times. The "module_specific_board" is just an example of how you can connect your parts to the generic boards (this will be analyzed in the upcoming wiki). You can order few of these as well, since it is designed in such a way that can host many different layouts.

The "power_bus", "power_distributor", and "teensy_motherboard" are designed as single-side circuit boards (with the "teensy_motherboard" using three jumpers, the "power_bus" only one, and the "power_distributor" none) so that it's easier to etch them yourself. You'll need only one per board (unless you want to build more than 16 modules, then you'll need a second "power_bus", explained in the upcoming wiki), so ordering those from a manufacturer might not work out.

Eacn board has a .pdf file which you can print for DIY etching, either with toner transfer or with photo-resistant boards. The "module_generic_top_board", "module_generic_bottom_board", and "module_specific_board" have two .pdf, one for each side, while the rest three have only one .pdf file.
