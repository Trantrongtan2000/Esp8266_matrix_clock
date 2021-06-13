#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <WiFiManager.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <WiFiUdp.h>
#include <Arduino_JSON.h>
#include <ESP8266HTTPClient.h>
#include "time.h"


String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

String weekDays[7]={"Chu Nhat", "Thu Hai", "Thu Ba", "Thu Tu", "Thu Nam", "Thu Sau", "Thu Bay"};

// LED pin  --> ESP
// DIN      --> D7
// CS       --> D6
// CLK      --> D5
#define pinCS             D6

String new1 = "https://api.thingspeak.com/apps/thinghttp/send_request?api_key=CFVHA6VGN9EI0VO7";
String new2 = "https://api.thingspeak.com/apps/thinghttp/send_request?api_key=4EX6UMGXBPFFFXJL";
String getnew1;
String getnew2;


//String date;
String weatherString;

int refresh = 0;

int HorizontalDisplays = 1;  // Số Lượng Led Matrix Ngang
int VerticalDisplays = 1;    // Số Lượng Led Matrix dọc

Max72xxPanel matrix = Max72xxPanel(pinCS, HorizontalDisplays, VerticalDisplays);

int wait = 30;          // tốc độ chạy, càng lớn càng chạy chậm
int spacer = 2;
int width = 5 + spacer; // Khoảng cách ký tự có thể điều chỉnh

//WiFiClient client;


WiFiUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP, "3.asia.pool.ntp.org", 7*3600, 60000);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void setup()
{
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);  
 
  matrix.setIntensity(5); // điều chỉnh độ sáng 0-15

  // Tọa Độ Led Matrix bắt đầu 8 * 8
  matrix.setRotation(0, 1);        // 1
  matrix.setRotation(1, 1);        // 2
  matrix.setRotation(2, 1);        // 3
  matrix.setRotation(3, 1); 
  WiFiManager wm;
  bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("MatrixClock01","trongtan2104"); // password protected ap

  if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
  else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }
  timeClient.begin();
}

void loop() 
{
  timeClient.update();
  getnew1= httpGETRequest(new1.c_str());
  getnew2= httpGETRequest(new2.c_str());
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900; 
  String currentDate = String(monthDay) + "-" + String(currentMonth) + "-" + String(currentYear);

  weatherString=weekDays[timeClient.getDay()];
  DisplayTime();
  ScrollText(weatherString);
  ScrollText(currentDate);
  ScrollText(getnew1);
  ScrollText(getnew2);
  delay(100);
}


// =======================================================================
void DisplayTime() 
{
  timeClient.update();
  matrix.fillScreen(LOW);
  int y = (matrix.height() - 8) / 2;
 
  matrix.drawChar(14, y, (String(":"))[0], HIGH, LOW,1);

  String hour1 = String (timeClient.getHours() / 10);
  String hour2 = String (timeClient.getHours() % 10);
  String min1 = String (timeClient.getMinutes() / 10);
  String min2 = String (timeClient.getMinutes() % 10);
  int xh = 2;
  int xm = 19;

  matrix.drawChar(xh, y, hour1[0], HIGH, LOW, 1);
  matrix.drawChar(xh + 7, y, hour2[0], HIGH, LOW, 1);
  matrix.drawChar(xm, y, min1[0], HIGH, LOW, 1);
  matrix.drawChar(xm + 7, y, min2[0], HIGH, LOW, 1);
  
  matrix.write();
}
// =======================================================================
void ScrollText (String text) 
{
  for ( int i = 0 ; i < width * text.length() + matrix.width() - 1 - spacer; i++ ) 
  {
    if (refresh == 1) 
      i = 0;
    
    refresh = 0;
    matrix.fillScreen(LOW);
    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2;

    while ( x + width - spacer >= 0 && letter >= 0 ) 
    {
      if ( letter < text.length() ) 
      {
        matrix.drawChar(x, y, text[letter], HIGH, LOW, 1);
      }
      letter--;
      x -= width;
    }
    matrix.write();
    delay(wait);
  }
}
