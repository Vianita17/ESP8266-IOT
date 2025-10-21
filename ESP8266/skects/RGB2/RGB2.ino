#include <configWiFi.h>
#include <esp8266OTA.h>

#define range_pwm 255
#define pinRed  D1 //led rojo
#define pinGreen D2 //led verde
#define pinBlue D3 //led blue

String HOST_NAME = "viana-";
String passwordOTA = "123456";

void color(int R, int G, int B){
  //LED CATODO COMUN
  analogWrite(pinRed,   R);
  analogWrite(pinGreen, G);
  analogWrite(pinBlue,  B);
  //LED ANODO COMUN
  /*
  analogWrite(pinRed,   255 - R);
  analogWrite(pinGreen, 255 - G);
  analogWrite(pinBlue,  255 - B);
  */
}
void setup() {
  // put your setup code here, to run once:
  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinBlue, OUTPUT);
  digitalWrite(pinRed, 0);//1 para anodo
  digitalWrite(pinGreen, 0);
  digitalWrite(pinBlue, 0);
  analogWriteRange(range_pwm);
  
  //OTA
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);

  //ANIMACION 
  int r,g,b;
  int delta = 10;
  for(r = 5; r <= 255; r = r + delta){
    for(g = 5; g <= 255; g = g + delta){
      for(b = 5; b <= 255; b = b + delta){
          color(r, g, b);
          delay(10);
        }
      }
    }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  loopOTA();
}
