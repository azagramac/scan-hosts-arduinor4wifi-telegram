#include <WiFi.h>
#include <HTTPClient.h>
#include <UNOR4WMatrixGFX.h>

const char* TOKEN = "YOUR_TOKEN";
const char* CHATID = "YOUR_CHATID";
String api_telegram = "https://api.telegram.org/bot" + String(TOKEN) + "/sendMessage?parse_mode=HTML";

UNOR4WMatrixGFX matrix;

const char* HOME_SSID = "YOUR_SSID";
const char* HOME_PASS = "YOUR_PASS_SSID";

void animationSearching(){
  for(int x=0;x<12;x++){
    matrix.clear();
    for(int y=0;y<8;y++){
      matrix.drawPixel(x,y,LED_ON);
    }
    matrix.writeDisplay();
    delay(100);
  }
}

void animationConnected(){
  int centerX=5, centerY=3;
  for(int layer=0; layer<6; layer++){
    matrix.clear();
    for(int dx=-layer; dx<=layer; dx++){
      for(int dy=-layer; dy<=layer; dy++){
        int px=centerX+dx, py=centerY+dy;
        if(px>=0 && px<12 && py>=0 && py<8) matrix.drawPixel(px,py,LED_ON);
      }
    }
    matrix.writeDisplay();
    delay(150);
  }
}

void animationHostProgress(int current, int total){
  int ledCount = map(current,0,total,0,12);
  matrix.clear();
  for(int x=0;x<ledCount;x++){
    for(int y=0;y<8;y++){
      matrix.drawPixel(x,y,LED_ON);
    }
  }
  matrix.writeDisplay();
}

void animationMessageSent(){
  for(int y=7; y>=0; y--){
    matrix.clear();
    for(int row=y; row<8; row++){
      for(int x=0;x<12;x++){
        matrix.drawPixel(x,row,LED_ON);
      }
    }
    matrix.writeDisplay();
    delay(150);
  }
}

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

void setup() {
  Serial.begin(115200);
  matrix.begin();
  matrix.clear();

  String ssidConnect="";
  bool connected=false;
  int encryptionTypeConnected=WIFI_AUTH_OPEN;

  while(!connected){
    animationSearching();
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
    animationSearching();
    if(millis()-startTime>15000) break;
    delay(200);
  }

  animationConnected();

  IPAddress ip = WiFi.localIP();
  IPAddress subnet = WiFi.subnetMask();
  uint32_t ipStart = (uint32_t(ip) & uint32_t(subnet)) + 1;
  uint32_t ipEnd = (uint32_t(ip) | ~uint32_t(subnet)) - 1;

  int hostsCount=0;
  String hostsList="";
  int totalHosts = ipEnd-ipStart+1;
  for(uint32_t addr=ipStart; addr<=ipEnd; addr++){
    IPAddress testIP((addr>>24)&0xFF,(addr>>16)&0xFF,(addr>>8)&0xFF,addr&0xFF);
    if(WiFi.ping(testIP)>=0){
      hostsCount++;
      hostsList += testIP.toString() + ", ";
    }
    if(addr%2==0) animationHostProgress(addr-ipStart+1,totalHosts);
  }

  String msg = "<b>Red:</b> " + ssidConnect + "\n";
  msg += "<b>Encriptacion:</b> ";
  msg += (encryptionTypeConnected == WIFI_AUTH_OPEN) ? "OPEN" : "WPA/WPA2";
  msg += "\n<b>IP:</b> " + ip.toString() + "\n";
  msg += "<b>Hosts activos:</b> " + String(hostsCount) + "\n";
  msg += "<b>IPs:</b> " + hostsList;

  sendTelegram(msg);
  animationMessageSent();
}

void loop(){}
