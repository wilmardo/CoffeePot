#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <IPAddress.h>
#include <FS.h>

#define DEBUG //define when debuggin, comment out when in production

/* Setting up the wifi
   SSID is your network SSID (name)
   PASS is your network password (use for WPA, or use as key for WEP)
   IP is the ipaddress of the arduino
   DOMOTICZIP is the ipaddress of the Domoticz
*/
const char* SSID = "den Ouden";
const char* PASSWORD = "wifiw8woord";
const char* HOST = "CoffeePot";
IPAddress DOMOTICZIP(192, 168, 1, 215);
WiFiClient CLIENT;
ESP8266WebServer SERVER(80);
ESP8266HTTPUpdateServer HTTP_UPDATER;

/*
 * Pin variables
 */
const byte OUT_CUPS = D0;
const byte OUT_STRENGTH = D6;
const byte OUT_GRINDER = D7;
const byte OUT_START = D8;

const byte IN_CUPS = D1;
const byte IN_STRENGTH = D2;
const byte IN_GRINDER = D5;
const byte IN_START = D4;

/*
 * Status variables
 */
byte CUPSCOUNT = 4;           // 0=2cups, 1=4cups, 2=6cups, 3=8cups, 4=10cups
byte STRENGTHCOUNT = 1;      // 0 mild, 1 medium, 2 strong
byte GRINDERCOUNT = 0;       // 0 grinder on, 1 grinder off
byte START = 0;             // 0 not started, 1 started

/*
 * Http server files
 */
File INDEX_HTML;
File STYLE_CSS;
File MAIN_JS;

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
  WiFi.begin(SSID, PASSWORD);
  WiFi.config(IPAddress(192, 168, 1, 23), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));

  // Wait for connection
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    #ifdef DEBUG
      Serial.println("Connection Failed! Rebooting...");
    #endif
    delay(5000);
    ESP.restart();
  }

  setupArduinoOTA();
  SPIFFS.begin(); //open SPIFFS
  setupWebserver();

  // Set pins
  pinMode(OUT_CUPS, OUTPUT);
  pinMode(OUT_STRENGTH, OUTPUT);
  pinMode(OUT_GRINDER, OUTPUT);
  pinMode(OUT_START, OUTPUT);
  pinMode(IN_CUPS, INPUT);
  pinMode(IN_STRENGTH, INPUT);
  pinMode(IN_GRINDER, INPUT);
  pinMode(IN_START, INPUT);

  //attach interrupts
  attachInterrupt(digitalPinToInterrupt(IN_CUPS), cups, RISING);
  attachInterrupt(digitalPinToInterrupt(IN_STRENGTH), strength, RISING);
  attachInterrupt(digitalPinToInterrupt(IN_GRINDER), grinder, RISING);
  attachInterrupt(digitalPinToInterrupt(IN_START), start, RISING);

  //send message to domoticz
  String message = "Startup reason, ";
  message += ESP.getResetReason().c_str();
  sendLog(message);

  #ifdef DEBUG
    Serial.printf("Coffeepot ready! Open http://%s.local/ in your browser\n", HOST);
  #endif
}

/*
 * ArduinoOTA handele
 */
void setupArduinoOTA() {
    //Arduino OTA code
   ArduinoOTA.setHostname(HOST);
   ArduinoOTA.onStart([]() {
   });
   ArduinoOTA.onEnd([]() {
   });
   ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
   });
   ArduinoOTA.onError([](ota_error_t error) {
     if (error == OTA_BEGIN_ERROR) sendLog("Flash Begin Failed");
     else if (error == OTA_CONNECT_ERROR) sendLog("Flash Connect Failed");
     else if (error == OTA_RECEIVE_ERROR) sendLog("Flash Receive Failed");
     else if (error == OTA_END_ERROR) sendLog("Flash End Failed");
   });
   ArduinoOTA.begin();
}
/*
 * Webserver handles
 */
 void handleRoot() {
   #ifdef DEBUG
     Serial.println("handleRoot");
   #endif
   INDEX_HTML = SPIFFS.open("/index.html.gz", "r");
   if(!INDEX_HTML) {
     handleNotFound();
     return;
   }
   SERVER.streamFile(INDEX_HTML, "text/html");
   INDEX_HTML.close();
 }
 void handleCss() {
   #ifdef DEBUG
     Serial.println("handleCss");
   #endif
   STYLE_CSS = SPIFFS.open("/style.css.gz", "r");
   if(!STYLE_CSS) {
     handleNotFound();
     return;
   }
   SERVER.streamFile(STYLE_CSS, "text/css");
   STYLE_CSS.close();
 }
