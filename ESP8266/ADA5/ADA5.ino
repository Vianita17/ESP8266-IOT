#include <configWiFi.h>
#include <esp8266OTA.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <UniversalTelegramBot.h>
#include <DHT.h>

#define DHTTYPE DHT11
#define DHTPin 14 // Pin del DHT11 (D5)
#define MSG_BUFFER_SIZE (50)

String HOST_NAME = "viana-";
String passwordOTA = "123456";

// Telegram Bot token
#define BOT_TOKEN "7253839365:AAH8GW-FzO5OYZDC5r0ZbY54uGoWzhgJDXo"
#define CHAT_ID  "6640684196"

const unsigned long BOT_MTBS = 500; // tiempo entre mensajes


X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages
DHT dht(DHTPin, DHTTYPE);

String getNetworkInfo() {
  String ssid = WiFi.SSID();
  String ip = WiFi.localIP().toString();
  return "SSID: " + ssid + "\nIP: " + ip + "\nChatID: " + CHAT_ID;
}



void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;//obtener el id
    String text = bot.messages[i].text;//obtener el texto
    String from_name = bot.messages[i].from_name;//obtener el nombre

    if (text == "/start") {
      String welcome = "Bienvenido al Bot Telegram, " + from_name + ".\n";
      welcome += "Puede usar los siguientes comandos:\n\n";
      welcome += "/temperatura : Obtener la temperatura\n";
      welcome += "/humedad : Obtener la humedad\n";
      welcome += "/sensacion : Obtener la sensación térmica\n";
      welcome += "/options : Opciones\n";
      bot.sendMessage(chat_id, welcome, "");
    }
    // Envía la temperatura
    else if (text == "/temperatura") {
      float t = dht.readTemperature();
      String datos = "Temperatura: " + String(t) + " °C";
      bot.sendMessage(chat_id, datos, "");
    }

    // Envía la humedad
    else if (text == "/humedad") {
      float h = dht.readHumidity();
      String datos = "Humedad: " + String(h) + "%";
      bot.sendMessage(chat_id, datos, "");
    }

    // Envía la sensación térmica
    else if (text == "/sensacion") {
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      float hi = dht.computeHeatIndex(t, h, false);
      String datos = "Sensación térmica: " + String(hi) + " °C";
      bot.sendMessage(chat_id, datos, "");
    }

    // Muestra opciones
    else if (text == "/options") {
      String keyboardJson = "[[\"/temperatura\", \"/humedad\"],[\"/sensacion\", \"/options\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Elija una de las siguientes opciones:", "", keyboardJson, true);
    }

  }
}

void setup() {
  conectarWiFi();
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
  configOTA(HOST_NAME, passwordOTA);
  String datos = getNetworkInfo();
  bot.sendMessage(CHAT_ID, "Bot started up\n" + datos, "");
  dht.begin();

}

void loop() {
  loopOTA();
  if (millis() - bot_lasttime > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }

}
