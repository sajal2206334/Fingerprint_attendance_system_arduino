//#include <Wire.h>

//#include <RTClib.h>

#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal.h>



#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(2, 3);

#else
#define mySerial Serial1

#endif
#define outputSignal 5


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

LiquidCrystal lcd(8,9,10,11,12,13);

//RTC_DS3231 rtc;

//DateTime dt = rtc.now();


uint8_t id;

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  lcd.begin(16, 2);


  /*if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }*/

  pinMode(outputSignal,OUTPUT);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }


}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void sendSignalRegister(bool success) {
  digitalWrite(outputSignal,success ? HIGH : LOW);
}

void loop()                     // run over and over again
{
  Serial.println("Group 10 present");
  lcd.setCursor(0,0);
  lcd.print("Group 10 present");
  delay(2000);
  lcd.clear();

  Serial.println("Enter 0 to attendance, 1 to enroll : ");
  lcd.setCursor(0,0);
  lcd.print("Enter 0A or 1E");
  delay(1000);
  while (!Serial.available());
  int userInput = Serial.parseInt();

  
  if (userInput == 1) {

    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Registration");
    lcd.setCursor(4,1);
    lcd.print("Process");
    delay(2000);
    lcd.clear();
    Serial.println("Ready to enroll a fingerprint!");
    lcd.setCursor(0, 0);
    lcd.print("Ready to enroll");
    delay(2000);
    Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
    lcd.clear();
    lcd.print("Type ID in I");
    id = readnumber();
    if (id == 0) {// ID #0 not allowed, try again!
      return;
    }
    Serial.print("Enrolling ID #");
    lcd.setCursor(0,0);
    lcd.print("Enrolling ID ");
    Serial.println(id);
    lcd.setCursor(0,1);
    lcd.print(id);
    getFingerprintEnroll();
    //dateTime();
  }else if(userInput == 0) {

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Mark Attendance");
    delay(500);
    lcd.clear();
    Serial.println("Ready to register a fingerprint!");
    lcd.setCursor(0,0);
    lcd.print("Ready to check");
    delay(1000);
    Serial.println("Place your finger on the sensor...");
    lcd.clear();
    lcd.print("Place finger on I");
    delay(1000);

    int fingerprintResult = getFingerprintID();
    if (fingerprintResult == FINGERPRINT_NOFINGER) {
      Serial.println("No finger detected");
      lcd.setCursor(0, 0);
      lcd.print("No finger detected");
      //dateTime();
    } else {
      // Handle the attendance for the detected finger
      // ...
      //lcd.setCursor(0, 0);
      //lcd.print("finger detected");
      delay(4000);
      //dateTime();
    }
  }

  else {
    Serial.println("Invalid input Enter only 0 to register or 1 to enroll");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Invalid Input");
    lcd.setCursor(0,1);
    lcd.print("Use only 0 or 1");
  }

  delay(1000);
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      lcd.setCursor(0, 0);
      lcd.print("Image Taken");
      delay(50);
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      lcd.setCursor(0,0);
      lcd.print("Image converted");
      delay(50);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  lcd.setCursor(0,1);
  lcd.print("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.setCursor(0,0);
  lcd.print("Place same");
  lcd.setCursor(0,1);
  lcd.print("Finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      lcd.setCursor(0, 0);
      lcd.print("Image Taken");
      delay(50);
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      lcd.setCursor(0,0);
      lcd.print("Image converted");
      delay(50);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
    delay(800);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Prints matched!");
    delay(50);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("did not match");
    delay(50);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    lcd.setCursor(0,0);
    lcd.print("Stored! At");
    lcd.clear();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}

uint8_t getFingerprintID() {
  int p = -1;
  Serial.print("Waiting for valid finger to check...");
  delay(4000);
  while (p != FINGERPRINT_OK){
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        lcd.setCursor(0,0);
        lcd.print("Image taken");
        delay(500);
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println("No finger detected");
        lcd.setCursor(0, 0);
        lcd.print("No finger detected");
        return p;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      lcd.setCursor(0,0);
      lcd.print("Image converted");
      delay(50);
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  lcd.setCursor(0,1);
  lcd.print("Remove finger");

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    lcd.setCursor(0,0);
    lcd.print("Print matches");delay(500);
    sendSignalRegister(true);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    lcd.setCursor(0,0);
    lcd.print("Print not match");
    sendSignalRegister(false);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

/*void dateTime() {
  DateTime dt = rtc.now();

  Serial.print(dt.year(), DEC);
  Serial.print('/');
  Serial.print(dt.month(), DEC);
  Serial.print('/');
  Serial.print(dt.day(), DEC);
  Serial.print(" ");
  Serial.print(dt.hour(), DEC);
  Serial.print(':');
  Serial.print(dt.minute(), DEC);
  Serial.print(':');
  Serial.print(dt.second(), DEC);
  Serial.println();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Date");
  lcd.print(dt.year(),DEC);
  lcd.print("/");
  lcd.print(dt.month(),DEC);
  lcd.print("/");
  lcd.print(dt.day(),DEC);
  lcd.setCursor(0,1);
  lcd.print("Date");
  lcd.print(dt.hour(),DEC);
  lcd.print("/");
  lcd.print(dt.minute(),DEC);
  lcd.print("/");
  lcd.print(dt.second(),DEC);
  delay(1000);
}*/


