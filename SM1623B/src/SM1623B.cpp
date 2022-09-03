#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "SM1623B.h"


//-----------------------------Caracteres y disposicion de direcciones

byte CRAM[14] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char s_addr[14] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D}; //{0x03, 0x0b, 0x0d, 0x07, 0x05, 0x09, 0x0d, 0x01};
unsigned char s_byte[10] = {0x44, 0x52, 0x80, 0x40, 0x21, 0x03, 0x04, 0x08, 0x10, 0x21};//{0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x01};

//          Carcteres especiales 1,2 y nùmerico XX3:XX:XX

const byte  Bit_Mask[10] = {0x44, 0x52, 0x08, 0x08, 0x21, 0x08, 0x08, 0x08, 0x08, 0x08}; // {0x44, 0x52, 0x80, 0x40, 0x21, 0x03, 0x04, 0x08, 0x10, 0x21}; //mascara para mostrar los seg faltantes
const byte  RAM_Addr[7] = {0x01, 0x03, 0x05, 0x07, 0x09, 0x0B, 0x0D};  //direccion de segmentos scundarios A-G


//          caracteres numéricos 00X:00:00 - 12X:45:67 - mostrar

const byte  Bit_Mask1[10] = {0x01, 0x01, 0x80, 0x40, 0x01, 0x03, 0x04, 0x08, 0x10, 0x21}; // máscara de bits para incluir el carácter numérico en el seg correspondiente
const byte  RAM_Addr1[7] = {0x00, 0x02, 0x04, 0x06,0x08, 0x0A, 0x0C};// direcciones en la RAM de visualización para segmentos A-G

//para la prueba de encender todos los digitos
const byte  Bit_Mask2[1] = {0xff};

/*
  The bits are displayed by mapping bellow
   -- 0 --      0 = a
  |       |     1 = b
  5       1     5 = f
   -- 6 --      6 = g
  4       2     2 = c
  |       |     4 = e
   -- 3 --      3 = d
*/
const byte Number_Font[38] = {
                              0b00111111,//0 - 1 
                              0b00000110,//1 - 2
                              0b01011011,//2 - 3
                              0b01001111,//3 - 4
                              0b01100110,//4 - 5
                              0b01101101,//5 - 6
                              0b01111101,//6 - 7
                              0b00000111,//7 - 8
                              0b01111111,//8 - 9
                              0b01101111,//9 - 10
                              0b01110111,//A - 11
                              0b01111100,//b - 12
                              0b00111001,//C - 13
                              0b01011110,//d - 14
                              0b01111001,//E - 15
                              0b01110001,//F - 16
                              0b01101111,//G - 17
                              0b01110110,//H - 18
                              0b00110000,//I - 19
                              0b00011110,//J - 20
                              0b00111000,//L - 21
                              0b01010100,//n - 22
                              0b01010101,//ñ - 23
                              0b00111111,//O - 24
                              0b01110011,//P - 25
                              0b01100111,//Q - 26
                              0b01010000,//R - 27
                              0b01101101,//S - 28
                              0b01111000,//t - 29
                              0b00111110,//U - 30
                              0b01101110,//y - 31
                              0b01011000,//c - 32
                              0b00000100,//i - 33
                              0b00110000,//l - 34
                              0b01011100,//o - 35
                              0b00011100,//u - 36
                              0b01000000,//"-" separator 37
                              0b00000000,//empty espace  - 38
};
const byte ctr = 38;//sizeof(Number_Font)/sizeof(byte);       // numero totales de caracteres

//                    a           b          c          d           e         f           g             nada
byte IndSeg[8] = {0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b01000000,0b00000000};
//                   nada        a           b          f           g           d           c          e
byte Disk[8] = {0b00000000, 0b00000001, 0b00000011, 0b00100011, 0b01100011, 0b01101011, 0b01101111, 0b01111111};//{0b00000001, 0b00000010, 0b00100000, 0b01000000, 0b00001000, 0b00000100, 0b00010000};
//                    nada         e           c          d           g           f           b          a
byte pordown[8] = {0b00000000, 0b00010000, 0b00010100, 0b00011100, 0b01011100, 0b01111100, 0b01111110, 0b01111111};


//___________________________________________________________________________________________________________________________                               


SM1623B::SM1623B(byte dataPin, byte clockPin, byte strobePin) {
  this->dataPin = dataPin;
  this->clockPin = clockPin;
  this->strobePin = strobePin;

  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(strobePin, OUTPUT);

  digitalWrite(strobePin, HIGH);
  digitalWrite(clockPin, HIGH);

//en otros ejemplos solo tienen hasta aqui

  SendCmd(0x40);
  SendCmd(0x80);

  digitalWrite(strobePin, LOW);
  Send(0xC0);
  for (int i = 0; i < ctr; i++)//estaba 16
  {
    Send(0x00);
  }
  digitalWrite(strobePin, HIGH);
}

//----------------------------comunicacion-----------------------------

