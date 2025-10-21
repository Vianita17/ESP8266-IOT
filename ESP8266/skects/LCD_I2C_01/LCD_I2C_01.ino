#include <configWiFi.h>
#include <esp8266OTA.h>
#include <PubSubClient.h>
#include <Wire.h>


#define MSG_BUFFER_SIZE (50)


String HOST_NAME = "viana-";
String passwordOTA = "123456";

const char* MQTT_SERVER = "192.168.1.211";//esp8266murrieta.ddns.net
const char* TOPIC_PUB  = "esp8266/viana/I2caddress"; // Topic para publicar mensajes de estado
const char* TOPIC_SUB = "esp8266/viana/led"; // Topic único para recibir comandos
char msg[MSG_BUFFER_SIZE];
unsigned long lastMsg;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length){
  
}

void reconnect(){
  while(!client.connected()){
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if(client.connect(clientId.c_str())){
      client.subscribe(TOPIC_SUB); // Suscribirnos a un único topic
      client.publish(TOPIC_PUB, "Julian Viana conectado! XD");
    }else{
      delay(5000);
    }
  }
}

void setup(){

  Wire.begin();//Wire.begin(pinSDA, pinSCL);-con argumentos
  
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);
  
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);

}

void scan_i2c() {
  byte error, address;
  int nDevices = 0; 
  client.publish(TOPIC_PUB, "Scanning...");   
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {           
      if (address<16) {
        snprintf (msg, MSG_BUFFER_SIZE,  "I2C device found at address 0x0%x" ,address);
        client.publish(TOPIC_PUB, msg);
      } else {             
        snprintf (msg, MSG_BUFFER_SIZE,  "I2C device found at address 0x%x" ,address);
        client.publish(TOPIC_PUB, msg);
      }
      nDevices++;
    } else if (error==4) {      
      if (address<16) {        
        snprintf (msg, MSG_BUFFER_SIZE,  "I2C device found at address 0x0%x" ,address);
        client.publish(TOPIC_PUB, msg);        
      } else {
        snprintf (msg, MSG_BUFFER_SIZE,  "I2C device found at address 0x%x" ,address);
        client.publish(TOPIC_PUB, msg);        
      }
    }    
  }
  if (nDevices == 0) {
    client.publish(TOPIC_PUB, "No I2C devices found\n");     
  } else {   
    client.publish(TOPIC_PUB, "Done\n"); 
  }
}

void loop(){
  loopOTA();
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long now = millis();
  if (now -lastMsg > 20000){
    scan_i2c();
    lastMsg = now;
  }
}
