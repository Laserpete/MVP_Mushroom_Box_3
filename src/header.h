#ifndef _HEADER_H_
#define _HEADER_H_

#include <Arduino.h>

void I2CScan();
void setupLCD();
void displayTempHum(float, float);

#endif