void SM1623B::start()
{	// if needed derived classes can use different patterns to start a command (eg. for TM1637)
  digitalWrite(strobePin, LOW);
  //bitDelay();
}

void SM1623B::stop()
{	// if needed derived classes can use different patterns to stop a command (eg. for TM1637)
  digitalWrite(strobePin, HIGH);
  //bitDelay();
}

void SM1623B::bitDelay()
{	// if needed derived classes can add a delay (eg. for TM1637)
	//delayMicroseconds(50);
  // On fast MCUs like ESP32 a delay is required, especially when reading buttons
  // An ESP32 running without delay at 240MHz gave a CLK of  ~0.3us (~ 1.6MHz)
  #if F_CPU>100000000
  	delayMicroseconds(1);
  #endif
}

//ingrese la cantidad de brillo requerido en un rango del 0 - 7, default = 3;
void SM1623B::DspBegin(byte intensity) {
  SendCmd(0x40);
  SendCmd(0x88 | min(7, intensity));
  Clock();
  //limpieza de memoria
  RAMclear();
}

void SM1623B::RAMclear(){
  start();
  Send(0xC0);
  for (int i = 0; i < ctr; i++)//estaba 16
  {
    Send(0x00);
  }
  stop();
}

void SM1623B::Clear() {
  for (int i=0; i<14; i++) CRAM[i]=0x00;
    nPrintPos = 3;
  DUpdate();
}

void SM1623B::Send(byte data)
{
  for (int i = 0; i < 8; i++) {
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, data & 1 ? HIGH : LOW);
    data >>= 1;
    digitalWrite(clockPin, HIGH);
  }
}


                    /*********** mid level  **********/

void SM1623B::SendData(byte address, byte data){
  SendCmd(0x40); // escribir datos en una celda fija (sin autoincrementación)
  digitalWrite(strobePin, LOW);     //start();
  Send(0xC0 | address);       
  Send(data);
  digitalWrite(strobePin, HIGH);     //stop();
}

void SM1623B::SendCmd(byte cmd){
  digitalWrite(strobePin, LOW);     //start();
  Send(cmd);
  digitalWrite(strobePin, HIGH);    //stop();
}

//-------------/*********** Low level  **********/-------------

byte SM1623B::Receive() {
  byte temp = 0;

  // Pull-up on
  pinMode(dataPin, INPUT);
  digitalWrite(dataPin, HIGH);

  for (int i = 0; i < 8; i++) {
    temp >>= 1;

    digitalWrite(clockPin, LOW);

    if (digitalRead(dataPin)) {
      temp |= 0x80;
    }

    digitalWrite(clockPin, HIGH);
  }

  // Pull-up off
  pinMode(dataPin, OUTPUT);
  digitalWrite(dataPin, LOW);

  return temp;
}

//Solo para uso interno de encendido y apagado distinguiendo como parametro basico el actuar medianante valores binarios
// para procesos que solo tengan que colocar un pin en estado alto o bajo

unsigned long dton = 0;
boolean fdt = true;
void SM1623B::dtime(int dt){
	
  if ((millis() - dton >= dt)) {// && fdt == true
    fdt = !fdt;
    dton = millis();
    }
    else{
      fdt = false;
    }
}
unsigned long animatOn = 0;
boolean animation = true;
void SM1623B::animationdt(int dt){
	
  if ((millis() - animatOn >= dt)) {// && fdt == true
    animation = !animation;
    animatOn = millis();
    }
    else{
      animation = false;
    }
}

//____________________________________________Display_______________________________________________________

//hace uso de un lDR para cambiar el brillo , int LDR = puerto analogico y int dtm = delaywithmillis = tiempo, on/off para print de datos del LDR
void SM1623B::Brillo (byte LDR, int dtm, boolean onoff) {
  dtime(dtm);
  if (fdt == true) {
    Val = analogRead(LDR);
    PWM = map (Val, 1023, 165, 0, 7);//(Val, 255, 4095, 0, 7)  for esp32 se utiliza 4095 dependiendo de la cantidad de bis que tenga la int analog
    if (onoff){
      Serial.println(Val);
      Serial.println(PWM);
    }
    SendCmd(0x88 | min(7, PWM));
  }
}

void SM1623B::DUpdate() {
  for (byte i = 0; i <= 0x0D; i++)
  {
    SendData(i, CRAM[i]);
  }
}

void SM1623B::displayClear(void)
{
  for (int i = 0; i <= 0x0D; i++)
  {
    CRAM[i] = 0;
    SendData(i, CRAM[i]);
  }
}

void SM1623B::clearDisplay()
{	// Clear all data registers. The number of registers depends on the chip.
	// TM1638 (10x8): 10 segments per grid, stored in two bytes. The first byte contains the first 8 display segments, second byte has seg9+seg10  => 16 bytes
	// TM1640 (8x16): one byte per grid => 16 bytes
	// TM1637 (8x6): one byte per grid => 6 bytes
	// TM1668 (10x7 - 14x3): two bytes per grid => 14 bytes
  SendCmd(0x40);		// set auto increment addressing mode
	// send the address followed by bulk-sending of the data to clear the display memory
	start();
  Send(0xC0);
  for (int i = 0; i < 0x0D; i++) {
    Send(0x00);
  }
	stop();

}

