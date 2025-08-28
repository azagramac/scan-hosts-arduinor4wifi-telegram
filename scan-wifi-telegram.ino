#include <WiFi.h>
#include <HTTPClient.h>
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

const char* TOKEN = "YOUR_TOKEN";
const char* CHATID = "YOUR_CHATID";
String api_telegram = "https://api.telegram.org/bot" + String(TOKEN) + "/sendMessage?parse_mode=HTML";

const char* HOME_SSID = "YOUR_SSID";
const char* HOME_PASS = "YOUR_PASS_SSID";

ArduinoLEDMatrix matrix;

const uint32_t searchFrames[][4] = {
  {0x80000000,0x0,0x0,100},{0x40000000,0x0,0x0,100},{0x20000000,0x0,0x0,100},{0x10000000,0x0,0x0,100},
  {0x8000000,0x0,0x0,100},{0x4000000,0x0,0x0,100},{0x2000000,0x0,0x0,100},{0x1000000,0x0,0x0,100}
};
const uint32_t connectedFrames[][4] = {
  {0x00001800,0x0,0x0,150},{0x00003C00,0x0,0x0,150},{0x00007E00,0x0,0x0,150},{0x0000FF00,0x0,0x0,150}
};
const uint32_t hostsFrames[][4] = {
  {0x000000FF,0x0,0x0,50},{0x0000FF00,0x0,0x0,50},{0x00FF0000,0x0,0x0,50},{0xFF000000,0x0,0x0,50}
};
const uint32_t sentFrames[][4] = {
  {0x00000001,0x0,0x0,100},{0x00000003,0x0,0x0,100},{0x00000007,0x0,0x0,100},{0x0000000F,0x0,0x0,100},
  {0x0000001F,0x0,0x0,100},{0x0000003F,0x0,0x0,100},{0x0000007F,0x0,0x0,100},{0x000000FF,0x0,0x0,100}
};

void sendTelegram(String message){
  if(WiFi.status()==WL_CONNECTED){
    HTTPClient http;
    http.begin(api_telegram);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String postData = "chat_id=" + String(CHATID) + "&text=" + message;
    http.POST(postData);
    http.end();
  }
}

void showText(const char* txt){
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFFFF);
  matrix.println(txt);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}

void setup() {
  Serial.begin(115200);
  matrix.begin();

  String ssidConnect="";
  bool connected=false;
  int encryptionTypeConnected=WIFI_AUTH_OPEN;

  matrix.loadSequence(searchFrames);
  matrix.play(true);
  showText("Searching");

  while(!connected){
    int n = WiFi.scanNetworks();
    for(int i=0;i<n;i++){
      if(WiFi.SSID(i) == HOME_SSID){
        ssidConnect = HOME_SSID;
        encryptionTypeConnected = WiFi.encryptionType(i);
        WiFi.begin(HOME_SSID, HOME_PASS);
        connected=true;
        break;
      }
    }
    if(!connected){
      for(int i=0;i<n;i++){
        if(WiFi.encryptionType(i) == WIFI_AUTH_OPEN){
          ssidConnect = WiFi.SSID(i);
          encryptionTypeConnected = WIFI_AUTH_OPEN;
          WiFi.begin(ssidConnect);
          connected=true;
          break;
        }
      }
    }
    delay(500);
  }

  unsigned long startTime = millis();
  while(WiFi.status()!=WL_CONNECTED){
    if(millis()-startTime>15000) break;
    showText("Searching");
    delay(200);
  }

  matrix.loadSequence(connectedFrames);
  matrix.play(true);
  showText("Connected");

  IPAddress ip = WiFi.localIP();
  IPAddress subnet = WiFi.subnetMask();
  uint32_t ipStart = (uint32_t(ip) & uint32_t(subnet)) + 1;
  uint32_t ipEnd = (uint32_t(ip) | ~uint32_t(subnet)) - 1;

  int hostsCount=0;
  String hostsList="";
  int totalHosts = ipEnd-ipStart+1;
  matrix.loadSequence(hostsFrames);
  matrix.play(true);
  showText("Hosts");

  for(uint32_t addr=ipStart; addr<=ipEnd; addr++){
    IPAddress testIP((addr>>24)&0xFF,(addr>>16)&0xFF,(addr>>8)&0xFF,addr&0xFF);
    if(WiFi.ping(testIP)>=0){
      hostsCount++;
      hostsList += testIP.toString() + ", ";
    }
  }

  String msg = "<b>Network:</b> " + ssidConnect + "\n";
  msg += "<b>Encryption:</b> ";
  msg += (encryptionTypeConnected == WIFI_AUTH_OPEN) ? "OPEN" : "WPA/WPA2";
  msg += "\n<b>IP:</b> " + ip.toString() + "\n";
  msg += "<b>Active hosts:</b> " + String(hostsCount) + "\n";
  msg += "<b>IPs:</b> " + hostsList;

  matrix.loadSequence(sentFrames);
  matrix.play(true);
  showText("Sending");
  sendTelegram(msg);
}

void loop(){}
