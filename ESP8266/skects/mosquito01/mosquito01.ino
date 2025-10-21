#include <configWiFi.h>
#include <esp8266OTA.h>
#include <PubSubClient.h>
int pinLed = D2;

const char* MOTT_SERVER = "esp8266murrieta.ddns.net";
const char* TOPIC_PUB = "esp8266/viana";
const char* TOPIC_SUB = "esp8266/viana/led";

String HOST_NAME = "viana-";
String passwordOTA = "123456";

WiFiClient espClient;
PubSubClient client(espClient);//cliente


void setup() {
  // put your setup code here, to run once:
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);

  client.setServer(MOTT_SERVER, 1883);
  client.setCallback(callback);
  
}
//logica de las acciones
void callback(char* topic, byte* payload, unsigned int length){
  if((char)payload[0] == '1'){
    digitalWrite(pinLed, HIGH);
  }else if((char)payload[0] == '0'){
    digitalWrite(pinLed, LOW);
  }
}
//CHECAR PAR VER SI ESTA CONECTADO AL SERVIDOR
void reconnect(){
  while(!client.connected()){
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if(client.connect(clientId.c_str())){
      client.subscribe(TOPIC_SUB);
      client.publish(TOPIC_PUB, "Julian Viana conectado! ");
    }else{
      delay(5000);
    }
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  loopOTA();

  if(!client.connected()){
    reconnect();
  }
  
  client.loop();
}
