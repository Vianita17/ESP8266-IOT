#include <configWiFi.h>
#include <esp8266OTA.h>

String HOST_NAME = "Viana-";
String passwordOTA = "123456";

void setup() {
  // put your setup code here, to run once:
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  loopOTA();
}
