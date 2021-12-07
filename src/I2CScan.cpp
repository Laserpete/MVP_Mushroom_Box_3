#ifndef _I2CScan_
#define _I2CScan_

#include <Arduino.h>
#include <Wire.h>

#include "header.h"

#define Low_Address 8
#define High_Address 119

void I2CScan() {
  Serial.println(F("--- Scan started ---"));
  int Devices_Count = 0;
  for (byte address = Low_Address; address < High_Address; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print(F("I2C device found at address 0x"));
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(F("  !"));

      Devices_Count++;
    } else if (error == 4) {
      Serial.print(F("Unknow error at address 0x"));
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (Devices_Count == 0) Serial.println(F("No I2C devices found"));
  Serial.println(F("--- Scan finished ---\n"));
}

#endif