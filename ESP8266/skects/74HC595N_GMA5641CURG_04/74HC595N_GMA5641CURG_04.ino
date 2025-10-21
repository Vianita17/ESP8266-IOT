#include <configWiFi.h>
#include <esp8266OTA.h>

const int CANTIDAD_DE_DIGITOS = 4;
const int digitoPin[CANTIDAD_DE_DIGITOS] = {D1, D2, D3, D4};
int digito[CANTIDAD_DE_DIGITOS]={0,0,0,0};

int dataPin = D6; // GPIO12
int latchPin = D7; // GPIO13
int clockPin = D8; // GPIO15

byte leds = 0;
/*CÁTODO COMÚN*/
const int DIGITOS[25] = {
/*0*/ 0b11111100,
/*1*/ 0b01100000,
/*2*/ 0b11011010,
/*3*/ 0b11110010,
/*4*/ 0b01100110,
/*5*/ 0b10110110,
/*6*/ 0b10111110,
/*7*/ 0b11100000,
/*8*/ 0b11111110,
/*9*/ 0b11110110,
/*P*/ 0b00000001,
/*A*/ 0b11101110,
/*C*/ 0b10011100,
/*E*/ 0b10011110,
/*F*/ 0b10001110,
/*G*/ 0b10111110,
/*H*/ 0b01101110,
/*I*/ 0b01100000,
/*J*/ 0b01110000,
/*L*/ 0b00011100,
/*O*/ 0b11111100,
/*P*/ 0b11001110,
/*R*/ 0b11101110,
/*S*/ 0b10110110,
/*U*/ 0b01111100
};

byte PUNTO[4] = {
 0b00000000,
 0b00000000,
 0b00000000,
 0b00000000  
};

String HOST_NAME = "viana-";
String passwordOTA = "123456";
long lastDisplay = 0;
int contador = 0;
 
void setup() {
  // put your setup code here, to run once:
  configOTA(HOST_NAME, passwordOTA);

  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  for(int i=0; i<CANTIDAD_DE_DIGITOS; i++) pinMode(digitoPin[i], OUTPUT);  
}

void updateShiftRegister(){
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST,  leds); //MSBFIRST
  digitalWrite(latchPin, HIGH);
}

void numero(float num) {
  int d0,d1,d2,d3;
  for (int i; i<CANTIDAD_DE_DIGITOS; i++) PUNTO[i] = 0b00000000;
  if (num >= 1000.) { 
    d0 = int(num/1000.);
    d1 = int((num - d0*1000.)/100.);
    d2 = int((num - d0*1000. - d1*100.)/10.);
    d3 = int((num - d0*1000. - d1*100. - d2*10.));   
  } else if (num >= 100) {
    PUNTO[2] = 0b00000001;
    d0 = int(num/100.);
    d1 = int((num - d0*100.)/10.);
    d2 = int(num - d0*100. - d1*10.);
    d3 = int((num - d0*100. - d1*10. - d2)*10.); 
  } else if (num >=10) {
    PUNTO[1] = 0b00000001;
    d0 = int(num/10.); 
    d1 = int(num - d0*10.);
    d2 = int((num - d0*10. - d1)*10.); 
    d3 = int((num - d0*10. - d1 - d2/10.)*100.);  
  } else if (num >=0.001) {
    PUNTO[0] = 0b00000001;
    d0 = int(num);
    d1 = int((num - d0)*10.); 
    d2 = int((num - d0 - d1/10.)*100.); 
    d3 = int((num - d0 - d1/10. - d2/100.)*1000.);  
  }
  digito[0] = d0;
  digito[1] = d1;
  digito[2] = d2;
  digito[3] = d3;
}

void display(){
  for (int i=0; i<CANTIDAD_DE_DIGITOS; i++){
    //Apagar todos los digitos
    for (int j=0; j<CANTIDAD_DE_DIGITOS; j++) digitalWrite(digitoPin[j], HIGH);
    leds = DIGITOS[digito[i]] | PUNTO[i];
    updateShiftRegister();
    digitalWrite(digitoPin[i], LOW);
    delay(3);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  loopOTA();

  long now = millis();
  int mul = 10;
  if(now-lastDisplay > 200){
    if (contador > 100000) {
      contador = contador + 1000;
    } else if (contador > 10000) {
      contador = contador + 100;
    } else if (contador > 1000) {
      contador = contador + 10;
    } else {
      contador ++;
    }
    
    lastDisplay = now;
    numero(contador/1000.);
  }

  display();
}
