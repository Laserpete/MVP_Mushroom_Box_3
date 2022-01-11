#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <SHTSensor.h>

#include "header.h"

#define BUILTIN_LED_PIN 13
#define FOGGER_PIN 12
#define FOGGER_FAN_PIN 11
#define VENTILATOR_PIN 10

#define FOGGER_FAN_PWM_VAL 100
#define VENTILATOR_FAN_PWM_VAL 100
#define RUN_INTERVAL 1
#define VENT_INTERVAL 900
#define VENT_DURATION 30
#define HUMIDITY_HIGH_THRESHOLD 90
#define HUMIDITY_LOW_THRESHOLD 70

SHTSensor sht(SHTSensor::SHT3X);

float temperature, humidity = 0;
uint32_t lastRunMillis, lastVentMillis = 0;
bool runBool, ventBool = false;

void setupPins() {
  pinMode(FOGGER_PIN, OUTPUT);
  pinMode(FOGGER_FAN_PIN, OUTPUT);
  pinMode(VENTILATOR_PIN, OUTPUT);
}

void setup() {
  TCCR2B = TCCR2B & B11111000 | B00000001;  // Set D3 & D11 PWM to 60kHz
  TCCR1B = TCCR1B & B11111000 | B00000001;  // Set D9 & D10 PWM to 60kHz
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
    // analogWrite(VENTILATOR_PIN, VENTILATOR_FAN_PWM_VAL);
    digitalWrite(BUILTIN_LED_PIN, HIGH);
    Serial.println("Humidifier on");
  } else {
    digitalWrite(FOGGER_PIN, LOW);
    digitalWrite(FOGGER_FAN_PIN, LOW);
    // digitalWrite(VENTILATOR_PIN, LOW);
    digitalWrite(BUILTIN_LED_PIN, LOW);
    Serial.println("Humidifier off");
  }
}

void vent(bool go) {
  if (go) {
    analogWrite(VENTILATOR_PIN, VENTILATOR_FAN_PWM_VAL);
    Serial.println("Venting...");
  } else {
    digitalWrite(VENTILATOR_PIN, LOW);
    Serial.println("Stopped venting");
  }
}

void loop() {
  if (millis() >= lastVentMillis + ((long)VENT_INTERVAL * 1000)) {
    vent(1);
  }
  if (millis() >= lastVentMillis + (((long)VENT_INTERVAL * 1000) +
                                    ((long)VENT_DURATION * 1000))) {
    vent(0);
    lastVentMillis = millis();
  }

  if (millis() >= lastRunMillis + (RUN_INTERVAL * 1000)) {
    lastRunMillis = millis();
    runBool = true;
  }

  if (runBool == true) {
    if (sht.readSample()) {
      temperature = sht.getTemperature();
      humidity = sht.getHumidity();
      Serial.print(F("Temperature °C: \t\t"));
      Serial.println((int)temperature);
      Serial.print(F("Humidity %: \t\t\t"));
      Serial.println((int)humidity);
    } else {
      Serial.println("Error in SHT31 readSample()");
    }

    displayTempHum(temperature, humidity);

    if (humidity >= HUMIDITY_HIGH_THRESHOLD) {
      humidify(0);
      vent(1);
    } else if (humidity <= HUMIDITY_LOW_THRESHOLD) {
      humidify(1);
      vent(0);
    } else if (humidity >= HUMIDITY_LOW_THRESHOLD &&
               humidity <= HUMIDITY_HIGH_THRESHOLD) {
      humidify(0);
      vent(0);
    } else {
      ;
    }
    Serial.println();
    runBool = false;
  } else {
    ;
  }
}