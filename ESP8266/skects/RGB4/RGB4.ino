#include <configWiFi.h>
#include <esp8266OTA.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <UniversalTelegramBot.h>

// Configuración de los pines con el LED
#define pinRed  D1 // LED rojo
#define pinGreen D2 // LED verde
#define pinBlue D3 // LED azul

// Configuración del nombre de la placa
String HOST_NAME = "viana-";
String passwordOTA = "123456";

// Token del Bot de Telegram
#define BOT_TOKEN "7253839365:AAH8GW-FzO5OYZDC5r0ZbY54uGoWzhgJDXo"
#define CHAT_ID  "6640684196"
const unsigned long BOT_MTBS = 500; // Tiempo entre mensajes

bool ledStatus = false; // Estado del LED
bool blink = false;

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // Última vez de actualización del bot

// Variables globales para el parpadeo del LED
int currentR, currentG, currentB; // Colores actuales

// Función para manejar los colores del LED RGB
void color(int R, int G, int B) {
  // LED Cátodo Común
  analogWrite(pinRed,   R);
  analogWrite(pinGreen, G);
  analogWrite(pinBlue,  B);

  // Guardar los colores actuales
  currentR = R;
  currentG = G;
  currentB = B;
}

// Función para el parpadeo del LED RGB
// Función para el parpadeo del LED RGB
Ticker tickerRGB;
void parpadeoLedWiFiRGB() {
  static bool estado = false; // Variable para alternar el estado
  if (estado) {
    color(currentR, currentG, currentB); // Restaurar el color actual
  } else {
    color(0, 0, 0); // Apagar el LED
  }
  estado = !estado; // Alternar el estado
}

// Información de red al iniciar
String getNetworkInfo() {
  String ssid = WiFi.SSID();
  String ip = WiFi.localIP().toString();
  return "SSID: " + ssid + "\nIP: " + ip + "\nChatID: " + CHAT_ID;
}

// Configuración del Bot y manejo de mensajes
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id; // Obtener el ID
    String text = bot.messages[i].text;       // Obtener el texto
    String from_name = bot.messages[i].from_name; // Obtener el nombre

    if (text == "Start") {
      String welcome = "Bienvenido al Bot Telegram, " + from_name + ".\n";
      welcome += "Puede usar los siguientes comandos:\n\n";
      welcome += "Rojo : Para encender el LED en rojo\n";
      welcome += "Verde : Para encender el LED en verde\n";
      welcome += "Azul: Para encender el LED en azul\n";
      welcome += "LedOff : Para apagar el LED\n";
      welcome += "BlinkON : Parpadeo del LED\n";
      welcome += "BlinkOff : Apagar parpadeo del LED\n";
      welcome += "Estado : Estado del LED\n";
      welcome += "Opciones : Opciones del LED\n";
      bot.sendMessage(chat_id, welcome, "");
    }
    else if (text == "Rojo") {
      color(255, 0, 0); // Encender LED rojo
      ledStatus = true;
      blink = false; // Detener el parpadeo si estaba activado
      tickerRGB.detach(); // Detener el ticker
    }
    else if (text == "Verde") {
      color(0, 255, 0); // Encender LED verde
      ledStatus = true;
      blink = false;
      tickerRGB.detach();
    }
    else if (text == "Azul") {
      color(0, 0, 255); // Encender LED azul
      ledStatus = true;
      blink = false;
      tickerRGB.detach();
    }
    else if (text == "LedOff") {
      color(0, 0, 0); // Apagar el LED
      ledStatus = false;
      blink = false;
      tickerRGB.detach();
    }
    else if (text == "BlinkON") {
      if (ledStatus) { // Si el LED está encendido, parpadea en el color actual
        tickerRGB.attach(0.5, parpadeoLedWiFiRGB); // Parpadeo cada 0.5 segundos
        blink = true;
      } else {
        bot.sendMessage(chat_id, "El LED está apagado. Enciende un color primero.", "");
      }
    }
    else if (text == "BlinkOff") {
      tickerRGB.detach(); // Detener el parpadeo
      blink = false;
    }
    else if (text == "Estado") {
      if (blink) {
        bot.sendMessage(chat_id, "El LED está parpadeando", "");
      } else if (ledStatus) {
        bot.sendMessage(chat_id, "El LED está encendido", "");
      } else {
        bot.sendMessage(chat_id, "El LED está apagado", "");
      }
    }
    else if (text == "Opciones") {
      String keyboardJson = "[[\"Rojo\", \"Verde\", \"Azul\"],[\"LedOff\", \"BlinkON\", \"BlinkOff\"],[\"Estado\", \"Opciones\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Elija una opción:", "", keyboardJson, true);
    }
  }
}

void setup() {
  pinMode(pinRed, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinBlue, OUTPUT);
  digitalWrite(pinRed, 0);   // Apagar los LEDs al inicio
  digitalWrite(pinGreen, 0);
  digitalWrite(pinBlue, 0);

  conectarWiFi(); // Conexión a WiFi
  secured_client.setTrustAnchors(&cert); // Añadir certificado para Telegram

  configTime(0, 0, "pool.ntp.org"); // Obtener hora UTC mediante NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    delay(100);
    now = time(nullptr);
  }

  Serial.println(now);
  configOTA(HOST_NAME, passwordOTA); // Configurar OTA
  String datos = getNetworkInfo();
  bot.sendMessage(CHAT_ID, "Bot iniciado\n" + datos, "");
}

void loop() {
  loopOTA(); // Manejar actualizaciones OTA

  if (millis() - bot_lasttime > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }
}
