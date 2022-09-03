/*
  Example for test of display
*/
#include <SM1623B.h>

SM1623B module(4, 5, 6);// DIO=4, CLK=5, STB=6// using module,dvd,display how names for default, you can using anywere name


void setup() {

  module.DspBegin();// ingrese el brillo requerido valor del 0 - 7, default 3; example "name.DspBegin(5);"
  module.println("tst ALL");//start test
  delay(1000);
  module.displayClear();//borra toda la pantalla

}

void loop() {

  module.displayProbeAll(250);//delay default 150
  //module.clearDisplay();
  module.println("tst End");//end test
  delay(1000);

  /*
    module.displayProbeAll(); equivale a usar... :

    SegmOnly();
    NumSegm();
    Display_one();
    Dspy_All();
    Display_Probe();
    for (byte i = 0; i < 0x07; i++){
    SetLED(i);
    delay(150);
    }
    SetDisk(ON);
    delay(150);
    ... en conjunto.

  */
}