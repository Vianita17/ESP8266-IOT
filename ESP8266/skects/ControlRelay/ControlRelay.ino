#include <configWiFi.h>
#include <esp8266OTA.h>


#define RELAY_PIN D5            // Control del relay
#define BUTTON_PIN D6           // Botón pulsador
#define TOUCH_PIN D7            // Sensor touch capacitivo
#define PHOTORESISTOR_PIN A0    // Fotoresistor

String HOST_NAME = "Viana-";
String passwordOTA = "123456";

bool focoEncendido = false;
bool estadoAnteriorBoton = false;
bool estadoAnteriorTouch = false;
int umbralLuz = 700;


void setup() {
  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);

  pinMode(RELAY_PIN, OUTPUT);     
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Botón con resistencia pull-up
  pinMode(TOUCH_PIN, INPUT); 
  digitalWrite(RELAY_PIN, LOW);
}


void loop() {
  
  loopOTA();
  int ldrValue = analogRead(PHOTORESISTOR_PIN);

  // Leer botón
  bool estadoBoton = !digitalRead(BUTTON_PIN); // Invertido por pull-up
  if (estadoBoton && !estadoAnteriorBoton) {
    cambiarEstadoFoco();
  }
  estadoAnteriorBoton = estadoBoton;

  // Leer touch
  bool estadoTouch = digitalRead(TOUCH_PIN);
  if (estadoTouch && !estadoAnteriorTouch) {
    cambiarEstadoFoco();
  }
  estadoAnteriorTouch = estadoTouch;

  // Leer fotoresistor
  int nivelLuz = analogRead(PHOTORESISTOR_PIN);

  if (nivelLuz >= umbralLuz ) {
    digitalWrite(RELAY_PIN, LOW);
    focoEncendido=true;
  } else if (nivelLuz <= umbralLuz) {
    digitalWrite(RELAY_PIN, focoEncendido ? HIGH : LOW);
  }

  delay(50); // Evitar rebotes
}


void cambiarEstadoFoco() {
  focoEncendido = !focoEncendido;
}