void SM1623B::SetSeg(byte addr, byte num) {//sirve, pero solo escoje los digitos 3,4,6,7,8,9,10
  for (int i = 0; i < 7; i++) {
    bitWrite(CRAM[i * 2], RAM_Addr[addr], bitRead(Number_Font[num], i));
    bitWrite(CRAM[i * 2], RAM_Addr1[addr], bitRead(Number_Font[num], i));
    DUpdate();
    //SendData(position << 1, segments);
  }
}

void SM1623B::SetSegments16(uint16_t segments, byte pos)//sirve, pero no de la manera esperada, no detecta todos los digitos
{	// Some modules support more than 8 segments (e.g. 10 max for TM1638)
  // The position of the additional segments in the second data byte can be different per module,
  // For that reason this module has no implementation in the base class.
  // E.g. for TM1638/TM1668 segments 8-9 are in bits 0-1, for TM1630 segment 14 is in bit 5
  // This method assumes segments 0-7 to be in the lower byte and the extra segments in the upper byte
  // Depending on the module this method should shift the segments to the proper data position.
 /*método para enviar más de 8 segmentos (10 máx. para TM1638)
  TM1638 utiliza 10 segmentos en dos bytes, similar a TM1668
  los segmentos 0-7 están en bits 0-7 de bytes de posición 0,2,4,6,8,10,12,14
  los segmentos 8-9 están en bits 0-1 de bytes de posición 1,3,5,7,9,11,13,15*/

		SendData(pos << 1, (byte)segments&0xFF);
		SendData((pos << 1) | 1, (byte)(segments>>8)&0x03);
	
}

byte SM1623B::getNumDigits()
{	// get the number of digits used (needed by SM1623BDisplay to combine modules)
  return(nNumDigits);
}

//no debe de usarse en bucles, solo debe de ejecutarse una sola vez, debido a que si se ejecuta nuevamente se apagaran
void SM1623B::SetLED(byte led){
  CRAM[RAM_Addr[led]] = CRAM[RAM_Addr[led]] ^ Bit_Mask[1 - 1];//[Position - 1];
  DUpdate();
}
/*
//no debe de usarse en bucles, solo debe de ejecutarse una sola vez, debido a que si se ejecuta nuevamente se apagaran
void SM1623B::SetLEDon(byte led){
  //CRAM[RAM_Addr[led]] = CRAM[RAM_Addr[led]] | Bit_Mask[1 - 1];
  CRAM[RAM_Addr[led]] = CRAM[RAM_Addr[led]] ^ Bit_Mask[4 - 1];
  DUpdate();
}

void SM1623B::SetLEDoff(byte led){
  CRAM[RAM_Addr[led]] &= 0xFF - Bit_Mask[led];//= CRAM[RAM_Addr[led]] | Bit_Mask[led];
  DUpdate();
}*/

//+++++++++++++++++++++++++++++++++++++++Funcion que me permite imprimir text en el Display+++++++++++++++++++++++++++++

//escritura en forma string;

void SM1623B::writeString(byte Position, byte Value){
  byte temp6;

  temp6 = FONT_DEFAULT1[Value];
  for (byte i = 0; i < 0x07; i++)//el 0x07 es por los 7 segmentos
  {
    if ( ((temp6 & 0B00000001) == 1 ))
    {
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] | Bit_Mask[Position - 1];
    }
    else
    {
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] | Bit_Mask[Position - 1];
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] ^ Bit_Mask[Position - 1];
    }


    if ( ((temp6 & 0B00000001) == 1 ))
    {
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] | Bit_Mask1[Position - 1];
    }
    else
    {
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] | Bit_Mask1[Position - 1];
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] ^ Bit_Mask1[Position - 1];
    }
    temp6 = temp6 >> 1;
  }
}

//permite establecer el cursor de la pantalla en x
void SM1623B::setCursor(int8_t nPos)
{		// Establezca la posición de impresión. Permitir que los números negativos admitan el desplazamiento
	nPrintPos=nPos;
}