void handleVarsJs() {
  #ifdef DEBUG
    Serial.println("varsJs");
    char bufferDebug[60];
    sprintf(bufferDebug, "cupsCount:%d, strengthCount:%d, grinderCount:%d, start:%d", CUPSCOUNT, STRENGTHCOUNT, GRINDERCOUNT, START);
    Serial.println(bufferDebug);
  #endif
  char buffer[100];
  String js = "var cupsCount = %d; var strengthCount = %d; var grinderCount = %d; var start = %d;";
  sprintf(buffer, js.c_str(), CUPSCOUNT, STRENGTHCOUNT, GRINDERCOUNT, START);
  SERVER.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  SERVER.sendHeader("Pragma", "no-cache");
  SERVER.send(200, "text/javascript", buffer);
 }
void handleMainJs() {
  #ifdef DEBUG
    Serial.println("handleMainJs");
  #endif
  MAIN_JS = SPIFFS.open("/main.min.js.gz", "r");
  if(!MAIN_JS) {
    handleNotFound();
    return;
  }
  SERVER.streamFile(MAIN_JS, "text/javascript");
  MAIN_JS.close();
}
void handleCups() {
  cups();
  #ifdef DEBUG
    Serial.println("handleCups");
  #endif

  //switch physical button
  unsigned long firstMillis = millis();
  while(millis() - firstMillis < 500) {
    if(digitalRead(OUT_CUPS) != HIGH) digitalWrite(OUT_CUPS, HIGH);
  }
  digitalWrite(OUT_CUPS, LOW);

  SERVER.send(200, "text/plain", "ok");
}
void handleStrength() {
  strength();
  #ifdef DEBUG
    Serial.println("handleStrength");
  #endif

  //switch physical button
  unsigned long firstMillis = millis();
  while(millis() - firstMillis < 500) {
    if(digitalRead(OUT_STRENGTH) != HIGH) digitalWrite(OUT_STRENGTH, HIGH);
  }
  digitalWrite(OUT_STRENGTH, LOW);

  SERVER.send(200, "text/plain", "ok");
}
void handleGrinder() {
  grinder();
  #ifdef DEBUG
    Serial.println("handleGrinder");
  #endif

  //switch physical button
  unsigned long firstMillis = millis();
  while(millis() - firstMillis < 500) {
    if(digitalRead(OUT_GRINDER) != HIGH) digitalWrite(OUT_GRINDER, HIGH);
  }
  digitalWrite(OUT_GRINDER, LOW);

  SERVER.send(200, "text/plain", "ok");
}
void handleStart() {
  start();
  #ifdef DEBUG
    Serial.println("handleStart");
  #endif

  //switch physical button
  unsigned long firstMillis = millis();
  while(millis() - firstMillis < 500) {
    if(digitalRead(OUT_START) != HIGH) digitalWrite(OUT_START, HIGH);
  }
  digitalWrite(OUT_START, LOW);

  SERVER.send(200, "text/plain", "ok");
}
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += SERVER.uri();
  message += "\nMethod: ";
  message += (SERVER.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += SERVER.args();
  message += "\n";
  for (uint8_t i=0; i<SERVER.args(); i++){
    message += " " + SERVER.argName(i) + ": " + SERVER.arg(i) + "\n";
  }
  SERVER.send(404, "text/plain", message);
}
void handleRestart() {
  String message = "ESP restarting";
  SERVER.send(200, "text/plain", message);
  ESP.restart();
}

