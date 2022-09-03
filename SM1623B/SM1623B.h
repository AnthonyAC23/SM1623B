/*
Library for TM1623B, SM1623B and similar chips.
Esta programacion esta basada en varias librerias, pero el funcionamiento en general ha sido posible gracias a:
Copyright (C) Baurzhan Duisaliev (https://www.youtube.com/shorts/ZxlLDDJ6koI), donde se muestra un ejemplo y en los comentarios
deja el codigo.
Modified by Anthony AC. See https://github.com/AnthonyAC23/SM1623B

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/



#ifndef SM1623B_h
#define SM1623B_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


//flags for turn on/off
#define ON true
#define OFF false
#define on true
#define off false
#define Brigth1 3
#define dtimedelay 150
//datos para el uso de PRINT
const byte FONT_DEFAULT1[] = {
  0b00000000, // (32)  <space>
  0b10000110, // (33)  !
  0b00100010, // (34) "
  0b01111110, // (35) #
  0b01101101, // (36) $
  0b00000000, // (37) %
  0b00000000, // (38) &
  0b00000010, // (39) '
  0b00110000, // (40) (
  0b00000110, // (41) )
  0b01100011, // (42) *
  0b00000000, // (43) +
  0b00000100, // (44) ,
  0b01000000, // (45) -
  0b00001000, // (46) .
  0b01010010, // (47) /
  0b00111111, // (48) 0
  0b00000110, // (49) 1
  0b01011011, // (50) 2
  0b01001111, // (51) 3
  0b01100110, // (52) 4
  0b01101101, // (53) 5
  0b01111101, // (54) 6
  0b00100111, // (55) 7
  0b01111111, // (56) 8
  0b01101111, // (57) 9
  0b00001001, // (58) :
  0b00000101, // (59) ;
  0b00000000, // (60) <
  0b01001000, // (61) =
  0b00000000, // (62) >
  0b01010011, // (63) ?
  0b01011111, // (64) @
  0b01110111, // (65) A
  0b01111111, // (66) B
  0b00111001, // (67) C
  0b00111111, // (68) D
  0b01111001, // (69) E
  0b01110001, // (70) F
  0b00111101, // (71) G
  0b01110110, // (72) H
  0b00000110, // (73) I
  0b00011111, // (74) J
  0b01101001, // (75) K
  0b00111000, // (76) L
  0b00010101, // (77) M
  0b00110111, // (78) N
  0b00111111, // (79) O
  0b01110011, // (80) P
  0b01100111, // (81) Q
  0b00110001, // (82) R
  0b01101101, // (83) S
  0b01111000, // (84) T
  0b00111110, // (85) U
  0b00101010, // (86) V
  0b00011101, // (87) W
  0b01110110, // (88) X
  0b01101110, // (89) Y
  0b01011011, // (90) Z
  0b00111001, // (91) [
  0b01100100, // (92) \ (this can't be the last char on a line, even in comment or it'll concat)
  0b00001111, // (93) ]
  0b00000000, // (94) ^
  0b00001000, // (95) _
  0b00100000, // (96) `
  0b01011111, // (97) a
  0b01111100, // (98) b
  0b01011000, // (99) c
  0b01011110, // (100)  d
  0b01111011, // (101)  e
  0b00110001, // (102)  f
  0b01101111, // (103)  g
  0b01110100, // (104)  h
  0b00000100, // (105)  i
  0b00001110, // (106)  j
  0b01110101, // (107)  k
  0b00110000, // (108)  l
  0b01010101, // (109)  m
  0b01010100, // (110)  n
  0b01011100, // (111)  o
  0b01110011, // (112)  p
  0b01100111, // (113)  q
  0b01010000, // (114)  r
  0b01101101, // (115)  s
  0b01111000, // (116)  t
  0b00011100, // (117)  u
  0b00101010, // (118)  v
  0b00011101, // (119)  w
  0b01110110, // (120)  x
  0b01101110, // (121)  y
  0b01000111, // (122)  z
  0b01000110, // (123)  {
  0b00000110, // (124)  |
  0b01110000, // (125)  }
  0b00000001, // (126)  ~
};

#include "Print.h"

class SM1623B : public Print
{
  public:
  // inicializar el modulo
  SM1623B(byte dataPin, byte clockPin, byte strobePin);
  void DspBegin(byte intensity = Brigth1);//100% necesario, solo se ejecuta 1 vez en el void setup
  void RAMclear();
  void DUpdate();
  void Clear();
  void displayClear();
  void clear();//virtual error xq habian disque funciones virtuales ,,
  void clearDisplay();
  void Brillo(byte LDR, int dtm, boolean onoff = OFF);
  void SetSegments16(uint16_t segments, byte pos);
  void SetChar(byte addr, byte chr);
  virtual size_t Write(int8_t chr);
  void SetTime(byte hour, byte min, byte sec);
  void SetLED(byte led);
  //void SetLEDon(byte led);
  //void SetLEDoff(byte led);
  void SetDisk(boolean onoff);
  void ShowNumber(byte Position, byte Value);
  void SPD (byte Position, byte Value);
  void Display_one(int dt = dtimedelay);
  void Display_Probe(int dt = dtimedelay);
  void Dspy_All(int dt = dtimedelay);
  void displayProbeAll(int dt = dtimedelay);
  void ShowOn();
  void ShowOff();
  void ShowLED();
  void ShowSetting();
  void ShowStby();
  void ShowSpeaker();
  void ShowHora();
  void ShowHello();
  void ShowAlert();
  void ShowAlt();
  void Showtop();
  void writext(const byte vec[], byte pos, byte size);
  void NumSegm (int dt);
  void SegAll();
  void SegmAll(byte addr, byte num);
  void SegmOnly(int dt = dtimedelay);
  void SegtoSegDigit(int dt = dtimedelay);
  void SecondsSeg(int countime = dtimedelay);
  void SetSeg(byte addr, byte num);
  void SetTime1(byte hour, byte min);
  void Changtime(byte hh = 0, byte mm = 0, byte ss = 0);
  void Clock();
  void Uphour();
  void Downhour();
  void Upmin();
  void Downmin();
  void Upsec();
  void Downsec();
  void ShowCLK1();
  void SecondsSeg1(int countime = dtimedelay);
  void ShowCLK();
  void Porcent_disk(byte addr, byte num);
  //void circleProbe (int laps, int inc);
  void circle (int laps, byte inc);
  void PorcentDown(byte addr, byte num); 
  void Upcircle(int dt = dtimedelay, byte fall = 0);
  void Downcircle(byte decm);
  void Forporcent(byte number);
  void writeNumber(byte num);
  void dtime(int dt = dtimedelay);
  void animationdt(int dt = dtimedelay);
  void beepbuzzer(byte buzz, int frec, int dt = dtimedelay);
  void multiBeep(byte buzz,int frec, byte numrepeat, int dt = dtimedelay);
  void iteration(byte numiter, unsigned long retard = dtimedelay);
  byte getNumDigits();
  byte GetButtons();
  //news
  virtual size_t Writex(int8_t chr, byte pos);//100% funcional
  void SetChar1(byte addr, byte chr);
  void setToString(const char* string, const byte pos = 0);

  void setCursor(int8_t nPos); 	// permite establecer negativo para admitir la impresión desplazada
  using Print::write; // pull in write(str) and write(buf, size) from Print
  virtual size_t write(uint8_t character);
  void clearDisplayDigit(byte pos, boolean dot);



  
  protected:
    byte Receive();
    void SendData(byte address, byte data);
    void SendCmd(byte cmd);
    void Send(byte data);
    void RTime ();
    void start();
    void stop ();
    void bitDelay();
    int8_t nPrintPos=3;
    byte dataPin;
    byte clockPin;
    byte strobePin;
    int Val = 0;
    byte PWM = 3;
    byte countiteration = 0;
    byte nNumDigits = 10;
    
  private:
  void ShowPelusa();
  void writeString(byte Position, byte Value);
  
};
#endif