size_t SM1623B::write(uint8_t c)
{	//Código para mostrar la letra cuando se le da el código ASCII para ello
	static uint8_t cPrevious=' ';		// recuerde el último carácter prnted, para agregar un punto cuando sea necesario
  // manejar el final de la línea
	if(c=='\0' || c=='\n' || c=='\r' || nPrintPos>= nNumDigits)
	{
		while(nPrintPos>3 && nPrintPos<nNumDigits)
		{	//Borrar el resto de la línea

			//Serial.println(nPrintPos);
			nPrintPos++;
		}

    // MMOLE 211103: Devolver cero de la escritura debería dejar de imprimir el resto de la cadena.
    // Sin embargo, en ESP32 print() no se detendrá al devolver 0, por lo que para mayor compatibilidad devolvemos 1.
    // y solo restablezca la posición de impresión cuando estemos al final. Esto debería funcionar en todas las plataformas.
    if(c=='\0' || c=='\n' || c=='\r')
  		nPrintPos=3;
		return(1);

	}
	bool fDot=false;
	if(c=='.' || c==',' || c==':' || c==';')
	{
		c=cPrevious;
		fDot=true;
		if(nPrintPos>3) nPrintPos--; // use same position to display the dot
	}
	if(nPrintPos>=3 && nPrintPos<nNumDigits)
    //Writex(c,nPrintPos);
    writeString(nPrintPos,c - 0x20);//funcion que permite obtener los valores de digitos y colocar un caracter, el "-0x20" es escencial
    DUpdate();
	cPrevious=c;
	nPrintPos++;
	return(1);
}

//___----------_________________Funciones personalizadas y de pruebas_-____---___-_-_-_-____---_-----_-_

void SM1623B::ShowNumber(byte Position, byte Value)
{
  byte temp;

  temp = Number_Font[Value];
  for (byte i = 0; i < 0x07; i++)//el 0x07 es por los 7 segmentos
  {
    if ( ((temp & 0B00000001) == 1 ))
    {
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] | Bit_Mask[Position - 1];
    }
    else
    {
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] | Bit_Mask[Position - 1];
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] ^ Bit_Mask[Position - 1];
    }


    if ( ((temp & 0B00000001) == 1 ))
    {
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] | Bit_Mask1[Position - 1];
    }
    else
    {
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] | Bit_Mask1[Position - 1];
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] ^ Bit_Mask1[Position - 1];
    }
    temp = temp >> 1;
  }
}
//            Segmento por digito
void SM1623B::SPD (byte Position, byte Value) {
  byte temp;
  temp = Number_Font[Value];
  for (byte i = 0; i < 0x07; i++)
  {
    if ( ((temp & 0B00000001) == 1 ))
    {
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] | Bit_Mask2[Position - 1];

    }
    else
    {
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] | Bit_Mask2[Position - 1];
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] ^ Bit_Mask2[Position - 1];

    }

    if ( ((temp & 0B00000001) == 1 ))
    {
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] | Bit_Mask2[Position - 1];
    }
    else
    {
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] | Bit_Mask2[Position - 1];
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] ^ Bit_Mask2[Position - 1];
    }


    temp = temp >> 1;
  }
}

void SM1623B::Display_one(int dt) {
  for (byte i = 1; i < 11; i++)
  {
    for (byte j = 0; j < ctr; j++)
    {
      ShowNumber(i, j);
      DUpdate();
      delay(dt);
    }
  }
}

void SM1623B::displayProbeAll(int dt){

SegmOnly(dt);
NumSegm(dt);
Display_one(dt);
Dspy_All(dt);
Display_Probe(dt);
for (byte i = 0; i < 0x07; i++){
  SetLED(i);
  delay(dt);
}
SetDisk(ON);
delay(dt);
}

void SM1623B::Display_Probe(int dt) {
  // Muestreo de los espacios y letras
    for (byte j = 0; j < ctr; j++)
    {
      SPD(1, j);
      DUpdate();
      delay(dt);
    }
}

void SM1623B::Dspy_All(int dt) {

  SPD(1, 8);
  DUpdate();
  delay(dt);

}

void SM1623B::ShowOn() {
  
  ShowNumber(6, 0);
  ShowNumber(7, 21);
  DUpdate();

}
void SM1623B::ShowOff() {
  
  ShowNumber(5, 0);
  ShowNumber(6, 15);
  ShowNumber(7, 15);
  DUpdate();

}

void SM1623B::ShowSpeaker() {
  ShowNumber(3, 27);
  ShowNumber(4, 24);
  ShowNumber(5, 14);
  ShowNumber(6, 10);
  ShowNumber(7, 25);
  ShowNumber(8, 14);
  ShowNumber(9, 26);
  DUpdate();

}

void SM1623B::ShowHora() {
  
  ShowNumber(4, 17);
  ShowNumber(5, 0);
  ShowNumber(6, 26);
  ShowNumber(7, 10);
  DUpdate();

}

void SM1623B::ShowLED() {
  
  ShowNumber(4, 20);
  ShowNumber(5, 14);
  ShowNumber(6, 13);
  DUpdate();

}

void SM1623B::ShowSetting(){

  ShowNumber(3, 27);
  ShowNumber(4, 14);
  ShowNumber(5, 28);
  ShowNumber(6, 28);
  //ShowNumber(7, 18);
  SegmAll(7,4);
  ShowNumber(8, 21);
  ShowNumber(9, 16);
  DUpdate();
  
}
void SM1623B::ShowStby(){

  ShowNumber(3, 27);
  ShowNumber(4, 28);
  ShowNumber(5, 10);
  ShowNumber(6, 21);
  ShowNumber(7, 13);
  ShowNumber(8, 11);
  ShowNumber(9, 30);
  DUpdate();

}

