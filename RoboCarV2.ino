#include <NewTone.h>
#include <SoftwareSerial.h>
#include <NewPing.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define TRIGGER_PIN 13
#define ECHO_PIN 12
#define MAX_DISTANCE 200
#define CW 0
#define CCW 1
#define MOTOR_A 0
#define MOTOR_B 1
#define buzzer A0

//sonar
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

//motor control assignments
const byte PWMA = 3;  //PWM control (speed) for motor A
const byte PWMB = 11; //PWM control (speed) for motor B
const byte DIRA = 2; //Direction control for motor A
const byte DIRB = 4; //Direction control for motor B
int spd;

//bluetooth initialization
SoftwareSerial HC05(6, 7); // RX | TX 
String msg, cmd;

//LCD Display
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

//car mode bools
bool autopilot, isBlinkingR, isBlinkingL, headlights, buzzerOn, buzzState, horn;

// LED vars
const int lBlinker = 8;
const int rBlinker = 9;
const int headlight1 = 10;
const int headlight2 = 5;
int lState = LOW;
int rState = LOW;
long blinkInterval = 500;
long previousMillis = 0;

void setup() {
  //Bluetooth Setup
  Serial.begin(9600);
  HC05.begin(9600);
  HC05.setTimeout(10);
  msg = "";

  //LCD Display Setup
  lcdDisplay();

  //Motor Setup
  setupArdumoto();
  spd = map(1, 1, 10, 150, 255);

  // Boolean initialization
  autopilot = false;
  isBlinkingR = false;
  isBlinkingL = false;
  headlights = false;
  buzzerOn = false;
  buzzState = false;
  horn = false;

  // LED Setup
  pinMode(lBlinker, OUTPUT);
  pinMode(rBlinker, OUTPUT);
  pinMode(headlight1, OUTPUT);
  pinMode(headlight2, OUTPUT);

  digitalWrite(lBlinker, LOW);
  digitalWrite(rBlinker, LOW);
  digitalWrite(headlight1, LOW);
  digitalWrite(headlight2, LOW);
  
}

void loop() {
  timedOutputs();
  
  if(HC05.available() > 0){
    msg = HC05.readString();
    Serial.println("Android Command: " + msg);
  }

  //Check for mode change
  if(msg == "a"){
    autopilot = !autopilot;
    msg = "";
    motorStop();
    }

  // Turn off/on turn signals
  else if (msg == "w"){
    isBlinkingL = !isBlinkingL;
    msg = "";
    }   
  else if (msg == "e"){
    isBlinkingR = !isBlinkingR;
    msg = "";
    }

  //headlights
  else if (msg == "h"){
    headlights = !headlights;
    msg = "";
    if(headlights){
      digitalWrite(headlight1, HIGH);
      digitalWrite(headlight2, HIGH);
    }
    else{
      digitalWrite(headlight1, LOW);
      digitalWrite(headlight2, LOW);
    }
  }

  //horn
  else if(msg == "hon" || msg == "hoff"){
    if (msg == "hon"){
      horn = true;
      NewTone(buzzer, 550);
    }
    else if(msg == "hoff"){
      horn = false;
      noNewTone(buzzer);
    }
    msg = "";
  }

  // Autopilot Mode
  if(autopilot){
    //delay(500);
    unsigned int uS = sonar.ping();
    if(uS / US_ROUNDTRIP_CM > 25 || uS / US_ROUNDTRIP_CM == 0)
    {
      autoForward();
    }
    else if (uS / US_ROUNDTRIP_CM < 25)
    {
      autoRight();
      delay(650);
    }
  }
  // Manual Mode
  else if(!autopilot){
    // Manual mode command entry
    if(msg.toInt() == 0){  // toInt() returns 0 if string not an integer | only integer when adjusting speed
      // Set Direction
      if (msg == "l"){goLeft();}
      else if (msg == "r"){goRight();}
      else if (msg == "u"){goForward();}
      else if (msg == "d"){goBackward();}
      else if (msg == "s"){motorStop();}
      // Turn off/on turn signals
      else if (msg == "w"){
        isBlinkingL = !isBlinkingL;
        msg = "";
        }   
      else if (msg == "e"){
        isBlinkingR = !isBlinkingR;
        msg = "";
        }  
    }
    // Set Speed
    else { changeSpeed(msg.toInt());}
  }

  
}

void timedOutputs(){
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > blinkInterval){
    previousMillis = currentMillis;

    if(isBlinkingL && lState == LOW){
      lState = HIGH;
    }
    else if(isBlinkingL && lState == HIGH){
      lState = LOW;
    }
    else {lState = LOW;}

    if(isBlinkingR && rState == LOW){
      rState = HIGH;
    }
    else if(isBlinkingR && rState == HIGH){
      rState = LOW;
    }
    else {rState = LOW;}
    
    if(buzzerOn && buzzState == 1){
      buzzState = 0;
      noNewTone(buzzer);
    }
    else if(buzzerOn && buzzState == 0){
      buzzState = 1;
      NewTone(buzzer, 1500);
    }
    else if(!buzzerOn && !horn){
      noNewTone(buzzer);
    }
    else if(buzzerOn && !horn){
      noNewTone(buzzer);
    }
    digitalWrite(lBlinker, lState);
    digitalWrite(rBlinker, rState);
  }
}

void goLeft()
{
  digitalWrite(DIRB, CW);
  digitalWrite(DIRA, CW);
  analogWrite(PWMB, 0);
  analogWrite(PWMA, spd);
}

void goRight()
{
  digitalWrite(DIRB, CCW);
  digitalWrite(DIRA, CCW);
  analogWrite(PWMB, spd);
  analogWrite(PWMA, 0);
}

void goBackward()
{
  digitalWrite(DIRB, CCW);
  digitalWrite(DIRA, CW);
  analogWrite(PWMB, spd);
  analogWrite(PWMA, spd*.9);  //compensation for weaker motorB
  buzzerOn = true;
}

void goForward()
{
  digitalWrite(DIRB, CW);
  digitalWrite(DIRA, CCW);
  analogWrite(PWMB, spd);
  analogWrite(PWMA, spd*.8);  //compensation for weaker motorB
}

void motorStop()
{
  analogWrite(PWMB, 0);
  analogWrite(PWMA, 0);
  buzzerOn = false;  
}

void autoForward()
{
  digitalWrite(DIRB, CW);
  digitalWrite(DIRA, CCW);
  analogWrite(PWMB, 150);
  analogWrite(PWMA, 125);   //compensation for weaker motor
}

void autoRight()
{
  digitalWrite(DIRB, CCW);
  digitalWrite(DIRA, CCW);
  analogWrite(PWMB, 150);
  analogWrite(PWMA, 0);
}

void setupArdumoto()
{
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(DIRA, OUTPUT);
  pinMode(DIRB, OUTPUT);
  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
  digitalWrite(DIRA, LOW);
  digitalWrite(DIRB, LOW);
}

void changeSpeed(int i){
  spd = map(i, 1, 10, 150, 255);
}

void lcdDisplay(){
  lcd.init();
  lcd.backlight();
  lcd.leftToRight();
  lcd.setCursor(0,0);
  lcd.print("AUG  CALI.  2021");
  lcd.setCursor(0,1);
  lcd.print("     WALL-E     ");
}
