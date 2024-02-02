

// Include the required Wire library for I2C<br>#include 
#include<Wire.h>
int buttonInp = 7 ;
int x = 1;
bool lock = false ;
int potPin = A3 ;
int potVal = 0 ;
int buzzer = 8;
unsigned long previousMillis = 0;
int interval  ;
unsigned long starttime ;
int ChoiceOfTransmission ;
byte I2C_OnOff;
String code;
int buf ;
bool codesent = false;
int queryInterval = 2000;
int queryTime ;
String full_pin;
int y = 0;
int idscan = 0;
int currentscan ;
void setup() {
 randomSeed(analogRead(A0));
  // Start the I2C Bus as controller 
 Serial.begin(9600);
 Wire.begin(); 
 pinMode(buttonInp,INPUT);
 pinMode(buzzer,OUTPUT);
}
 
void loop() {
  //checks whether button has been pressed and the box is unlocked, so that it removes any issues caused by the button being pressed during a lock cycle.
  if ((digitalRead(buttonInp)== HIGH) && (lock == false)) {
    // reads the potentiometer value so that it can set the time for the correct duration
     potVal = analogRead(potPin);
     if (potVal  < 341 ){
      interval = 2000;
     }
     else {
       interval = 7000;
     }
    starttime = millis();
    //sends data to the agent arduino which has the servo and LED so that it can lock the box
    Wire.beginTransmission(9); 
    x = 1;
    Wire.write(x);
    Wire.endTransmission();
    //sends data to agent arduino with RFID so that it knows to check for scans
    Wire.beginTransmission(2);
    y = 1;
    Wire.write(y);
    Wire.endTransmission();
   // generates the random code and daves it as a variable code. 
   if (codesent == false) { 
    //Sends the code to the agent arduino with the LCD to display the code
     Wire.beginTransmission(1);
     code = "" ;
     for (int i = 0; i <=3; i++) {
       int randNumber = random(10);
       Wire.write(randNumber);
       code += String(randNumber);
     }
     Serial.println("code is: " + code);
     codesent = true;
     Wire.endTransmission(1);
     lock = true; //sets locked to true so that this code cannot run again while locked, to ensure the different codes arent sent to the agent and the duration time doesnt restart.
     Wire.endTransmission(9);
    }
   queryTime = millis(); // takes the current run time and sets it to querytime
  }
  if ((millis() - starttime > interval) && (lock == true )){ // checks whether the required duration has elapsed and the box is locked
    // sends data to agent arduinos to let them know time has elapsed so that they can unlock the box and stop checking for scans on the rfid sensor
    Wire.beginTransmission(9);
    x = 0;
    y = 0;
    Wire.write(x);
    Wire.endTransmission(9); 
    //Buzzer beeps to signal time has elapsed and box is unlocked
    tone(buzzer,500);
    delay(500);
    noTone(buzzer);
    delay(1000);
    lock = false ;
    codesent = false;
  }
  //requests data from the agent arduinos while the box is locked to check whether it needs to prematurely unlock
  else if ((millis() > queryTime + queryInterval) && (lock == true)){ 
    queryTime = millis();
    full_pin = "" ;
    // gets value of rfid scan , if it has been scanned will recieve 1 otherwise recives 0 when nothing has been scanned
    Wire.requestFrom(2,1);
    Serial.println("id scan is:" + String(idscan));
    idscan = Wire.read();
    Wire.requestFrom(9,4);
    //requests data from arduino with keypad
    Serial.println("sent a request for pin");
    // gets the pin that has been inputted into the keypad
    while (Wire.available()){
     int recievedCode = Wire.read();
     recievedCode = recievedCode - 48 ;    // recieves as ascii value therefore has to convert back to integer. 0 = 48 in ascii
     full_pin += String(recievedCode);
      }
    Serial.println("pin recieved: " + full_pin);
    Serial.println("pin expected: " + code);
    //checks whether recieved pin is equal to the code that was sent to lcd arduino / generated code and if card has been scanned
    if ((full_pin == code)&& (idscan == 1)) { 
       full_pin = "";
       code = "stop";
       Serial.println("pins matched, sending premature unlock");
       // sends data to agent arduinos to let them know that box needs to be unlocked
       Wire.beginTransmission(9);
       x = 0;
       y = 0;
       Wire.write(x);
       Wire.endTransmission(9); 
       //buzzer beeps to let user know box is unlocked
       tone(buzzer,500);
       delay(500);
       noTone(buzzer);
       delay(1000);
       lock = false ;
       codesent = false;
    }
  }
}
