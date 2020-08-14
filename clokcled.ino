#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <WiFiUdp.h>
#include "time.h"

const char *ssid     = "your name wifi";      //Tên wifi
const char *password = "your pass";  //Mật khẩu

String weekDays[7]={"Chu Nhat", "Thu Hai", "Thu Ba", "Thu Tu", "Thu Nam", "Thu Sau", "Thu Bay"};

// LED pin  --> ESP
// DIN      --> D7
// CS       --> D6
// CLK      --> D5
#define pinCS             D6
#define DHTPin            D4
#define DHTTYPE           DHT11

//String date;
String weatherString;

int refresh = 0;

int HorizontalDisplays = 4;  // Số Lượng Led Matrix Ngang
int VerticalDisplays = 1;    // Số Lượng Led Matrix dọc
int t = 0;
int h = 0;
Max72xxPanel matrix = Max72xxPanel(pinCS, HorizontalDisplays, VerticalDisplays);

int wait = 20;          // tốc độ chạy, càng lớn càng chạy chậm
int spacer = 2;
int width = 5 + spacer; // Khoảng cách ký tự có thể điều chỉnh

//WiFiClient client;
DHT dht(DHTPin, DHTTYPE);

WiFiUDP ntpUDP;

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP, "3.asia.pool.ntp.org", 7*3600, 60000);

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
// NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void setup()
{
  Serial.begin(115200);
  dht.begin();
  
  pinMode(DHTPin, INPUT);
 
  matrix.setIntensity(5); // điều chỉnh độ sáng 0-15

  // Tọa Độ Led Matrix bắt đầu 8 * 8
  matrix.setRotation(0, 1);        // 1
  matrix.setRotation(1, 1);        // 2
  matrix.setRotation(2, 1);        // 3
  matrix.setRotation(3, 1);        // 4

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) 
  {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println( "Got Wifi" );
  timeClient.begin();
}

void loop() 
{
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900; 
  String currentDate = String(monthDay) + "-" + String(currentMonth) + "-" + String(currentYear);
  h = dht.readHumidity();
  t = dht.readTemperature();

  weatherString=weekDays[timeClient.getDay()];
  ScrollText(weatherString);
  ScrollText(currentDate);
  hienthi_t_h();
  DisplayTime();
  delay(30000);
}

void hienthi_t_h()
{
  
  weatherString = "Nhiet do:" + String(t) + "`C";
  ScrollText(weatherString);
    
  weatherString = "Do am:" + String(h) + "%";
  ScrollText(weatherString); 
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
