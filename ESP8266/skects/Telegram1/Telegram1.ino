#include <configWiFi.h>
#include <esp8266OTA.h>

String HOST_NAME = "viana-";
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
/*Here is the token for bot CursoloT @IotcursoT_bot:

7253839365:AAH8GW-FzO5OYZDC5r0ZbY54uGoWzhgJDXo*/
