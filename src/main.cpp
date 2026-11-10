#include <Arduino.h>

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>

#include "webpage.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


constexpr uint8_t LED_PIN = 32;
constexpr uint8_t NUM_LEDS = 21;  // 7 Segmente * 3 LEDs pro Segment
constexpr int MAX_COUNTER_VALUE = 99;

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

const uint8_t digitFont[] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

void displayDigit(int number, uint32_t color) {
  if (number < 0 || number > 9) return;

  uint8_t segments = digitFont[number];

  for (int segment = 0; segment < 7; segment++) {
    int segmentStartIndex = segment * 3;
    bool isSegmentOn = segments & (1 << segment);

    for (int led = 0; led < 3; led++) {
      int currentLedIndex = segmentStartIndex + led;

      if (isSegmentOn) {
        strip.setPixelColor(currentLedIndex, color);
      } else {
        strip.setPixelColor(currentLedIndex, strip.Color(0, 0, 0));
      }
    }
  }
}

LiquidCrystal_I2C lcd(0x27, 16, 2);
WebServer server(80);
int counter = 0;

void connectToWiFi() {
  WiFiManager wifiManager;
  bool connected = wifiManager.autoConnect("AutoConnectAP", "password");

  if (connected) {
    Serial.println("connected...yeey :)");
  } else {
    Serial.println("Failed to connect");
  }
}

void handleCounterPost() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "Parameter fehlt");
    return;
  }

  int newValue = server.arg("value").toInt();
  if (newValue < 0 || newValue > MAX_COUNTER_VALUE) {
    server.send(400, "text/plain", "Ungueltiger Wert");
    return;
  }

  counter = newValue;
  Serial.print("Neuer Counter-Wert: ");
  Serial.println(counter);
  displayDigit(counter, strip.Color(0, 255, 0));
  strip.show();

  String json = "{\"value\":" + String(counter) + "}";
  server.send(200, "application/json", json);
}

void configureRoutes() {
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", webpage);
  });
  server.on("/counter", HTTP_POST, handleCounterPost);
}

void startWebServer() {
  configureRoutes();
  server.begin();
  Serial.println("Webserver gestartet!");
}

void initializeDisplays() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(WiFi.localIP().toString());

  strip.begin();
  strip.setBrightness(64);
  strip.show();
}

void initOled(){
  if(!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.setCursor(0,0);
  oled.print("IP: ");
  oled.println(WiFi.localIP());
  oled.display();
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  startWebServer();
  initializeDisplays();
  initOled();
}

void loop() {
  server.handleClient();
}