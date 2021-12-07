#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <SHTSensor.h>

#include "header.h"

#define BUILTIN_LED_PIN 13
#define FOGGER_PIN 12
#define FOGGER_FAN_PIN 11
#define VENTILATOR_PIN 10

#define FOGGER_FAN_PWM_VAL 50
#define VENTILATOR_FAN_PWM_VAL 50
#define RUN_INTERVAL 1
#define HUMIDITY_HIGH_THRESHOLD 99
#define HUMIDITY_LOW_THRESHOLD 90

SHTSensor sht(SHTSensor::SHT3X);

float temperature, humidity = 0;
uint32_t lastRunMillis = 0;
bool runBool = false;

void setupPins() {
  pinMode(FOGGER_PIN, OUTPUT);
  pinMode(FOGGER_FAN_PIN, OUTPUT);
  pinMode(VENTILATOR_PIN, OUTPUT);
}

void setup() {
  TCCR2B = TCCR2B & B11111000 | B00000111;  // Set D3 & D11 PWM to 30.64 Hz
  TCCR1B = TCCR1B & B11111000 | B00000101;  // Set D9 & D10 PWM to 30.64 Hz
  Serial.begin(115200);
  while (!Serial)
    ;
  setupPins();
  setupLCD();
  I2CScan();
  if (sht.init()) {
    Serial.print("init(): success\n");
  } else {
    Serial.print("init(): failed\n");
  }
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);
}

void humidify(bool go) {
  if (go) {
    digitalWrite(FOGGER_PIN, HIGH);
    analogWrite(FOGGER_FAN_PIN, FOGGER_FAN_PWM_VAL);
    analogWrite(VENTILATOR_PIN, VENTILATOR_FAN_PWM_VAL);
    digitalWrite(BUILTIN_LED_PIN, HIGH);
  } else {
    digitalWrite(FOGGER_PIN, LOW);
    digitalWrite(FOGGER_FAN_PIN, LOW);
    digitalWrite(VENTILATOR_PIN, LOW);
    digitalWrite(BUILTIN_LED_PIN, LOW);
  }
}

void loop() {
  if (millis() >= lastRunMillis + (RUN_INTERVAL * 1000)) {
    lastRunMillis = millis();
    runBool = true;
  }

  if (runBool == true) {
    if (sht.readSample()) {
      temperature = sht.getTemperature();
      humidity = sht.getHumidity();
      Serial.print(F("Temperature °C: \t\t"));
      Serial.println(temperature);
      Serial.print(F("Humidity %: \t\t\t"));
      Serial.println(humidity);
    } else {
      Serial.println("Error in SHT31 readSample()");
    }

    displayTempHum(temperature, humidity);

    if (humidity >= HUMIDITY_HIGH_THRESHOLD) {
      Serial.println(F("Humidity reading too high, sensor heater on."));
      // htu21d.setHeater(HTU21D_ON);
      humidify(0);
    } else if (humidity <= HUMIDITY_LOW_THRESHOLD) {
      humidify(1);
      // htu21d.setHeater(HTU21D_OFF);

      Serial.println(F("Humidifier on"));

      Serial.println(F("Humidity reading acceptable, sensor heater off."));
    } else if (humidity >= HUMIDITY_LOW_THRESHOLD &&
               humidity <= HUMIDITY_HIGH_THRESHOLD) {
      humidify(0);
      //  htu21d.setHeater(HTU21D_OFF);

      Serial.println(F("Humidifier off"));
    } else {
      ;
    }
    Serial.println();
    runBool = false;
  } else {
    ;
  }
}