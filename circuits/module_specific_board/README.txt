This board is a rather generic design for mounting potentiometers and wiring banana terminals, switches and LEDs to a module. Each module will have its own layout, so the board that hosts the parts of the module is rather specific than generic. Still, this board can host quite some different layouts the way it is designed.

The digital inputs and outputs are broken out to the right side and bottom side of the board respctively. The inputs start counting from the top and the outputs from left to right.

Four potentiometers are placed on this board which are connected to the first four multiplexer inputs. The other four are broken out on the right side of the top header, where the top pin of the four-pin header is mux pin 8, and the bottom is mux pin 5. I have included 10K pull-down resistors in case you run out of digital inputs but have some space multiplexer pins. In this case you can attach a pull-down resistor to one of these pins (or all four) and use it for a switch.
