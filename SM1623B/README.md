# SM1623B
Arduino SM1623B library for LED & KEY and similar chips. Simply use print() on 7-segment displays.

## SM1623B LEDs and Buttons library
This Arduino library facilitates driving LED displays using SM1623B LED driver chips.
The SM1623B chip family allows driving 7-segment LED displays or LED matrices.
Next to built-in high-frequency LED multiplexing, they offer control of LED brightness.
Most SM1623B chips also support reading key-scan data for button presses.
Using this library you can simply use print() on a 7-segment display.


Initial version was based on the [TM1628 code](https://www.youtube.com/shorts/ZxlLDDJ6koI) by Baurzhan Duisaliev. Further inspiration from the [TM1637 library](https://github.com/avishorp/TM1637) by Avishay and the [OneButton library](https://github.com/mathertel/OneButton) by Matthias Hertel. 

## Table of contents
- [SM1623B chip features](#SM1623B-chip-features)
- [Library structure](#library-structure)
- [Basic usage](#basic-usage)
- [New in this library](#new-in-this-library)
- [Features & limitations](#features--limitations)
- [More information](#more-information)

## SM1623B chip features

The SM1623B family is quite large. Currently the following chips are supported:

Type    | Segments x digits    | Buttons      | Interface   | Notes
------  | -------------------- | ------------ | ------------|-------------
TM1623B | 10 x 7 - 13 x 4      | 10 x 2 multi | DIN/CLK/STB |
TM1628  | 10 x 7 - 13 x 4      | 10 x 2 multi | DIO/CLK/STB |


\*

## Library structure
This library has a layered structure to simplify the support of multiple SM1623B chips.
By using a base class that provides a uniform API, your application doesn't need chip specific code.
Likewise, the library can offer common functionality in display specific classes that support multiple chips.

The figure below illustrates that concept:

![Layered structure](/images/structure.png)

## Basic usage
To use this library you need to include the class that matches the chip on your module and instantiate the object:
```C++
#include <SM1623B.h>

SM1623B module(4, 5, 6);   // DIO=8, CLK=9, STB=7
```

In the setup() function you can set the intensity of the display, but that's not mandatory:
```C++
void setup() {
  module.DspBegin();// ingrese el brillo requerido valor del 0 - 7, default 3; example "name.DspBegin(5);"
  module.println("HELLO");
  delay(1000);
  module.displayClear();//borra toda la pantalla
}
```

In the loop() function you can use basic display methods provided by the base class:
```C++
void loop() {
  module.print("Count");
  module.SegmAll(10, 2); //enciende los segundos ":"
  module.println(count++);//en cada print incrementa en 1 el valor de count
  delay(100);
  if (count == 100) {
    count = 0;
    module.displayClear();
  }
}
```
For the easy to use print() method and more advance display methods you can use the [_SM1623BDisplay_](#SM1623Bdisplay-class) class.

The SM1623B chip makes it easy to see if a button is pressed.
To check if a button was pressed you can use the getButtons() method:
```C++
  byte keyschip = module.GetButtons();
  Serial.println(keyschip);
```
Please note that while you don't need to write any code for debouncing, the button state may be reset when you display something. For advanced detection of button clicks, double clicks and long presses you can use the [_SM1623BButtons_](#SM1623Bbuttons-class) class.


## New in this library

Added library functionality:
- Support for ESP32 in addition to regular Arduinos.
- Revised library structure to simplify support of other SM1623B chips.
- Basic functionality in base class for a uniform API.
- Simple display of text and numbers on 7-segment and 14-segment displays using familiar print() and println() methods.
- Support for combining multiple 7-segment and 14-segment display modules into one large display.

Functionality in original library by Baurzhan Duisaliev:
- Support for the SM1628 module;
- Use the implementation of asignation of pos and byte write and funtion of send data and command;
- Support for dimming the display and LEDs and for show val;

## Features & limitations
- The current version of this library supports ESP32, Atmel ATmega (e.g. ATmega328 and ATmega168). 
- It is possible to define multiple display objects for multiple different modules (see the SM1623B_TM1637ex_two_modules example). The library now supports combining multiple 7-segment modules into one display using the SM1623BDisplay class.


## More information

### Examples
See the [library examples](/examples) for more information on how to use this library. See also the [examples](https://github.com/AnthonyAC23/SM1623B/tree/main/SM1623B/examples/example1). Most will still work or only require minor changes.

### Links
- show how using this segments of display (https://sites.google.com/site/angmuz/home/proyecto-36-4-arduino-clock-thermometer-dvd-display).
## Disclaimer
- All code on this GitHub account, including this library is provided to you on an as-is basis without guarantees and with all liability dismissed. It may be used at your own risk. Unfortunately I have no means to provide support.
