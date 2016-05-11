#include "IRremote/IRremote.h"

IRsend irsend;

void setup()
{
  Serial.begin(9600);
}

void loop() {
  irsend.sendSAMSUNG(0xa90, 12);
  delay(500);
} 

