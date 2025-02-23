/*
  IOT2050_MiniSensor_By_MoKasaei
  Based on ESP32-C3
  Hardware_Version = 1
  Firmware_Version = 1
  ESP32 board V3.0.7
  CPU Freq = 40MHz
  Flash Size = 8MB (64Mb)
  Partition Scheme = No OTA (2MB APP/2MB SPIFFS)
*/

/*****************************************************************************/
/*********************************  library  *********************************/
#include <Wire.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <driver/adc.h>
#include <vector>
/*********************************  library  *********************************/
/*****************************************************************************/
void ICACHE_RAM_ATTR button_pressed();

void TurnOffLEDs();
void changeStateOrangeLED();
void changeinitialCheck();
void setup_sensors();
void read_sensors();
void sendDataVoid();

void readSensorVoid();

double roundFloat(double value);

float predictNextValue(const std::vector<float>& data);

String DeviceType = "E-CARD";

boolean firstPayload = true;

int OrangeLED = 8;
int button = 1;

bool temperature_data_sent = false;
bool button_data_sent = false;

Ticker blinkerOrangeLED;
Ticker sendDataTicker;
Ticker readSensorTicker;

int percentage;

#define si7021Addr 0x40
float humidity = 0;
float temp = 0;

bool buttonPressed = false;

String buttonStatus = "NOT Pressed";

String payloadMAIN;

bool ledFade = false;
bool ledGoBriht = true;
int ledBrightness = 0;

int resetCounter = 0;
int buttonCounter = 0;
bool startNewButtonCounter = false;
unsigned long oldButtonPressTimer = -50;

bool LED_BLINK_ON = false;

DynamicJsonDocument doc(2048);

unsigned long Millis = 0;

String interval = "20";

std::vector<float> tempReadings;
std::vector<float> humidityReadings;
float predictedTempValue;
float predictedHumidityValue;
const int WINDOW_SIZE = 10;  // Number of previous readings used for prediction

/*****************************************************************************/
/**********************************  setup  **********************************/
void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);

  pinMode(OrangeLED, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  if (digitalRead(button) == LOW) {
    buttonPressed = true;
  }
  analogWrite(OrangeLED, 0);

   for(int i = 0; i < 2; i++) {
    analogWrite(OrangeLED, 50);
    delay(100);
    analogWrite(OrangeLED, 0);
    delay(100);
  }
  attachInterrupt(digitalPinToInterrupt(button), button_pressed, CHANGE);

  analogWrite(OrangeLED, 50);

  TurnOffLEDs();
  sendDataTicker.attach(interval.toInt(), sendDataVoid);

  setup_sensors();
  read_sensors();
  tempReadings.reserve(WINDOW_SIZE);
  humidityReadings.reserve(WINDOW_SIZE);
  for (int i = 0; i < WINDOW_SIZE; i++) {
    read_sensors();
  }

  readSensorTicker.attach(10, readSensorVoid);
}
/**********************************  setup  **********************************/
/*****************************************************************************/


/*****************************************************************************/
/***********************************  loop  **********************************/
void loop() {
  if (temperature_data_sent == false) {
    doc.clear();
    doc["data"]["Temperature"] = roundFloat(predictedTempValue);
    doc["data"]["Humidity"] = roundFloat(predictedHumidityValue);
    doc["data"]["Button"] = buttonStatus;
    payloadMAIN = "";
    serializeJson(doc, payloadMAIN);
    Serial.println(payloadMAIN);
    TurnOffLEDs();
    temperature_data_sent = true;
  }

  if (button_data_sent == false) {
    if (buttonPressed) factory_reset();
    doc.clear();
    doc["data"]["Temperature"] = roundFloat(predictedTempValue);
    doc["data"]["Humidity"] = roundFloat(predictedHumidityValue);
    doc["data"]["Button"] = buttonStatus;
    payloadMAIN = "";
    serializeJson(doc, payloadMAIN);
    TurnOffLEDs();
    Serial.println(payloadMAIN);
    buttonCounter = 0;
    buttonStatus = "NOT Pressed";
    button_data_sent = true;
    startNewButtonCounter = true;
  }
  if (button_data_sent == true) {
    buttonStatus = "NOT Pressed";
  }

  if (buttonPressed) factory_reset();
  if (buttonCounter > 0 && ((millis() - oldButtonPressTimer) > 1500) && button_data_sent == true && startNewButtonCounter == false) {
    button_data_sent = false;
    buttonStatus = "Pressed";
  }

  if (ledFade) {
    if (ledGoBriht) {
      ledBrightness++;
      if (ledBrightness >= 50) ledGoBriht = false;
    } else {
      ledBrightness--;
      if (ledBrightness <= 0) ledGoBriht = true;
    }
    analogWrite(OrangeLED, ledBrightness);
    delay(150);
  }
}
/***********************************  loop  **********************************/
/*****************************************************************************/
