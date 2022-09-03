/*
  Example for using of print
*/
#include <SM1623B.h>
int count = 0;

SM1623B module(4, 5, 6);// DIO=4, CLK=5, STB=6// using module,dvd,display how names for default, you can using anywere name


void setup() {

  module.DspBegin();// ingrese el brillo requerido valor del 0 - 7, default 3; example "name.DspBegin(5);"
  module.println("HELLO");
  delay(1000);
  module.displayClear();//borra toda la pantalla

}

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
