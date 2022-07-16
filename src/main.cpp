#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI   23
#define OLED_CLK    18
#define OLED_DC     16
#define OLED_CS     5
#define OLED_RESET  17

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
const char* ssid = "yourssid";
const char* password = "yourpassword";

String API_KEY = "apikeyfromcoinmarketcap";
String CONVERT_TO = "TRY";

String serverName;
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

double price;
double percent_change_24h;
double percent_change_7d;
double percent_change_30d;


void getCoin(String SYMBOL) {

  if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      serverName = "https://pro-api.coinmarketcap.com/v1/cryptocurrency/quotes/latest?CMC_PRO_API_KEY=" + API_KEY + "&symbol=" + SYMBOL + "&convert=" + CONVERT_TO;
      http.begin(serverName);
      int httpResponseCode = http.GET();
      if (httpResponseCode>0) {

        String payload = http.getString();
        DynamicJsonDocument doc(3072);
        DeserializationError error = deserializeJson(doc, payload);
        JsonObject data = doc["data"][SYMBOL];
        JsonObject coin = data["quote"][CONVERT_TO];
        price = coin["price"]; // 366058.12904759543
        percent_change_24h = coin["percent_change_24h"]; // 0.86038791
        percent_change_7d = coin["percent_change_7d"]; // -4.1280132
        percent_change_30d = coin["percent_change_30d"];

        Serial.println(SYMBOL + ": " +String(price) + " TRY");
        Serial.println(String(percent_change_24h)+"%");
        display.clearDisplay(); 
        display.setTextSize(2.3);
        display.setCursor(48,0);
        display.setTextColor(SSD1306_WHITE);
        display.println(SYMBOL);
        display.setCursor(0,25);
        display.setTextSize(1.8);
        display.println("Price: "+ String(price)+" TRY");
        display.setCursor(0,35);
        display.println("24h: "+ String(percent_change_24h)+"%");
        display.setCursor(0,45);
        display.println("7d: "+String(percent_change_7d)+"%");
        display.setCursor(0,55);
        display.println("30d: "+String(percent_change_30d)+"%");
        display.display();
        delay(5000);

if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}
}}}
}


void setup() {
  Serial.begin(115200);
   if(!display.begin(SSD1306_SWITCHCAPVCC))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.display();
  delay(2000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println(WiFi.localIP());
  Serial.println("started");
}

void loop() {
  getCoin("BTC");
  getCoin("ETH");
  getCoin("ADA");
  getCoin("SOL");
}