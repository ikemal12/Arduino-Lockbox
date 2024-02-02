#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
int locked = 0;
int scanned = 0;


void setup() {
  Serial.begin(9600);   // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(4);       // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  Wire.begin(2);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  scanned = 0;
  if (locked == 1) { //only run if the box is locked
      if ( ! mfrc522.PICC_IsNewCardPresent()) { // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.

        return;
      }
    
      // Select one of the cards
      if ( ! mfrc522.PICC_ReadCardSerial()) {
        return;
      }
    
      String content= "";
      byte letter;
      for (byte i = 0; i < mfrc522.uid.size; i++) // for every byte
      {
         content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ")); // adds spaces every 2 hex character so that the UID is in the correct format
         content.concat(String(mfrc522.uid.uidByte[i], HEX)); // turns bytes into HEX
      }
      if (content.substring(1) == "7a 5c ba 42")
      {
    
        Serial.println(" Authorized access");
        Serial.println();
        scanned = 1;
        delay(5000);
      }
     
     else   {
        Serial.println(" Access denied");
        Serial.println();
        delay(3000);
      }
    }
  }

void receiveEvent(int bytes) {
  locked = Wire.read();
  Serial.println("Recieved " + String(locked));
  }

void requestEvent() {
  Wire.write(scanned);
  Serial.println("Request Recieved " + String(scanned));
  }
