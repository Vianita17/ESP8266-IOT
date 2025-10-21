#include <configWiFi.h>
#include <esp8266OTA.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>


String HOST_NAME = "Couoh Arthur-";
String passwordOTA = "123456";
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "7417353952:AAEIDZFyiM5yIMjshuIeGN9YutiW5mQK0t0"
#define CHAT_ID "912326125"

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

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
    bot.sendMessage(bot.messages[i].chat_id, "AngelCouoh", "");

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
  String skibidy = getNetworkInfo();
  bot.sendMessage(CHAT_ID, "Bot started up\n" + skibidy, "");

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
