#include <configWiFi.h>
#include <esp8266OTA.h>
#include <ESP8266mDNS.h>
#include "html.h"

String HOST_NAME = "viana";
String passwordOTA = "123456";


ESP8266WebServer webServer(80);

const int led = 13;

String firstname = "John";
String lastname = "Doe";

void setup(void) {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
 
  Serial.println();

  conectarWiFi();

  if(!MDNS.begin(HOST_NAME)){
      Serial.println("Error de MDS");
  }else{
      Serial.println("NMDS iniciado");
  }
  
  //configOTA(HOST_NAME, passwordOTA);
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  webServer.begin();
  Serial.println("HTTP server started");

   initialize_wwwServer();
}

void loop(void) {
  loopOTA();
  webServer.handleClient();
}

void initialize_wwwServer() {
  webServer.on("/", handle_root);
  webServer.on("/save", HTTP_POST, handle_form);
  webServer.onNotFound(handle_not_found);
  webServer.begin(); 
}

void handle_root() {
  String head = HTML_header;
  String foot = HTML_footer;
  String formhead = HTML_form_header;
  String formfoot = HTML_form_footer;

  String content = "<p>My name is: " + firstname + " " + lastname + "</p>";
  String formcontent = "Enter new Firstname: <input type=\"text\" name=\"firstname\" value=\"" + firstname + "\"><br />Enter new Lastname: <input type=\"text\" name=\"lastname\" value=\"" + lastname + "\"><br />";
  
  webServer.send(200, "text/html", head + content + formhead + formcontent + formfoot + foot);
}

void handle_form() {
  String head = HTML_header;
  String foot = HTML_footer;
  String link = HTML_saved_link;
  
  if ( webServer.hasArg( "firstname" ) && webServer.arg( "firstname" ) != NULL ) {
    firstname = webServer.arg( "firstname" );
  }
  if ( webServer.hasArg( "lastname" ) && webServer.arg( "lastname" ) != NULL ) {
    lastname = webServer.arg( "lastname" );
  }
  
  webServer.send(200, "text/html", head + link + foot);
}

void handle_not_found() {
  webServer.send(404, "text/plain", "Page not found");
}
