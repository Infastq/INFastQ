#include <Arduino.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <TinyGPS++.h>
#include <string>
#include <Adafruit_GFX.h>
#include <FS.h>
#include <SPIFFS.h>
#include <Adafruit_ST7789.h>
#include <qrcode.h>

using namespace std;

#define S0 33
#define S1 32
#define Out 27
#define S2 26
#define S3 25
#define IR 35
#define RXD2 16
#define TXD2 17
#define TFT_DC      2     
#define TFT_RST     4     
#define TFT_CS     23

HardwareSerial neogps(1);
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

TinyGPSPlus gps;

float pi = 3.1415926;

int pos = 0;

int on = false;

int sensor = 1;

const char *ssid = "SiPalingPaling";
const char *password = "31415926";

// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbyr48bsFpnYg2lPdlOwnD0SrerXouHf7-vivWSiGKyiz6M3GeN74LbnubV3T1QFKFG5wg"; // change Gscript ID

// Stores frequency read by the photodiodes
int red = 0;
int green = 0;
int blue = 0;
int freq = 0;

// put function declarations here:
int getRed();
int getGreen();
int getBlue();
void sendHttpRequest(int red, int green, int blue);
void visualisasi_GPS_lcd (void);
void visualisasi_GPS_Serial (void);
void testlines(uint16_t color);
void testdrawtext(char *text, uint16_t color);
void testfastlines(uint16_t color1, uint16_t color2);
void testdrawrects(uint16_t color);
void testfillrects(uint16_t color1, uint16_t color2);
void testfillcircles(uint8_t radius, uint16_t color);
void testdrawcircles(uint8_t radius, uint16_t color);
void testtriangles();
void testroundrects();
void tftPrintTest();
void mediabuttons();

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

char units[5];
unsigned long last_time = 0;
unsigned long current_time = 0;

bool isCollectingData = false;

void setup()
{
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // =========== Setting the sensorOut as an input ===========
  pinMode(Out, INPUT);
  pinMode(IR, INPUT);
  // ============ Setting frequency scaling to 20% =================
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  Serial.begin(9600);
  delay(10);

  Wire.begin();

  // initialize LCD
  lcd.begin();
  // turn on LCD backlight
  lcd.backlight();

  // //Begin serial communication Neo6mGPS
  // neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  // initialize tft
  tft.init(240, 240, SPI_MODE3);
  tft.setRotation(2);

  // Display a QR Code
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)]; // Use larger ECC level and size
  qrcode_initText(&qrcode, qrcodeData, 3, ECC_MEDIUM, "https://www.example.com"); // Change this URL

  tft.fillScreen(ST77XX_BLACK);
  uint16_t pixelColor = ST77XX_WHITE;
  uint8_t moduleSize = tft.width() / qrcode.size; // Calculate size of each QR code module
  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule(&qrcode, x, y)) {
        tft.fillRect(x * moduleSize, y * moduleSize, moduleSize, moduleSize, pixelColor);
      }
    }
  }

  delay(5000); // Display for 5 seconds
}

String value = "";
void loop()
{
  // while(ss.available()>0)
  //   gps.encode(ss.read());
  // visualisasi_GPS_lcd ();
  // Visualisasi_GPS_Serial();

  // put your main code here, to run repeatedly:
  sensor = digitalRead(IR);
  if (sensor == 0)
  {
    on = true;

    red = getRed();
    delay(100); /*wait a 200mS*/
    green = getGreen();
    delay(100); /*wait a 200mS*/
    blue = getBlue();
    delay(100);        /*wait a 200mS*/
    Serial.print(red); /*Print Red Color Value on Serial Monitor*/
    Serial.print(",");
    Serial.print(green); /*Print Green Color Value on Serial Monitor*/
    Serial.print(",");
    Serial.println(blue); /*Print Blue Color Value on Serial Monitor*/
    
    lcd.clear();
    String rgb = String(red) + "," + String(green) + "," + String(blue);
    lcd.setCursor(0, 0);
    lcd.print(rgb);
    if (isCollectingData){
      String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "red=" + String(red) + "&green=" + String(green) + "&blue=" + String(blue);
      HTTPClient http;
      http.begin(urlFinal.c_str());
      http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      int httpCode = http.GET();

      String payload;
      if (httpCode > 0)
      {
        payload = http.getString();
        Serial.println("Payload: " + payload);
      }
      //---------------------------------------------------------------------
      http.end();
      Serial.println();
    } else{
      sendHttpRequest(red, green, blue);
      delay(10000);
    }
    
  }
}

