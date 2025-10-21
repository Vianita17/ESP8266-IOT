#include <configWiFi.h>
#include <esp8266OTA.h>
#include <PubSubClient.h>


const char* MQTT_SERVER = "esp8266murrieta.ddns.net";
const char* TOPIC_PUB = "esp8266/viana/dc05/lab";
const char* TOPIC_SUB = "esp8266/viana/control";

String HOST_NAME = "viana-";
String passwordOTA = "123456";

const int CANTIDAD_DE_DIGITOS = 2;
const int digitoPin[CANTIDAD_DE_DIGITOS] = {D3,D4};
int digito[CANTIDAD_DE_DIGITOS];

int dataPin = D6;
int latchPin = D7;
int clockPin= D8;
byte leds = 0;



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

WiFiClient espClient; // Esta línea debe ir antes de la función setup
PubSubClient client(espClient); // Declaración de client antes de setup()

void setup() {
  // put your setup code here, to run once:
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);
  
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  for(int i = 0; i < CANTIDAD_DE_DIGITOS; i++) pinMode(digitoPin[i], OUTPUT);

  client.setServer(MQTT_SERVER, 1883);  // Ahora client está declarado correctamente
  client.setCallback(callback);
}


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

bool pausado = false;  // contador estado
bool detenido = true;  //

void callback(char* topic, byte* payload, unsigned int length){
   String mensaje;
    for (int i = 0; i < length; i++) {
      mensaje += (char)payload[i];
    }
  
    if (String(topic) == TOPIC_SUB) {
      if (mensaje == "iniciar" && "continuar") {
        detenido = false;
        pausado = false;
      } else if (mensaje == "pausar") {
        pausado = true;
      } else if (mensaje == "continuar") {
        pausado = false;
      } else if (mensaje == "detener") {
        detenido = true;
        delay(6000);
        numero(0);  // Mostrar 00 en el display
        display();  // Actualizar el display de inmediato
        contador = 0;  // Resetear el contador cuando se detiene
      } else if (mensaje == "reiniciar") {
        contador = 0;
        pausado = false;
        detenido = false;
      }
    }
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
  loopOTA();  // Permitir actualizaciones OTA

  if (!client.connected()) {
    reconnect();  // Asegurar conexión MQTT
  }

  client.loop();  // Procesar mensajes MQTT

  long now = millis();
  
  // Solo actualizar el display si no está detenido o pausado
  if (!detenido && !pausado) {
    if (now - lastDisplay > 1000) {  // Actualizar cada segundo
      numero(contador);  // Convertir el número en dígitos
      lastDisplay = now;
      contador++;  // Incrementar el contador
    }
  }

  display();  // Mostrar el número actual en el display

  if (contador > 99) contador = 0;  // Reiniciar el contador si es mayor que 99
}
