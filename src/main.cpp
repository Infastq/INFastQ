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

const String ssid = "yourssid";
const String passowrd = "yourpassword";

// Stores frequency read by the photodiodes
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;
int freq = 0;

// put function declarations here:
int getRed();
int getGreen();
int getBlue();
void sendHttpRequest(int red, int green, int blue);


void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // =========== Setting the sensorOut as an input =========== 
  pinMode(Out, INPUT);
  pinMode(IR, INPUT);
  // ============ Setting frequency scaling to 20% =================
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  Serial.begin(9600);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");

}

void loop() {
  // put your main code here, to run repeatedly:
  sensor = digitalRead(IR);
  if (sensor == 0){
    redFrequency = getRed();
    delay(100); /*wait a 200mS*/
    greenFrequency = getGreen();
    delay(100); /*wait a 200mS*/
    blueFrequency = getBlue();
    delay(100); /*wait a 200mS*/
    Serial.print(redFrequency); /*Print Red Color Value on Serial Monitor*/
    Serial.print(",");
    Serial.print(greenFrequency); /*Print Green Color Value on Serial Monitor*/
    Serial.print(",");
    Serial.println(blueFrequency); /*Print Blue Color Value on Serial Monitor*/
  }
  


}

int getRed() {
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  freq = pulseIn(Out, LOW); /*Get the Red Color freq*/
  return freq;
}

int getGreen() {
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  freq = pulseIn(Out, LOW); /*Get the Green Color freq*/
  return freq;
}

int getBlue() {
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  freq = pulseIn(Out, LOW); /*Get the Blue Color freq*/
  return freq;
}

void sendHttpRequest(int red, int green, int blue) {
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

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response: " + response);
  } else {
    Serial.print("HTTP Error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}