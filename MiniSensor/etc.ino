/*****************************************************************************/
/*******************************  Read Sensor  *******************************/
/*void read_sensor() {
  Wire.begin(6, 7);
  Wire.beginTransmission(si7021Addr);
  Wire.endTransmission();
  unsigned int data[2];

  Wire.beginTransmission(si7021Addr);
  uint8_t wireBuffer1[2] = { 0xE6, 0x00 };
  Wire.write(wireBuffer1, 2);
  Wire.endTransmission();
  delay(50);

  Wire.beginTransmission(si7021Addr);
  uint8_t wireBuffer2[2] = { 0x51, 0x00 };
  Wire.write(wireBuffer2, 2);
  Wire.endTransmission();
  delay(50);

  Wire.beginTransmission(si7021Addr);
  Wire.write(0x3A);
  Wire.endTransmission();
  delay(50);

  Wire.beginTransmission(si7021Addr);
  Wire.write(0xF5);
  Wire.endTransmission();
  delay(50);

  Wire.requestFrom(si7021Addr, 2);
  if (Wire.available() == 2) {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }
  humidity = ((data[0] * 256.0) + data[1]);
  humidity = ((125 * humidity) / 65536.0) - 6;

  Wire.beginTransmission(si7021Addr);
  Wire.write(0xF3);
  Wire.endTransmission();
  delay(50);

  Wire.requestFrom(si7021Addr, 2);

  if (Wire.available() == 2) {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }
  temp = ((data[0] * 256.0) + data[1]);
  temp = ((175.72 * temp) / 65536.0) - 46.85;


  if (debug) {
    Vo = 0;
    for (int i = 0; i < 30; i++) {
      Vo += analogRead(Thermistor);
    }
    Vo /= 30;
    R2 = R1 * (4095.0 / (float)Vo - 1.0);
    logR2 = log(R2);
    T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
    Tc = T - 273.15;
  }
  }*/


/*******************************  Read Sensor  *******************************/
/*****************************************************************************/


/*****************************************************************************/
/***************************  Door Status Changed  ***************************/
void ICACHE_RAM_ATTR door_status_changed() {
  if (digitalRead(reedSwitch) == HIGH) {
    doorStatus = "Open";
  } else {
    doorStatus = "Close";
  }
  door_data_sent = false;
}
/***************************  Door Status Changed  ***************************/
/*****************************************************************************/


/*****************************************************************************/
/***************************  PIR Status Changed  ****************************/
void ICACHE_RAM_ATTR pir_status_changed() {
  if (digitalRead(PIR) == HIGH) {
    movementStatus = "Detected";
  } else {
    movementStatus = "Scanning...";
  }
  movement_data_sent = false;
}
/***************************  PIR Status Changed  ****************************/
/*****************************************************************************/


/*****************************************************************************/
/******************************  Button Pressed  *****************************/
void ICACHE_RAM_ATTR button_pressed() {
  if (digitalRead(button) == LOW) {
    if (startNewButtonCounter == true) {
      startNewButtonCounter = false;
      buttonCounter = 0;
      buttonPressed = true;
    }
    if (millis() - oldButtonPressTimer > 100) {
      buttonCounter++;
    }
  }
  else {
    oldButtonPressTimer = millis();
  }
}
/******************************  Button Pressed  *****************************/
/*****************************************************************************/


/*****************************************************************************/
/*******************************  Factory Reset  *****************************/
void factory_reset() {
  analogWrite(OrangeLED, 254);
  resetCounter = 0;
  while (digitalRead(button) == LOW) {
    if (LED_BLINK_ON == true) {
      analogWrite(OrangeLED, 0);
      LED_BLINK_ON = false;
    }
    else {
      analogWrite(OrangeLED, 254);
      LED_BLINK_ON = true;
    }
    resetCounter++;
    if (resetCounter >= 200) {
      analogWrite(OrangeLED, 254);
      delay(1500);
      resetMqtt();
    }
    delay(50);
  }
  analogWrite(OrangeLED, 0);
  /*if (buttonCounter <= 1) {
    buttonStatus = "Pressed";
    } else if (buttonCounter == 2) {
    buttonStatus = "Double";
    } else if (buttonCounter >= 3) {
    buttonStatus = "Triple";
    }*/
  buttonPressed = false;
}
/*******************************  Factory Reset  *****************************/
/*****************************************************************************/


/*****************************************************************************/
/*******************************  Turn Off LED  ******************************/
void TurnOffLEDs() {
  blinkerOrangeLED.detach();
  analogWrite(OrangeLED, 0);
  ledFade = true;
}
/*******************************  Turn Off LED  ******************************/
/*****************************************************************************/


/*****************************************************************************/
/********************************  toggle LED  *******************************/
void changeStateOrangeLED() {
  ledFade = false;
  if (LED_BLINK_ON == true) {
    analogWrite(OrangeLED, 0);
    LED_BLINK_ON = false;
  } else {
    analogWrite(OrangeLED, 254);
    LED_BLINK_ON = true;
  }
}
/********************************  toggle LED  *******************************/
/*****************************************************************************/


/*****************************************************************************/
/********************************  check wifi  *******************************/
void changeinitialCheck() {
  if (WiFi.status() != WL_CONNECTED && SSID != "") {
    //initial = false;
    ESP.restart();
  }
}
/********************************  check wifi  *******************************/
/*****************************************************************************/


/*****************************************************************************/
/*****************************  10 Seconds flag  *****************************/
void sendDataVoid() {
  temperature_data_sent = false;
}
/*****************************  10 Seconds flag  *****************************/
/*****************************************************************************/

/*****************************************************************************/
/*****************************  10 Seconds flag  *****************************/
void readSensorVoid() {
  read_sensor();
}
/*****************************  10 Seconds flag  *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************  2 Minutes flag  *****************************/
void meshDataVoid() {
  mesh_data_sent = false;
}
/******************************  2 Minutes flag  *****************************/
/*****************************************************************************/


/*****************************************************************************/
/******************************  MQTT Watchdog  ******************************/
void espRestart() {
  ESP.restart();
}
/******************************  MQTT Watchdog  ******************************/
/*****************************************************************************/


/*****************************************************************************/
/****************************** float rounding  ******************************/
double roundFloat(double value) {
  return (int)(value * 10 + 0.5) / 10.0;
}
/****************************** float rounding  ******************************/
/*****************************************************************************/


/*****************************************************************************/
/********************************  Scan Mesh  ********************************/
String scanprocessor(const String& var)
{
  if (var == "SCAN")
    return mesh.subConnectionJson(false) ;
  return String();
}
/********************************  Scan Mesh  ********************************/
/*****************************************************************************/


/*****************************************************************************/
/******************************* get local IP  *******************************/
IPAddress getlocalIP()
{
  return IPAddress(mesh.getStationIP());
}
/******************************* get local IP  *******************************/
/*****************************************************************************/
