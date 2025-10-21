#include <configWiFi.h>
#include <esp8266OTA.h>

int dataPin = D6;
int latchPin = D7;
int clockPin= D8;
byte leds = 0;


String HOST_NAME = "viana-";
String passwordOTA = "123456";

long lastDisplay =0;
int contador = 0;

/*ANODO COMUN*/
const int DIGITOS[25] = {
    /*0*/ 0b11111100,   // Invertido
    /*1*/ 0b01100000,   // Invertido
    /*2*/ 0b11011010,   // Invertido
    /*3*/ 0b11110010,   // Invertido
    /*4*/ 0b01100110,   // Invertido
    /*5*/ 0b10110110,   // Invertido
    /*6*/ 0b10111110,   // Invertido
    /*7*/ 0b11100000,   // Invertido
    /*8*/ 0b11111110,   // Invertido
    /*9*/ 0b11110110,   // Invertido
    /*P*/ 0b00000001,   // Invertido
    /*A*/ 0b11101110,   // Invertido
    /*C*/ 0b10011100,   // Invertido
    /*E*/ 0b10011110,   // Invertido
    /*F*/ 0b10001110,   // Invertido
    /*G*/ 0b10111110,   // Invertido
    /*H*/ 0b01101110,   // Invertido
    /*I*/ 0b01100000,   // Invertido
    /*J*/ 0b01110000,   // Invertido
    /*L*/ 0b00011100,   // Invertido
    /*O*/ 0b11111100,   // Invertido
    /*P*/ 0b11001110,   // Invertido
    /*R*/ 0b11101110,   // Invertido
    /*S*/ 0b10110110,   // Invertido
    /*U*/ 0b01111100    // Invertido
};

void setup() {
  // put your setup code here, to run once:
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);
  
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

void updateShiftRegister(){
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, leds);//MSBFIRST
  digitalWrite(latchPin, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  loopOTA();

  long now = millis();
  if (now -lastDisplay > 1000){
    leds = DIGITOS[contador];
    updateShiftRegister();
    lastDisplay = now;
    contador++;
  }
  if (contador > 9) contador = 0;
  
}
