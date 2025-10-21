#include <configWiFi.h>
#include <esp8266OTA.h>
#include <PubSubClient.h>

#define MSG_BUFFER_SIZE (50)

int pinLed = D2;
int analogInPin = A0;

const char* MOTT_SERVER = "esp8266murrieta.ddns.net";
const char* TOPIC_PUB = "esp8266/viana/a0/lab";
const char* TOPIC_SUB = "esp8266/viana/led";

String HOST_NAME = "viana-";
String passwordOTA = "123456";

//float factor = 3.298/1024;
unsigned long int lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
int value = 0;

WiFiClient espClient;
PubSubClient client(espClient);//cliente


void setup() {
  // put your setup code here, to run once:
  //inicializar el pin led
  pinMode(pinLed, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);

  client.setServer(MOTT_SERVER, 1883);
  client.setCallback(callback);

  digitalWrite(pinLed, LOW);
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

void enviarMensaje(){
  digitalWrite(LED_BUILTIN, LOW);
  value =  analogRead(analogInPin);
  float voltaje = 0.003222656 *value;
  
  snprintf(msg, MSG_BUFFER_SIZE, "Lectura A0:  %ld; V = %6.4fV", value, voltaje);
  client.publish(TOPIC_PUB, msg);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  loopOTA();

  if(!client.connected()){
    reconnect();
  }

  unsigned long now = millis();
  if(now - lastMsg > 5000){
    enviarMensaje();
    lastMsg = now;
  }
  
  client.loop();
}
