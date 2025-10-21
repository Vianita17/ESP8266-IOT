#include <configWiFi.h>
#include <esp8266OTA.h>
#include <PubSubClient.h>

const char* MOTT_SERVER = "esp8266murrieta.ddns.net";
const char* TOPIC_PUB = "esp8266/viana/dc05/lab";
const char* TOPIC_SUB = "esp8266/viana/control";

const int CANTIDAD_DE_DIGITOS = 2;
const int digitoPin[CANTIDAD_DE_DIGITOS] = {D3,D4};
int digito[CANTIDAD_DE_DIGITOS];

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

void setup() {
  // put your setup code here, to run once:
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);
  
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  for(int i = 0; i < CANTIDAD_DE_DIGITOS; i++) pinMode(digitoPin[i], OUTPUT);

  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
}

void updateShiftRegister(){
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, leds);//MSBFIRST
  digitalWrite(latchPin, HIGH);
}
void numero(int num){
  int decenas = num/10;
  digito[0] = decenas;
  int unidades = num - decenas * 10;
  digito[1] = unidades;
}
void display(){
  for(int i=0; i < CANTIDAD_DE_DIGITOS; i++){
    //apagar todos los digitos
    for(int j=0; j < CANTIDAD_DE_DIGITOS; j++) digitalWrite(digitoPin[j], HIGH);
    leds = DIGITOS[digito[i]];
    updateShiftRegister();
    digitalWrite(digitoPin[i], LOW);
    delay(1);
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  loopOTA();

  long now = millis();
  if (now -lastDisplay > 1000){
    numero(contador);
    lastDisplay = now;
    contador++;
  }
  
  display();
  
  if (contador > 99) contador = 0;
  
  if(!client.connected()){
    reconnect();
  }

  client.loop();
}