void SM1623B::ShowHello(void) {

  ShowNumber(4, 17);
  ShowNumber(5, 14);
  ShowNumber(6, 20);
  ShowNumber(7, 20);
  ShowNumber(8, 0);
  DUpdate();

}

void SM1623B::ShowPelusa(void) {

  ShowNumber(3, 24);
  ShowNumber(4, 14);
  ShowNumber(5, 20);
  ShowNumber(6, 29);
  ShowNumber(7, 27);
  ShowNumber(8, 10);
  DUpdate();

}

void SM1623B::ShowAlert() {

  ShowNumber(4, 10);//A
  ShowNumber(5, 20);//l
  ShowNumber(6, 14);//E
  ShowNumber(7, 26);//r
  ShowNumber(8, 28);//t
  ShowNumber(9, 10);
  DUpdate();

}

void SM1623B::ShowAlt(){

  ShowNumber(3, 10);//A
  ShowNumber(4, 20);//l
  ShowNumber(5, 28);//t
  SegmAll(10,0);
  DUpdate();

}
void SM1623B::Showtop(){
  ShowNumber(3, 28);//t
  ShowNumber(4, 0); //O
  ShowNumber(5, 24);//p
  SegmAll(10,1);
  DUpdate();

}

void SM1623B::writext(const byte vec[], byte pos, byte size){//permite escribir text en la pantalla por medio del usode arrays
  byte iter = 0;
  //displayClear();
  if (pos < 3) iter = 3;
  else iter = pos;
  for (byte j = 0; j < size; j++){
    ShowNumber(iter, vec[j]);
    DUpdate();
    iter++;
    if (iter > 9) break;
    //delay(1000);
  }
}

void SM1623B::NumSegm (int dt) {
  // encendido y conteo de los espacios y letras
  int desp = 1;
  for (byte i = 1; i < 11; i++)
  {
    for (byte j = 0; j < desp; j++)
    {
      ShowNumber(i, j);
      DUpdate();
      //Brillo();
      delay(dt);
      Serial.println(i);
      Serial.print(j);
    }
    desp++;
    if (desp == 11) {
      desp = 1;
    }
  }
}

void SM1623B::SegAll (void) {
  int segm = 0;
  for (byte i = 1; i < 11; i++)
  {

  }
}

void SM1623B::SegmAll(byte addr, byte num) {
  byte temp1;

  temp1 = IndSeg[num];
  for (byte i = 0; i < 0x07; i++)//el 0x07 es por los 7 segmentos
  {
    if ( ((temp1 & 0B00000001) == 1 ))
    {
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] | Bit_Mask[addr - 1];
    }
    else
    {
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] | Bit_Mask[addr - 1];
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] ^ Bit_Mask[addr - 1];
    }


    if ( ((temp1 & 0B00000001) == 1 ))
    {
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] | Bit_Mask1[addr - 1];
    }
    else
    {
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] | Bit_Mask1[addr - 1];
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] ^ Bit_Mask1[addr - 1];
    }
    temp1 = temp1 >> 1;
  }
}

void SM1623B::SegmOnly(int dt) {//using delay for this function

  for (byte i = 1; i < 11; i++)
  {
    for (byte j = 0; j < 0x07; j++)
    {
      SegmAll(i, j);
      DUpdate();
      delay(dt);
      //      Serial.println(i);
      //      Serial.print(j);
    }
  }
}
//using millis for this function
byte upseg = 0;
byte updig = 1;
unsigned long timesegxseg = 0;
void SM1623B::SegtoSegDigit(int dt) {
  SegmAll(updig, upseg);
  if (millis() - timesegxseg > dt){
    upseg++;
    timesegxseg = millis();
    if (upseg == 7){
      updig++;
      upseg = 0;
    }
    DUpdate();
    if (updig == 11) updig = 1;
  }

}

boolean flgb = true;
unsigned long Blinkoff = 0;
unsigned long SegBlink = 0;

void SM1623B::SecondsSeg(int countime) {
  //  boolean flgb = true;

  if ((millis() - SegBlink >= countime) && flgb == true) {
    flgb = !flgb;
    Blinkoff = millis();
    ShowNumber(10, 1);
  }
  else if ((millis() - Blinkoff >= countime) && flgb == false) {
    SegBlink = millis();
    ShowNumber(10, 31);
    flgb = !flgb;
  }
  DUpdate();
}

unsigned long TimeUpdate = 0;
byte hora;
byte minu;
byte segu;
byte cdc = 0; //contador de correccion 

void SM1623B::SetTime(byte hour, byte min, byte sec) {

  //if (hour >= 100) ShowNumber(3, (hour / 100));
  //if (hour >= 10) 
  ShowNumber(4, (hour / 10));//% 10)
  ShowNumber(5, (hour % 10));//% 10);
  ShowNumber(6, (min / 10));
  ShowNumber(7, (min % 10));
  ShowNumber(8, (sec / 10));
  ShowNumber(9, (sec % 10));
  DUpdate();
}

