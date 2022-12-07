/*
file name: final_proj.ino
authors: Kelvin Shi
revision date: 11/15/2022
*/

#include "LedControl.h"
#include <LiquidCrystal.h>
#include <IRremote.h>

LedControl lc = LedControl(27,23,25,1);

LiquidCrystal lcd(32, 34, 22, 24, 26, 28);

#define LED 37

#define ENABLE 12
#define DIRA 8
#define DIRB 13

#define echoPin 5
#define trigPin 53

long duration;
int distance;

#define BLUE 9
#define GREEN 10
#define RED 11

int redValue = 0;
int greenValue = 0;
int blueValue = 0;

int buzzerValue = 1000;

bool on = false;


int IRPIN = 3;
IRrecv irrecv(IRPIN);
decode_results results;

#define buzzer 6


void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 5);
  lc.clearDisplay(0);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Distance:");

  pinMode(LED, OUTPUT);

  pinMode(ENABLE,OUTPUT);
  pinMode(DIRA,OUTPUT);
  pinMode(DIRB,OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  irrecv.enableIRIn();

}

void statusLED() {
  if (on == true) {
    digitalWrite(LED, HIGH);
  }
  if (on == false) {
    digitalWrite(LED, LOW);
  }
}

void translateIR() {
  switch(results.value) {
    
    case 0xFFA25D: on = true; statusLED(); break;
    case 0xFF629D: if (buzzerValue < 1000) {buzzerValue += 200;}; break;
    case 0xFFA857: if (buzzerValue > 0) {buzzerValue -= 200;}; break;
    case 0xFF02FD: on = false; statusLED(); break;

    default: lcd.setCursor(0, 1); lcd.write("               "); delay(200); lcd.setCursor(0, 1); lcd.write(" ");

  }

}

void writeHappy() {
  byte happy[8] = {B00111100, B01000010, B10101001, B10000101, B10000101, B10101001, B01000010, B00111100};
  lc.setRow(0,0,happy[0]);
  lc.setRow(0,1,happy[1]);
  lc.setRow(0,2,happy[2]);
  lc.setRow(0,3,happy[3]);
  lc.setRow(0,4,happy[4]);
  lc.setRow(0,5,happy[5]);
  lc.setRow(0,6,happy[6]);
  lc.setRow(0,7,happy[7]);
}

void writeSad() {
  byte sad[8] = {B00111100, B01000010, B10100101, B10001001, B10001001, B10100101, B01000010, B00111100};
  lc.setRow(0,0,sad[0]);
  lc.setRow(0,1,sad[1]);
  lc.setRow(0,2,sad[2]);
  lc.setRow(0,3,sad[3]);
  lc.setRow(0,4,sad[4]);
  lc.setRow(0,5,sad[5]);
  lc.setRow(0,6,sad[6]);
  lc.setRow(0,7,sad[7]);
}

void checkDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  lcd.setCursor(0, 0);
  lcd.print("Distance:");
  lcd.setCursor(10,0);
  lcd.print("    cm");
  lcd.setCursor(10,0);
  lcd.print(distance);
  delay(200);
}


void loop() {
  
  while (on == true){
    checkDistance();
    lcd.clear();
    while (distance < 20) {
      analogWrite(GREEN, 0);
      redValue = 255;
      blueValue = 3;
      //led go up
      for (int i = 0; i < 201; i+=3) {
        redValue -=3;
        blueValue +=4;

        analogWrite(RED, redValue);
        analogWrite(BLUE, blueValue);
        delay(10);
      }
      if (irrecv.decode(&results)) {
        translateIR();
        irrecv.resume();
      }
      
      analogWrite(buzzer, buzzerValue);
      digitalWrite(ENABLE, HIGH);
      digitalWrite(DIRA, LOW);
      digitalWrite(DIRB, HIGH);
      lc.clearDisplay(0);
      writeSad();
      delay(100);
      checkDistance();
    }
    analogWrite(buzzer, 0);
    digitalWrite(ENABLE, LOW);
    lc.clearDisplay(0);
    writeHappy();
    analogWrite(BLUE, 0);
    analogWrite(RED, 0);
    analogWrite(GREEN, 255);  
  if (irrecv.decode(&results)) {
    translateIR();
    irrecv.resume();
  }
  }

  if (on == false) {
    lc.clearDisplay(0);
    analogWrite(buzzer, 0);
    digitalWrite(echoPin, LOW);
    digitalWrite(ENABLE, LOW);
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.write("OFF!");
    analogWrite(GREEN, 0);
    if (irrecv.decode(&results)) {
      translateIR();
      irrecv.resume();
    }
  }
}
  
