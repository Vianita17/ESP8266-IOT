#include <configWiFi.h>
#include <esp8266OTA.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

String HOST_NAME = "Viana-";
String passwordOTA = "123456";
//String url = "http://www.google.com";
String url = "https://vermiip.es";
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println(" ");
  delay(10);

  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);
  Serial.begin(115200);
  
  Serial.println();
  Serial.println();
  Serial.println();
  delay(10);
}

void loop() {
  // put your main code here, to run repeatedly:
  loopOTA();

  if ((WiFi.status() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    // Ignore SSL certificate validation
    client->setInsecure();
    
    //create an HTTPClient instance
    HTTPClient https;
    
    //Initializing an HTTPS communication using the secure client
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, url)) {  // HTTPS
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
  Serial.println();
  Serial.println("Waiting 2min before the next round...");
  delay(120000);
  
}