void SM1623B::SetTime1(byte hour, byte min) {
  ShowNumber(6, (hour / 10));
  ShowNumber(7, (hour % 10));
  ShowNumber(8, (min / 10));
  ShowNumber(9, (min % 10));

  DUpdate();
}

void SM1623B::Clock(){
if (millis() >= TimeUpdate + 1000) {
    TimeUpdate = millis();
    RTime();
  }
}

void SM1623B::ShowCLK1(void) {
  Clock();
  SetTime1(hora, minu);
  SecondsSeg1(500);
}

void SM1623B::SecondsSeg1(int countime) {
  SegAll();
  if ((millis() - SegBlink >= countime) && flgb == true) {
    flgb = !flgb;
    Blinkoff = millis();
    SegmAll(10, 2);
  }
  else if ((millis() - Blinkoff >= countime) && flgb == false) {
    SegBlink = millis();
    SegmAll(10, 6);
    flgb = !flgb;
  }
  DUpdate();
}

void SM1623B::ShowCLK(void) {
  Clock();
  SetTime(hora, minu, segu);
  ShowNumber(10,1);
  //SecondsSeg(350);
}

void SM1623B::Changtime(byte hh, byte mm, byte ss){
hora = hh;
minu = mm;
segu = ss;
}


void SM1623B::RTime () {//int hora, int minuto, int segundo
  segu++;
  if (segu >= 60) {
    segu = 0;
    minu++;
    cdc++;
   // interval++;
  }
  if (minu >= 60) {
    minu = 0;
    hora++;
  }
  if (hora >= 24) {
    hora = 0;
  }
  if (cdc == 10){
    cdc = 0;
    segu++;
  }
}

void SM1623B::Uphour() {
hora++;
}
void SM1623B::Downhour() {
hora--;
}
void SM1623B::Upmin() {
minu++;
}
void SM1623B::Downmin() {
minu--;
}
void SM1623B::Upsec() {
segu++;
}
void SM1623B::Downsec() {
segu--;
}

void SM1623B::SetDisk(boolean onoff) {
  if (onoff) {
    for (int i = 0; i < 7; i++) // turn on all disc segments excepto el segmento7 del dig 1
      //CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] | Bit_Mask[i - 1];
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] ^ Bit_Mask[2 - 1];
      SetLED(6);//enciende el segmento 7 del dig 1
  } else {
    for (int i = 0; i < 7; i++)
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] & (0xFF - Bit_Mask[2 - 1]);
      //SetLED(6);//enciende el segmento 7 del dig 1
  }
  DUpdate();
}
//addr es para el segmento a escojer (1-10) y num es el numero de segmentos a encender
void SM1623B::Porcent_disk(byte addr, byte num) {
  byte temp2;
  temp2 = Disk[num];
  for (byte i = 0; i < 0x07; i++)//el 0x07 es por los 7 segmentos
  {
    if ( ((temp2 & 0B00000001) == 1 ))
    {
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] | Bit_Mask[addr - 1];
    }
    else
    {
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] | Bit_Mask[addr - 1];
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] ^ Bit_Mask[addr - 1];
    }


    if ( ((temp2 & 0B00000001) == 1 ))
    {
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] | Bit_Mask1[addr - 1];
    }
    else
    {
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] | Bit_Mask1[addr - 1];
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] ^ Bit_Mask1[addr - 1];
    }
    temp2 = temp2 >> 1;
  }
}


void SM1623B::PorcentDown(byte addr, byte num) {
  byte temp3;
  temp3 = pordown[num];
  for (byte i = 0; i < 0x07; i++)//el 0x07 es por los 7 segmentos
  {
    if ( ((temp3 & 0B00000001) == 1 ))
    {
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] | Bit_Mask[addr - 1];
    }
    else
    {
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] | Bit_Mask[addr - 1];
      CRAM[RAM_Addr[i]] = CRAM[RAM_Addr[i]] ^ Bit_Mask[addr - 1];
    }


    if ( ((temp3 & 0B00000001) == 1 ))
    {
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] | Bit_Mask1[addr - 1];
    }
    else
    {
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] | Bit_Mask1[addr - 1];
      CRAM[RAM_Addr1[i]] = CRAM[RAM_Addr1[i]] ^ Bit_Mask1[addr - 1];
    }
    temp3 = temp3 >> 1;
  }
}