int getRed()
{
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  freq = pulseIn(Out, LOW); /*Get the Red Color freq*/
  return freq;
}

int getGreen()
{
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  freq = pulseIn(Out, LOW); /*Get the Green Color freq*/
  return freq;
}

int getBlue()
{
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  freq = pulseIn(Out, LOW); /*Get the Blue Color freq*/
  return freq;
}

void sendHttpRequest(int red, int green, int blue)
{
  HTTPClient http;
  http.begin("https://infastq-api-production.up.railway.app/api/calculate/"); // Change this to your server URL
  http.addHeader("Content-Type", "application/json");

  // Create JSON payload
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["red"] = red;
  jsonDoc["green"] = green;
  jsonDoc["blue"] = blue;
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Send POST request
  int httpResponseCode = http.POST(jsonString);

  if (httpResponseCode > 0)
  {
    String response = http.getString();
    Serial.println("HTTP Response: " + response);
    // Parse the JSON response
    DynamicJsonDocument jsonResp(200);
    deserializeJson(jsonResp, response);

    // Extract the "data" value
    String value = jsonResp["data"];
    Serial.print("Data Value: ");
    Serial.println(value);
    lcd.setCursor(0,1);
    lcd.print(value);
  }
  else
  {
    Serial.print("HTTP Error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

// #include <Arduino.h>
// #include <Wire.h>
// #include <TinyGPSPlus.h>

// // The TinyGPSPlus object

// TinyGPSPlus gps;

// void displayInfo();
// void updateSerial();

// void updateSerial()
// {

//   delay(500);
//   while (Serial.available())
//   {
//     Serial2.write(Serial.read()); // Forward what Serial received to Software Serial Port
//   }
//   while (Serial2.available())
//   {
//     Serial.write(Serial2.read()); // Forward what Software Serial received to Serial Port
//   }
// }


// void displayInfo()
// {
//   Serial.print(F("Location: "));

//   if (gps.location.isValid())
//   {

//     Serial.print("Lat: ");

//     Serial.print(gps.location.lat(), 6);

//     Serial.print(F(","));

//     Serial.print("Lng: ");

//     Serial.print(gps.location.lng(), 6);

//     Serial.println();
//   }
//   else
//   {
//     Serial.print(F("INVALID"));
//   }
// }

// void setup()
// {

//   Serial.begin(9600);

//   Serial2.begin(9600);

//   delay(3000);
// }

// void loop()
// {
//   updateSerial();

//   while (Serial2.available() > 0)
//   {
//     if (gps.encode(Serial2.read()))
//     {
//       displayInfo();
//     }
//   }

//   if (millis() > 5000 && gps.charsProcessed() < 10)
//   {
//     Serial.println(F("No GPS detected: check wiring."));

//     while (true)
//       ;
//   }
// }

// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>  
// #include <TinyGPS++.h>
// #include <SoftwareSerial.h>
// #include <HardwareSerial.h>

// #define RXD2 16
// #define TXD2 17
// char datoCmd = 0;

// #define NMEA 0
// #define I2C_ADDR 0x27
// LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);
// // SoftwareSerial ss(16,17);
// HardwareSerial ss(1);
// TinyGPSPlus gps;

// //------------------------------------------------------------------------------------------------------------------------------------------

// void setup() 
// {
//   Wire.begin(21, 22);
//   Serial.begin(115200); 
//   ss.begin(9600, SERIAL_8N1, RXD2, TXD2);
//   lcd.begin ();                 
//   //lcd.setBacklightPin(3,POSITIVE);  // Backlight OFF 
//   lcd.setBacklight(HIGH);           // Backlight ON 
//   lcd.clear();
//   lcd.print(" MULAI");  
//   delay(2000);

// }

//------------------------------------------------------------------------------------------------------------------------------------------

void visualisasi_GPS_lcd (void)
{
 lcd.clear();
 lcd.print("LAT: ");
 lcd.print(gps.location.lat(),6);
 lcd.setCursor (0, 1);
 lcd.print("LON: "); 
 lcd.print(gps.location.lng(),6);  
 delay (2000);
 lcd.clear();
 lcd.print("VEL: ");
 lcd.print(gps.speed.kmph());
 lcd.print("Km/h");
 lcd.setCursor (0, 1);
 lcd.print("SAT: "); 
 lcd.print(gps.satellites.value());
 delay (2000);
 lcd.clear();
 lcd.print("ALT: ");
 lcd.print(gps.altitude.meters(), 0); 
 delay (2000);
 lcd.clear();
 lcd.print("HOR: "); 
 lcd.print(gps.time.hour());
 lcd.print(":");
 lcd.print(gps.time.minute());
 lcd.print(":");
 lcd.print(gps.time.second());
 lcd.setCursor (0, 1);
 lcd.print("FEC: "); 
 lcd.print(gps.date.day());
 lcd.print("/");
 lcd.print(gps.date.month());
 lcd.print("/");
 lcd.print(gps.date.year()); 
 delay (2000);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Visualisasi_GPS_Serial(void)
{ 
   
  if (gps.location.isUpdated())
  {  
    Serial.print("Lat: ");
    Serial.println(gps.location.lat(),6);
    Serial.print("Lng: ");
    Serial.println(gps.location.lng(),6);  
    Serial.print("Speed: ");
    Serial.println(gps.speed.kmph());    
    Serial.print("SAT:");
    Serial.println(gps.satellites.value());
    Serial.print("ALT:");   
    Serial.println(gps.altitude.meters(), 0);     

    Serial.print("Date: ");
    Serial.print(gps.date.day()); Serial.print("/");
    Serial.print(gps.date.month()); Serial.print("/");
    Serial.println(gps.date.year());

    Serial.print("Hour: ");
    Serial.print(gps.time.hour()); Serial.print(":");
    Serial.print(gps.time.minute()); Serial.print(":");
    Serial.println(gps.time.second());
    Serial.println("---------------------------");
  }
  else
  {
    Serial.println("No GPS Signal");  
  }  
}

void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, 0, x, tft.height() - 1, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, 0, tft.width() - 1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width() - 1, 0, x, tft.height() - 1, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width() - 1, 0, 0, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, tft.height() - 1, x, 0, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, tft.height() - 1, tft.width() - 1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width() - 1, tft.height() - 1, x, 0, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width() - 1, tft.height() - 1, 0, y, color);
    delay(0);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y = 0; y < tft.height(); y += 5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x = 0; x < tft.width(); x += 5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = tft.width() - 1; x > 6; x -= 6) {
    tft.fillRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2 , x, x, color1);
    tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x = radius; x < tft.width(); x += radius * 2) {
    for (int16_t y = radius; y < tft.height(); y += radius * 2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x = 0; x < tft.width() + radius; x += radius * 2) {
    for (int16_t y = 0; y < tft.height() + radius; y += radius * 2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST77XX_BLACK);
  int color = 0xF800;
  int t;
  int w = tft.width() / 2;
  int x = tft.height() - 1;
  int y = 0;
  int z = tft.width();
  for (t = 0 ; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x -= 4;
    y += 4;
    z -= 4;
    color += 100;
  }
}

void testroundrects() {
  tft.fillScreen(ST77XX_BLACK);
  int color = 100;
  int i;
  int t;
  for (t = 0 ; t <= 4; t += 1) {
    int x = 0;
    int y = 0;
    int w = tft.width() - 2;
    int h = tft.height() - 2;
    for (i = 0 ; i <= 16; i += 1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x += 2;
      y += 3;
      w -= 4;
      h -= 6;
      color += 1100;
    }
    color += 100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  tft.print(pi, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(" seconds.");
}

void mediabuttons() {
  // play
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST77XX_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_GREEN);
}

// void loop() 
// {
//   while(ss.available()>0)
//     gps.encode(ss.read());
//   visualisasi_GPS_lcd ();
//   Visualisasi_GPS_Serial();
// }

//------------------------------------------------------------------------------------------------------------------------------------------
/*
 * Rui Santos 
 * Complete Project Details https://randomnerdtutorials.com
 */
 
// #include <SoftwareSerial.h>

// // The serial connection to the GPS module
// SoftwareSerial ss(4, 3);

// void setup(){
//   Serial.begin(9600);
//   ss.begin(9600);
// }

// void loop(){
//   while (ss.available() > 0){
//     // get the byte data from the GPS
//     byte gpsData = ss.read();
//     Serial.println(gpsData);
//   }
// }
