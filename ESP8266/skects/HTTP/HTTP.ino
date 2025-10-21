#include <configWiFi.h>
#include <esp8266OTA.h>
#include <ESP8266HTTPClient.h>

String HOST_NAME = "Viana-";
String passwordOTA = "123456";
//String url = "http://www.google.com";
String url = "http://esp8266murrieta.ddns.net/web/device/register?chip_id=0123456789";
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println(" ");
  delay(10);

  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);
  Serial.begin(115200);
  Serial.println(" ");
  delay(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  loopOTA();

  HTTPClient http;
  WiFiClient client;

  if (http.begin(client, url)) //Iniciar conexión
  {
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();  // Realizar petición

    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();  // Obtener respuesta
        Serial.println(payload);  // Mostrar respuesta por serial
      }
    }
    else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
  else {
    Serial.printf("[HTTP} Unable to connect\n");
  }

  delay(30000);
  
}
