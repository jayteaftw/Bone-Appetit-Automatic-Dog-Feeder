#define CUSTOM_SETTINGS
#define INCLUDE_TERMINAL_MODULE
#include <Dabble.h>
#include <Servo.h>

Servo myservo;

void setup() {
Serial.begin(250000);
Dabble.begin(9600);     //Enter baudrate of your bluetooth.Connect bluetooth on Bluetooth port present on evive.
 myservo.attach(9);  
 myservo.write(0);
}

void loop() {
  Dabble.processInput();


  
  if(Terminal.available())
  {
    
    Serial.println(Terminal.readString());
    if(Terminal.compareString("Feed"))
    {
      myservo.write(180);
      delay(5000);
      myservo.write(0);
    }
  }

  

  myservo.write(0);
  delay(500);
}
