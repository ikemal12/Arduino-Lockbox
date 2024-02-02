//libraries
#include <Wire.h>
#include <Servo.h>
#include <Keypad.h>

//variables
Servo myservo;

int pos = 0;
int LEDG = 13;
int LEDR = 8;
int lockState = 0;
int unlockPos = 0;
int lockPos = 90;

String currentPin = "0000";
String nextPin = "";
String full_code = "";

const byte ROWS = 4; 
const byte COLS = 3; 
  //var to do with keypad 
char customKey;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {9, 7, 6, 5}; 
byte colPins[COLS] = {4, 3, 2}; 
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() {
  Serial.begin(9600);
  myservo.attach(10); //servo on pin 10
  pinMode (LEDG, OUTPUT);
  pinMode (LEDR, OUTPUT);
  // Start the I2C Bus as an agent on address 9
  Wire.begin(9); 
  //request and recieve functions of I2C
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}
void receiveEvent(int bytes) { //set new lockstate when on receive event
  Serial.println("\n received event");
  lockState = Wire.read();
  Serial.println("\n lockstate is: " + String(lockState));
}
void requestEvent (){ //send current pin inputted when on request event
  Serial.println("Requested " + currentPin);
  for (int i = 0; i<=3; i++){
    int send = currentPin[i]; //send each character as integers one by one
    Wire.write(send);
  }
}
void loop() {
  //If lock in unlocked state, led green is on, led red is off, lock is unlocked
  if (lockState == 0) {
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDR,LOW);
    myservo.write(unlockPos);
  }
  //If lock in locked state, led red is on, led green is off, lock is locked
  if (lockState == 1) {
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, LOW);
    myservo.write(lockPos);
    //will read keypad inputs when in the locked state
    customKey = customKeypad.getKey();
    if (customKey){ //if key input
      Serial.println("\n key pressed: ");
      Serial.println(customKey);
      nextPin = nextPin + String(customKey); //append to next pin
      if (nextPin.length() == 4){ //when next pins is len 4, update pin for request event
        currentPin = nextPin;
        nextPin = ""; //clear nextpin
        Serial.println("Pin updated: " + currentPin);
      }
    } 
  }
}