## Adafruit TLV493D Triple-Axis Magnetometer PCB

<a href="http://www.adafruit.com/products/4366"><img src="assets/4366.jpg?raw=true" width="500px"><br/>
Click here to purchase one from the Adafruit shop</a>

PCB files for the Adafruit TLV493D Triple-Axis Magnetometer. 

Format is EagleCAD schematic and board layout
* https://www.adafruit.com/product/4366

### Description
The TLV493D 3-axis magnetometer is a great little sensor for detecting magnets [_in 3D_](https://www.youtube.com/watch?v=3FQDUX6X6-w). In fact, the manufacturer Infineon suggests it could be used to make a joystick! You could also use it for other cool things like detecting objects with magnets attached, like the lid of a box, or maybe a statue that unlocks your secret lair when placed on your mantle?

The TLD493D excels at measuring nearby magnetic fields in three dimensions. It's not going to make a good compass, it's not sensitive enough to pick up the Earth's magnetic field, but you can use it to track the movement of nearby magnets in three dimensions. 

Here are a few specs:

 * Digital output via 2-wire based standard I2C interface up to 1 MBit/sec
 * 12-bit data resolution for each measurement direction
 * Bx, By and Bz linear field measurement up to +130 [mT](https://en.wikipedia.org/wiki/Tesla_\(unit\))
 * Excellent matching of X/Y measurement for accurate angle sensing

As we are wont to do, we've made the TLV easy to use by putting it on a breakout PCB along with the circuitry to support it. A voltage regulator and logic level shifting combine to make it usable with a range of microcontrollers with either 3.3V or 5V logic levels. We even give you a pin for you to use the excess 3.3V juice not needed by the TLV493D.

We've also outfitted the breakout with [SparkFun Qwiic](https://www.sparkfun.com/qwiic) compatible [STEMMA QT](https://learn.adafruit.com/introducing-adafruit-stemma-qt) connectors to allow you to use it with other similarly equipped boards without needing to solder. Because the TLV493D "speaks" I2C, you only need two wires (in addition to power) to interface with your microcontroller, and you can share those pins with other I2C sensors. The manufacturer Infineon was kind enough to provide a library to use the sensor with Arduino, and we've written one to use it with CircuitPython and Python for use on Raspberry Pi and other Linux computers.

### License

Adafruit invests time and resources providing this open source design, please support Adafruit and open-source hardware by purchasing products from [Adafruit](https://www.adafruit.com)!

Designed by Kattni Rembor for Adafruit Industries.

Creative Commons Attribution/Share-Alike, all text above must be included in any redistribution. 
See license.txt for additional details.
