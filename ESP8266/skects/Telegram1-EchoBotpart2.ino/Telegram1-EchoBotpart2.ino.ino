#include <configWiFi.h>
#include <esp8266OTA.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <UniversalTelegramBot.h> 

String HOST_NAME = "viana-";
String passwordOTA = "123456";

//Telegram Bot token
#define BOT_TOKEN "7253839365:AAH8GW-FzO5OYZDC5r0ZbY54uGoWzhgJDXo"
#define CHAT_ID  "6640684196"

const unsigned long BOT_MTBS = 500; // mean time between scan messages

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

String getNetworkInfo() {
  String ssid = WiFi.SSID();
  String ip = WiFi.localIP().toString();
  return "SSID: " + ssid + "\nIP: " + ip + "\nChatID: "+ CHAT_ID;
}

void handleNewMessages(int numNewMessages)
{
  for (int i = 0; i < numNewMessages; i++)
  {
    bot.sendMessage(bot.messages[i].chat_id, "Julian Viana", "");

  }
}


void setup()
{
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
  

void loop()
{
  loopOTA();
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
