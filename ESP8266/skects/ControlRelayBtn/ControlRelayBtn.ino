#include <configWiFi.h>
#include <esp8266OTA.h>

// Pines
#define RELAY_PIN D5            // Control del relay
#define BUTTON_PIN D7           // Botón
#define TOUCH_PIN D6            // Sensor touch
#define PHOTORESISTOR_PIN A0    // Fotoresistor

// ESTADOS DE CAMBIOS
#define OFF 0
#define ON 1
#define INV 2

String HOST_NAME = "Viana-";
String passwordOTA = "123456";

bool focoEncendido = false;     // Estado del foco
bool estadoAnteriorBoton = false; // Estado anterior del botón
bool estadoAnteriorTouch = false; // Estado anterior del touch
int estadoAnteriorLuz = 0;
int nivelLuz = 0;
int umbralLuz = 500;            // Umbral para la luz ambiental

void setup() {
  // put your setup code here, to run once:

  conectarWiFi();
  configOTA(HOST_NAME, passwordOTA);

  pinMode(RELAY_PIN, OUTPUT);     // Pin para controlar el relay
  pinMode(BUTTON_PIN, INPUT); // Botón con resistencia pull-down
  pinMode(TOUCH_PIN, INPUT);      // Entrada del sensor touch
 
  digitalWrite(RELAY_PIN, HIGH);   // Asegurar que el relay esté apagado al inicio
}

void loop() {
  loopOTA();
  // leemos las entradas
  estadoAnteriorLuz = nivelLuz;
  delay(20);
  nivelLuz = analogRead(PHOTORESISTOR_PIN);
  bool estadoBoton = digitalRead(BUTTON_PIN); // Invertido por pull-up
  bool estadoTouch = digitalRead(TOUCH_PIN);

  // logica foco
  if (nivelLuz < umbralLuz) {
    cambiarEstadoFoco(OFF);
  } else {
    if (nivelLuz >= umbralLuz && estadoAnteriorLuz < umbralLuz) {
      cambiarEstadoFoco(ON);
    }
    if (estadoBoton || estadoTouch) {
      cambiarEstadoFoco(INV);
    }
  }
}

void cambiarEstadoFoco(int nuevoEstado) {
  if (nuevoEstado == INV) {
    focoEncendido = !focoEncendido;
  } else if (nuevoEstado == ON) {
    focoEncendido = true;
  } else if (nuevoEstado == OFF) {
    focoEncendido = false;
  } else { // en cualquier otro caso apagamos el foco
    focoEncendido = false;
  }
  digitalWrite(RELAY_PIN, focoEncendido ? HIGH : LOW);
}
