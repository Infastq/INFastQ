#include <Arduino.h>
// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>
#include <string.h>

#define S0 33
#define S1 32
#define Out 27
#define S2 26
#define S3 25
#define IR 35

// // // Stores frequency read by the photodiodes
// // int redFrequency = 0;
// // int greenFrequency = 0;
// // int blueFrequency = 0;
// // int freq = 0;

// LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// // put function declarations here:
// // int getRed();
// // int getGreen();
// // int getBlue();

// int x = 0;

// void setup()
// {
//   pinMode(S0, OUTPUT);
//   pinMode(S1, OUTPUT);
//   pinMode(S2, OUTPUT);
//   pinMode(S3, OUTPUT);

//   // =========== Setting the IRSensorOut as an input ===========
//   pinMode(Out, INPUT);

//   // ============ Setting frequency scaling to 20% =================
//   digitalWrite(S0, HIGH);
//   digitalWrite(S1, LOW);

//   lcd.init(); // initialize the lcd

//   // Print a message to the LCD.
//   lcd.init();
//   lcd.backlight();
//   lcd.setCursor(0, 0);
//   lcd.print("  Turn counter ");
//   lcd.setCursor(0, 1);
//   lcd.print(x);
//   lcd.print("     =turn  ");

//   Serial.begin(9600);

//   // Write headers to the CSV file
//   // Serial.println("Red Frequency,Green Frequency,Blue Frequency");
// }

// void loop()
// {
//   // put your main code here, to run repeatedly:
//   IRSensor = digitalRead(IR);
//   // if (IRSensor == 0) {
//   //   redFrequency = getRed();
//   //   // delay(100);
//   //   greenFrequency = getGreen();
//   //   // delay(100);
//   //   blueFrequency = getBlue();
//   //   String freq = String(redFrequency) + "," + String(greenFrequency) + "," + String(blueFrequency);
//   //   Serial.println(freq);
//   //   // delay(100);
//   // }

//   if (IRSensor == LOW)
//   {
//     lcd.setCursor(0, 1);
//     x = x + 1;
//     lcd.print(x);
//   }
//   else
//   {
//     lcd.setCursor(0, 1);
//     lcd.print(x);
//   }
// }

// // int getRed()
// // {
// //   digitalWrite(S2, LOW);
// //   digitalWrite(S3, LOW);
// //   freq = pulseIn(Out, LOW);
// //   return freq;
// // }

// // int getGreen()
// // {
// //   digitalWrite(S2, HIGH);
// //   digitalWrite(S3, HIGH);
// //   freq = pulseIn(Out, LOW);
// //   return freq;
// // }

// // int getBlue()
// // {
// //   digitalWrite(S2, LOW);
// //   digitalWrite(S3, HIGH);
// //   freq = pulseIn(Out, LOW);
// //   return freq;
// // }

// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>

// int x = 0;
// // int input = A0;
// int state = 1;

// LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// void setup()
// {

//   pinMode(IR, INPUT);

//   lcd.init(); // initialize the lcd

//   // Print a message to the LCD.
//   lcd.init();
//   lcd.backlight();
//   lcd.setCursor(0, 0);
//   lcd.print("  Turn counter ");
//   lcd.setCursor(0, 1);
//   lcd.print(x);
//   lcd.print("     =turn  ");
// }
// void loop()
// {
//   int IRSensor = digitalRead(IR);
//   if (IRSensor == LOW)
//   {
//     lcd.setCursor(0, 1);
//     x = x + 1;
//     lcd.print(x);
//   }
//   else
//   {
//     lcd.setCursor(0, 1);
//     lcd.print(x);
//   }
// }
