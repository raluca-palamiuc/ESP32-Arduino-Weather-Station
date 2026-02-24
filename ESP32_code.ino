/*
  Environmental Monitoring Station - ESP32

  This project monitors environmental conditions using sensors.
  The ESP32 receives sensor data through Serial communication.

  Author: Raluca Palamiuc
  Year: 2026
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <WebServer.h>

//configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
#define BOTtoken "YOUR_BOT_TOKEN" //Telegram bot token
#define CHAT_ID "YOUR_CHAT_ID" //Telegram chat ID

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
WebServer server(80);

unsigned long lastTimeBotRan;
int botRequestDelay = 2000; 
String ultimaCitire = "Waiting for sensor data...";

bool alertaPloaieActiva = false; 
bool alertaAerActiva = false;


//Web dashboard page
void handleRoot() {
  String html = "<html><head><meta charset='UTF-8'><meta http-equiv='refresh' content='5'>";
  html += "<style>body{font-family:Arial; text-align:center; background:#7f1ba63f;} ";
  html += ".card{background:white; padding:20px; border-radius:10px; display:inline-block; margin-top:50px; box-shadow: 0 4px 8px rgba(0,0,0,0.1);}</style>";
  html += "<title>Environmental Monitoring Station </title></head><body>";
  html += "<div class='card'><h1> Real-time Weather Dashboard</h1>";
  html += "<p style='font-size:1.2em; color:#333;'>" + ultimaCitire + "</p>";
  html += "<hr><p>Data refreshes automatically every 5 seconds</p></div>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {

  Serial.begin(115200);

  Serial.setTimeout(50);
  client.setTimeout(1000);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500); 
    Serial.print(".");
  }
  
  client.setInsecure(); // Required for Telegram SSL without certificate management
  Serial.println("\nWiFi connected");
  
  server.on("/", handleRoot);
  server.begin();
  
  delay(2000); //allow system to stabilize before first network request
  bot.sendMessage(CHAT_ID, "Weather Station is online", "");

  Serial.print("Web Dashboard: http://");
  Serial.println(WiFi.localIP());

}

//Handles incoming Telegram messages
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String text = bot.messages[i].text;
    if (text == "/status") {
      bot.sendMessage(CHAT_ID, "Current Status:\n" + ultimaCitire, "");
    }
    if (text == "/site") {
      bot.sendMessage(CHAT_ID, "Web Dashboard Link: http://" + WiFi.localIP().toString(), "");
    }
  }
}

void loop() {
  
  server.handleClient();

  if (!client.connected()) {
   client.stop();
  }

//check WiFi connection and reconnect if needed
  if (WiFi.status() != WL_CONNECTED) {
     static unsigned long lastRetry = 0;
     if (millis() - lastRetry > 10000) {
      //connect to WiFi network
       WiFi.begin(ssid, password);
       lastRetry = millis();
     }
  }

  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (numNewMessages) {
      handleNewMessages(numNewMessages);
    }
    lastTimeBotRan = millis();
  }

  //Process data from Arduino
  if (Serial.available()) {
    String date = Serial.readStringUntil('\n');
    date.trim();
    
    if (date.length() > 0) {
      ultimaCitire = date;

      //check alert when rain is detected
      if (date.indexOf("R:YES") >= 0 && !alertaPloaieActiva) {
        bot.sendMessage(CHAT_ID, "ALERT: Rain detected!", "");
        alertaPloaieActiva = true; 
      } 
      else if (date.indexOf("R:NO") >= 0 && alertaPloaieActiva) {
        bot.sendMessage(CHAT_ID, "Rain has stopped.", "");
        alertaPloaieActiva = false; 
      }

      //check air quality value and sent alert is if to high 
      int indexA = date.indexOf("A:");
      if (indexA >= 0) {
        int valAer = date.substring(indexA + 2).toInt();
        if (valAer > 800 && !alertaAerActiva) {
          bot.sendMessage(CHAT_ID, "Warning! High pollution level detected: " + String(valAer), "");
          alertaAerActiva = true;
        } else if (valAer < 600 && alertaAerActiva) {
          bot.sendMessage(CHAT_ID, "Air quality is back to normal.", "");
          alertaAerActiva = false;
        }
      }
    }
  }
}