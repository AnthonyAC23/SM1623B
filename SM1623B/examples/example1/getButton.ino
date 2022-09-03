/*
  Example for scaning buttons conected a chip
*/
#include <SM1623B.h>
byte keyschip;

SM1623B module(4, 5, 6);// DIO=4, CLK=5, STB=6// using module,dvd,display how names for default, you can using anywere name


void setup() {

  Serial.begin(9600);
  module.DspBegin();// ingrese el brillo requerido valor del 0 - 7, default 3; example "name.DspBegin(5);"
  module.println("button");
  delay(1000);
  module.displayClear();//borra toda la pantalla

}

void loop() {

  keyschip = module.GetButtons();
  if (keyschip != 0) { //or use if (keyschip){...}
    module.displayClear();//if use writeNumber is not necesary
    delay(150);//para evitar el efecto de rebote
    module.print("btt  ");
    module.SegmAll(10, 1); //turn on first ":"
    module.println(keyschip);//or use xxxxx.writeNumber(keyschip);
    Serial.println(keyschip);
    keyschip = 0;
  }
}