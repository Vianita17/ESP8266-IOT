#include <configWiFi.h>
#include <esp8266OTA.h>
#include <PubSubClient.h>

const char* MQTT_SERVER = "esp8266murrieta.ddns.net";
const char* MOTT_SERVER = "esp8266murrieta.ddns.net";
const char* TOPIC_PUB = "esp8266/viana/dc05/lab";
const char* TOPIC_SUB = "esp8266/viana/digito";

int LEDs[] = {D0,D1,D2,D3,D4,D6,D7,D8}; // for ESP8266 microcontroller
int digito[11][8] = {
//configuracion con anodo
{0, 0, 0, 0, 0, 0, 1, 1}, // estados de LEDs para mostrar el 0
{1, 0, 0, 1, 1, 1, 1, 1}, // estados de LEDs para mostrar el 1
{0, 0, 1, 0, 0, 1, 0, 1}, // estados de LEDs para mostrar el 2
{0, 0, 0, 0, 1, 1, 0, 1}, // estados de LEDs para mostrar el 3
{1, 0, 0, 1, 1, 0, 0, 1}, // estados de LEDs para mostrar el 4
{0, 1, 0, 0, 1, 0, 0, 1}, // estados de LEDs para mostrar el 5
{0, 1, 0, 0, 0, 0, 0, 1}, // estados de LEDs para mostrar el 6
{0, 0, 0, 1, 1, 1, 1, 1}, // estados de LEDs para mostrar el 7
{0, 0, 0, 0, 0, 0, 0, 1}, // estados de LEDs para mostrar el 8
{0, 0, 0, 0, 1, 0, 0, 1}, // estados de LEDs para mostrar el 9
{1, 1, 1, 1, 1, 1, 1, 0} // estados de LEDs para mostrar el pd
};
long lastDisplay = 0;

String HOST_NAME = "viana-";
String passwordOTA = "123456";

void display(int N){
  for (int i = 0; i<8; i++) digitalWrite(LEDs[i], digito[N][i]);
}

void callback(char* topic, byte* payload, unsigned int length){
  char msj = (char)payload[0];
  display((int)String(msj).toInt());
}

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect(){
  while(!client.connected()){
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if(client.connect(clientId.c_str())){
      client.subscribe(TOPIC_SUB);
      client.publish(TOPIC_PUB, "Viana conectado :/!");
    }else{
      delay(5000);
    }
  }
}

void setup(){
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);
  
  for (int i = 0; i<8; i++) pinMode(LEDs[i], OUTPUT);

  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

void loop(){
  loopOTA();

  if(!client.connected()){
    reconnect();
  }

  client.loop();
}
