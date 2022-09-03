/*
  Example for Show time or clock
*/
#include <SM1623B.h>

SM1623B module(4, 5, 6);// DIO=4, CLK=5, STB=6// using module,dvd,display how names for default, you can using anywere name


void setup() {

  Serial.begin(9600);
  module.DspBegin();// ingrese el brillo requerido valor del 0 - 7, default 3; example "name.DspBegin(5);"
  module.println("Clock");
  delay(1000);
  module.displayClear();//borra toda la pantalla
  module.Changtime(22, 33, 44); //sintaxis (hour, min, second)
}

void loop() {
  module.ShowCLK();//show hour,min,sec
  //module.ShowCLK1();//show hour,min
}