void SM1623B::circle(int laps, byte inc) {

  if (inc <= 8) {
    inc = inc;
  }
  else {
    inc = 8;
  }
  if (laps == 0){
    byte resd = inc - 1;
    Porcent_disk(2, resd);
    if (inc == 7) {
      SegmAll(1, 6);
    }
  }
  else {
  for (byte j = 0; j < inc; j++)
  {
    Porcent_disk(2, j);
    if (j >= 7) {
      SegmAll(1, 6);
    }
    DUpdate();
    delay(laps);
  
    Serial.print(j);
  }
  Serial.println(" ");
}
}
//se debe crear otra funcion para poder poder hacer el efecto de decremeto y que quede en rojo. seria como porcent disk, pero cambiando la fuente
void SM1623B::Upcircle(int dt, byte fall){//siento que ese 7 que esta ahi es innecesario, si se lo podria elimiar estaria nice!!
  byte lim = fall;
  if (fall <= 7){
    fall = fall;
  }
  else {
    fall = 7;
  }
  if ((dt == 0) || (lim == 8)){
    Porcent_disk(2, fall);
    if (lim == 8) {
      SegmAll(1, 6);
    }
    else {
      SegmAll(1, 7);
    }
  }
  else {
    
    for (fall;fall < 0x08; fall++){
    Porcent_disk(2,fall); 
    DUpdate();
    delay(dt);//hay que quitarle ese dalay y solo colocarle el delaytime(); prueba 1 falló
    if (fall >= 7){
    SegmAll(1,6);
    }
    }
  }
 
}
/* /intento de reemplzar el delay #1 fallido
//intento #2
void SM1623B::Upcircle(int dto, byte fall){
  byte lim = fall;
  if (fall <= 7){
    fall = fall;
  }
  else {
    fall = 7;
  }
  if (dto == 0){
    Porcent_disk(2, fall);
    if (lim == 8) {
      SegmAll(1, 6);
    }
  }
  else {
   /* animationdt(dto);
    if (animation == true){
      fall++;
      Porcent_disk(2,fall); 
      DUpdate();
    if ((lim == 8) || (fall >=7)){
    SegmAll(1,6);
    }
      if (fall == 8){
      fall = fall;
    }
    }*asterisco/
   iteration(7,dto);
   Porcent_disk(2,countiteration); 
   DUpdate();
   if ((lim == 8) || (countiteration >=7)){
    SegmAll(1,6);
    }
      if (countiteration == 8){
      countiteration = countiteration;
    }
  }
 
}*/

void SM1623B::Downcircle(byte decm){
  
  byte jota = decm;

  if (decm <= 7) {
    decm = decm;
  }
  else {
    decm = 7;
  }
     PorcentDown(2, decm);
    if (jota == 8) {
      SegmAll(1, 6);
    }
    else {
      SegmAll(1, 7);
    }
    DUpdate();
  /*for (byte j = 0; j < 0x07; j++){
    PorcentDown(2,j);
    
    DUpdate();
    delay(dt);//hay que quitarle ese delay y solo colocarle el delaytime();
    if (j >= 0x06){
    SegmAll(1,6);
    DUpdate();
    }
    }*/
}


void SM1623B::Forporcent(byte number){
  if (number >= 100){
    if (number >= 100) ShowNumber(3, (number / 100));
    if (number >= 10) ShowNumber(4, (number % 100) / 10);
    ShowNumber(5, (number % 100) % 10);
  }   
  if (number < 100) {
    if (number >= 10) ShowNumber(3, (number % 100) / 10);
    ShowNumber(4, (number % 100) % 10);
    SegmAll(5,6);
  }
  DUpdate();
}

//numero entre 0 y 255, si desea cambiar el tamaño del numero introducido cambie el tipo de variable a "int"
void SM1623B::writeNumber(byte number){
  if (number >= 100){
    if (number >= 100) ShowNumber(7, (number / 100));
    if (number >= 10) ShowNumber(8, (number % 100) / 10);
    ShowNumber(9, (number % 100) % 10);
  }   
  if (number < 100) {
    if (number >= 10) ShowNumber(8, (number % 100) / 10);
    ShowNumber(9, (number % 100) % 10);
    SegmAll(7,7);//limpiar el valor anterior
    SegmAll(10,3);//encender el indicador de medida en "cm"
  }
  DUpdate();
}

///este retorna todos los valores de todos los botones conectados al chip
byte SM1623B::GetButtons() {
  byte keys = 0;

  digitalWrite(strobePin, LOW);
  Send(0x42);
  for (int i = 0; i < 5; i++) {
    keys |= Receive() << i;
  }
  digitalWrite(strobePin, HIGH);

  return keys;
}

byte lastpos = 0;

size_t SM1623B::Write(int8_t chr){//funciona pero no es usable precisamente para el chip SM1623B
  if(nPrintPos<10) {//Start<0x07
    SetChar(nPrintPos, chr);//SetChar(Start, chr);
    nPrintPos++;//Start++;
  }
}

