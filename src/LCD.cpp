#ifndef _LCD_
#define _LCD_
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "header.h"

#define LCD_IIC_ADDRESS 0x27

unsigned long lastDisplayToggle = 0;
bool displayToggle = false;

LiquidCrystal_I2C lcd(LCD_IIC_ADDRESS, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setupLCD() {
  lcd.begin(16, 2);
  lcd.backlight();

  lcd.home();
  lcd.print("Hello Mushrooms");
}

void displayTempHum(float temp, float hum) {
  lcd.home();
  lcd.clear();
  lcd.print("Temp = ");
  lcd.print((int)temp);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Humd = ");
  lcd.print((int)hum);
  lcd.print("%");
}

#endif
