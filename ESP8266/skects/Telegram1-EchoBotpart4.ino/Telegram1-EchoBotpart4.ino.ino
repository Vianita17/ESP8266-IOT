
#include <configWiFi.h>
#include <esp8266OTA.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <UniversalTelegramBot.h>

String HOST_NAME = "viana-";
String passwordOTA = "123456";

// Telegram Bot token
#define BOT_TOKEN "7253839365:AAH8GW-FzO5OYZDC5r0ZbY54uGoWzhgJDXo"
#define CHAT_ID  "6640684196"
#define ledPin D1
const unsigned long BOT_MTBS = 500; // tiempo entre mensajes


X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages

bool ledStatus = false; // estatus del led
bool blink = false; // blink estatus

String getNetworkInfo() {
  String ssid = WiFi.SSID();
  String ip = WiFi.localIP().toString();
  return "SSID: " + ssid + "\nIP: " + ip + "\nChatID: " + CHAT_ID;
}

void alternar() {
  ledStatus = !ledStatus; // Alterna el estado del LED
  digitalWrite(ledPin, ledStatus ? HIGH : LOW);
}

void LedBlink() {
  if (blink) {
    ticker.attach(0.1, alternar); // tiempo de llamada del ticker
  } else {
    ticker.detach(); // Detiene el parpadeo
  }
}

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;//obtener el id
    String text = bot.messages[i].text;//obtener el texto
    String from_name = bot.messages[i].from_name;//obtener el nombre

    if (text == "/start") {
      String welcome = "Bienvenido al Bot Telegram, " + from_name + ".\n";
      welcome += "Puede usar los siguientes comandos:\n\n";
      welcome += "/ledon : Para encender el LED\n";
      welcome += "/ledoff : Para apagar el LED\n";
      welcome += "/blinkon : Para parpadear el LED\n";
      welcome += "/blinkoff : Para detener el parpadeo\n";
      welcome += "/status : Estado del LED\n";
      welcome += "/options : Estado del LED\n";
      bot.sendMessage(chat_id, welcome, "");
    }

    else if (text == "/ledon") {
      ledStatus = true;
      blink = false; // asegurar el estado del blink
      digitalWrite(ledPin, HIGH); // activar led
      bot.sendMessage(chat_id, "Led en ON", "");
    }

    else if (text == "/ledoff") {
      ledStatus = false;
      ticker.detach(); 
      digitalWrite(ledPin, LOW); // Deactivates the LED (off)
      blink = false; // asegurar el estado del blink
      bot.sendMessage(chat_id, "Led en OFF", "");
    }

    else if (text == "/blinkon") {
      ledStatus = true;
      blink = true; // hacer que parpadee
      LedBlink(); // Iniciar parpadeo
      bot.sendMessage(chat_id, "Led parpadeando", "");
    }

    else if (text == "/blinkoff") {
      ledStatus = false;
      blink = false; // dejar de parpadear
      LedBlink(); // Detener parpadeo
      digitalWrite(ledPin, LOW); 
      bot.sendMessage(chat_id, "Parpadeo detenido", "");
    }

     else if (text == "/status") {
      if (blink) {
        bot.sendMessage(chat_id, "Led está parpadeando", "");
      } else if (ledStatus) {
        bot.sendMessage(chat_id, "Led en ON", "");
      } else {
        bot.sendMessage(chat_id, "Led en OFF", "");
      }
    }
    else if(text == "/options"){
      String keyboardJson = "[[\"/ledon\", \"/ledoff\"],[\"/blinkon\", \"/blinkoff\"],[\"/status\"],[\"/options\"]]";
        bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson, true);
      }
  }
}

void setup() {
  pinMode(ledPin, OUTPUT); // Initialize the LED pin
  digitalWrite(ledPin, LOW); // Ensure LED is off initially
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

  LedBlink(); // Asegura que el estado del LED se maneje correctamente
}