void setupWebserver() {
  MDNS.begin(HOST);
  HTTP_UPDATER.setup(&SERVER);

  SERVER.on("/", handleRoot);
  SERVER.on("/style.css", handleCss);
  SERVER.on("/vars.js", handleVarsJs);
  SERVER.on("/main.min.js", handleMainJs);

  SERVER.on("/cups", handleCups);
  SERVER.on("/strength", handleStrength);
  SERVER.on("/grinder", handleGrinder);
  SERVER.on("/start", handleStart);
  SERVER.onNotFound(handleNotFound);
  SERVER.on("/restart", handleRestart);

  SERVER.begin();
  MDNS.addService("http", "tcp", 80);
}

/**
 * Functions to handle buttonstates
 **/
void cups() {
  unsigned long firstMillis = millis();
  while(millis() - firstMillis < 500) { //debounce
    CUPSCOUNT++;
    if(CUPSCOUNT == 5) CUPSCOUNT = 0;
  }
  #ifdef DEBUG
    char bufferDebug[20];
    sprintf(bufferDebug, "cupsCount:%d", CUPSCOUNT);
    Serial.println(bufferDebug);
  #endif
}
void strength() {
  unsigned long firstMillis = millis();
  while(millis() - firstMillis < 500) { //debounce
    STRENGTHCOUNT++;
    if(STRENGTHCOUNT == 3) STRENGTHCOUNT = 0;
  }
  #ifdef DEBUG
    char bufferDebug[20];
    sprintf(bufferDebug, "strengthCount:%d", STRENGTHCOUNT);
    Serial.println(bufferDebug);
  #endif
}
void grinder() {
  unsigned long firstMillis = millis();
  while(millis() - firstMillis < 500) { //debounce
    GRINDERCOUNT++;
    if(GRINDERCOUNT == 2) GRINDERCOUNT = 0;
  }
  #ifdef DEBUG
    char bufferDebug[20];
    sprintf(bufferDebug, "grinderCount:%d", GRINDERCOUNT);
    Serial.println(bufferDebug);
  #endif
}
void start() {
  unsigned long firstMillis = millis();
  while(millis() - firstMillis < 500) { //debounce
    START++;
    if(START == 2) START = 0;
  }
  #ifdef DEBUG
    char bufferDebug[60];
    sprintf(bufferDebug, "start:%d", START);
    Serial.println(bufferDebug);
  #endif
}

/*
 * Function to send request to Domoticz by URL
 * Uses URL decoder from http://hardwarefun.com/tutorials/url-encoding-in-arduino
 */
void sendRequest(String urlString) {
  const char* url = urlString.c_str();
  String encodedUrl;
  const char *hex = "0123456789abcdef";

  while (*url != '\0') {
    if ( ('a' <= *url && *url <= 'z')
         || ('A' <= *url && *url <= 'Z')
         || ('0' <= *url && *url <= '9')
         || ('/' == *url)
         || ('?' == *url)
         || ('=' == *url)
         || ('&' == *url)) {
      encodedUrl += *url;
    } else {
      encodedUrl += '%';
      encodedUrl += hex[*url >> 4];
      encodedUrl += hex[*url & 15];
    }
    url++;
  }

  String ipaddress;
  ipaddress += DOMOTICZIP[0];
  ipaddress += ".";
  ipaddress += DOMOTICZIP[1];
  ipaddress += ".";
  ipaddress += DOMOTICZIP[2];
  ipaddress += ".";
  ipaddress += DOMOTICZIP[3];

  if (CLIENT.connect(DOMOTICZIP, 8080)) {
    // Make a HTTP request:
    CLIENT.print("GET ");
    CLIENT.print(encodedUrl);
    CLIENT.print(" HTTP/1.1\r\n");
    CLIENT.print("Host: ");
    CLIENT.print(ipaddress);
    CLIENT.print("\r\n");
    CLIENT.print("Accept: */*\r\n");
    CLIENT.print("User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n");
    CLIENT.print("\r\n");
  } else {
    #ifdef DEBUG
      Serial.println("Connection to Domoticz server failed");
    #endif
  }
}

/* Function to add message to the Domoticz log */
void sendLog(String message) {
  sendRequest("/json.htm?type=command&param=addlogmessage&message=CoffeePot: " + message);
}

/* Main loop */
void loop() {
  ArduinoOTA.handle();
  SERVER.handleClient();
}