void SM1623B::SetChar(byte addr, byte chr) {
  for(int i=0; i<7; i++){
    bitWrite(CRAM[i * 2], RAM_Addr1[addr], bitRead(FONT_DEFAULT1[chr - 0x20], i));
    bitWrite(CRAM[i * 2], RAM_Addr[addr], bitRead(FONT_DEFAULT1[chr - 0x20], i));
      //SetSegments(chr | 0b10000000 , addr);
    }
  DUpdate();
}
/*
//retorna en valores decimales muy altos, pero solo reconoce el k1 y el k4
uint32_t SM1623B::getBtt()
{
  // TM1638 returns 4 bytes/8 nibbles for keyscan. Each byte has K3, K2 and K1 status in lower bits of each nibble for KS1-KS8
  // NOTE: K1/K2 are implemented for this class, but the TM1638 LED&KEY module only uses K3.
  //       Also note that the buttons are wired in odd sequence: S1=KS1, S5=KS2, S2=KS3, S6=KS4, S3=KS5, S7=KS6, S4=KS7, S8=KS8
  byte keys_K1 = 0;
  byte keys_K2 = 0;
  byte keys_K3 = 0;

  digitalWrite(strobePin, LOW);
  Send(0x42); // B01000010 Read the key scan data

  for (int i = 0; i < 4; i++)
  {
    byte rec = Receive();
    keys_K1 |= ((rec & _BV(2)) >> 2 | (rec & _BV(6)) >> 5) << (2 * i); // bits 2 and 6 for K1/KS1 and K1/KS2
    keys_K2 |= ((rec & _BV(1)) >> 1 | (rec & _BV(5)) >> 4) << (2 * i); // bits 1 and 5 for K2/KS1 and K2/KS2
    keys_K3 |= ((rec & _BV(0))    | (rec & _BV(4)) >> 3) << (2 * i); // bits 0 and 4 for K3/KS1 and K3/KS2
  }

  digitalWrite(strobePin, HIGH);

  return ((uint32_t)keys_K3 << 16 | (uint32_t)keys_K2 << 8 | (uint32_t)keys_K1);
}

uint32_t SM1623B::getButtons()
{	// return state of up to 32 keys.
	return(0);
}
*/
//constantes para el uso de un buzzer
int laptime = 0;
const float pi = 3.14159;

//añada el pin de salida y el tiempo o "frecuencia" necesitado(pin,"frecuencia",tiempo(ms))
void SM1623B::beepbuzzer(byte buzz, int frec, int dt){
  int contador = (dt*8*pi) / frec;
  while  (laptime <= contador) {
    digitalWrite(buzz, HIGH);
    delayMicroseconds(frec);
    digitalWrite(buzz, LOW);
    delayMicroseconds(frec);
    laptime++;
  }
laptime = 0;
}
//funcion (pin,"frcuencia",repeticiones,"tiempo",pausa (usa 100)),//frecuencia y tiempo siempre seran relativos y una depende de la otra
void SM1623B::multiBeep(byte buzz, int frec, byte numrepeat, int dt) {
  int contador = (dt*8*pi) / frec;
  byte repeat = 1;
  while  (laptime <= contador) {
    digitalWrite(buzz, HIGH);
    delayMicroseconds(frec);
    digitalWrite(buzz, LOW);
    delayMicroseconds(frec);
    laptime++;
    if ((laptime == contador) && (repeat < numrepeat )) {
      repeat++;
      delay(100);
      laptime = 0;
    }
  }
  laptime = 0;
}
/*//multibeep sin delay
unsigned long timeBuzzerOn;
unsigned long blablabla;
//funcion (pin,"frcuencia",repeticiones,"tiempo",pausa (usa 100)),
void SM1623B::multiBeep(byte buzz, int frec, byte numrepeat, int dt) {
  int contador = (dt*8*pi) / frec;
  byte repeat = 1;
  while  (laptime <= contador) {
    if (micros() - timeBuzzerOn >= dt){
    digitalWrite(buzz, !digitalRead(buzz));
    //delayMicroseconds(frec);
    //digitalWrite(buzz, LOW);
    //delayMicroseconds(frec);
    laptime++;
    }
    if ((laptime == contador) && (repeat < numrepeat )) {
      //repeat++;
      //delay(100);
      //laptime = 0;
      if (millis() - blablabla >= 100){
        repeat++;
        laptime = 0;
        blablabla = millis();
      }
    }
  }
  laptime = 0;
}*/

unsigned long timecount;
//max itertion 255 , because using byte, (limite, tiempo), using default 7
void SM1623B::iteration(byte numiter, unsigned long retard){
  if (millis() - timecount >= retard){
    countiteration++;
  if (countiteration == numiter) countiteration = 0;
  dton = millis();
    //digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
    //Serial.println(countiteration); 
  }
}


//funciones para escribir , prueba #1**********************************************************************************************

size_t SM1623B::Writex(int8_t chr, byte pos){//funcionax,/escribe letra por letra 
    //SetChar1(pos, chr);//SetChar(Start, chr);
    if(pos<0x07) {//Start<0x07
    writeString(pos,chr - 0x20);
    DUpdate();
    pos++;//Start++;
    if (pos != lastpos){
      Serial.println(pos);
      lastpos = pos;
    }
  }
}

void SM1623B::setToString(const char* string, const byte pos){
  for (int i = 3; i < 10 - pos; i++) {
  	if (string[i] != '\0') {
      writeString(i + pos, string[i - 32]);
      DUpdate();
		} else {
		  break;
		}
  }
}




