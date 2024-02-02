#include <LiquidCrystal.h>
#include <Wire.h>

// declare the digital pins that the LCD is connected to
#define LCD_Backlight 10 
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  lcd.begin(16, 2); // sets up dimensions of LCD screen
  pinMode(LCD_Backlight, OUTPUT); // initialise the pin used to control the backlight of the LCD
  analogWrite(LCD_Backlight, 128); // set the brightness of the backlight

  // get data from the controller arduino
  Wire.begin(1);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}

void loop() {
}

void receiveEvent(int bytes) {
  String full_code = ""; // initialise a variable to store the code
  lcd.setCursor(6,1); // center the code on the screen

  // read each integer received and store it in one variable
  while (Wire.available()) {
    int code = Wire.read();
    full_code += String(code);
    Serial.print("Code received"); // used for debugging
  }
  
  lcd.print(full_code); // display the code on the LCD
  Serial.print(full_code);
}
