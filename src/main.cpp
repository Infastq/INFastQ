#include <Arduino.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define S0 33
#define S1 32
#define Out 27
#define S2 26
#define S3 25
#define IR 35
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

char units[5];
unsigned long last_time = 0;
unsigned long current_time = 0;

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

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

void loop()
{
  // put your main code here, to run repeatedly:
  sensor = digitalRead(IR);
  if (sensor == 0)
  {
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
  http.begin("http://example.com/api/calculate"); // Change this to your server URL
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
  }
  else
  {
    Serial.print("HTTP Error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}