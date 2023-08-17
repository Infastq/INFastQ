#include <Arduino.h>

#define S0 33
#define S1 32
#define Out 27
#define S2 26
#define S3 25
#define IR 35
int sensor = 1;

// Stores frequency read by the photodiodes
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;
int freq = 0;

// put function declarations here:
int getRed();
int getGreen();
int getBlue();


void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // =========== Setting the sensorOut as an input =========== 
  pinMode(Out, INPUT);

  // ============ Setting frequency scaling to 20% =================
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  Serial.begin(9600);
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
    Serial.print("Red Freq = ");
    Serial.print(redFrequency); /*Print Red Color Value on Serial Monitor*/
    Serial.print("   ");
    Serial.print("Green Freq = ");
    Serial.print(greenFrequency); /*Print Green Color Value on Serial Monitor*/
    Serial.print("   ");
    Serial.print("Blue Freq = ");
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