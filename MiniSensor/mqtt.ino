const int mqttPort = 8883;
int _trymqtt = 0;
byte MQTTpubQos = 1;
boolean willRetain = true;

#define NB_TRYMQTT 4

/*****************************************************************************/
/*****************************  connect to mqtt  *****************************/
void Connect_MQTT() {
  _trymqtt = 0;

  TurnOffLEDs();

  if (flagymqtt == "2") {
    topic = readFile("/topic.txt");
    mqttUser = topic;   //MAC_Address.c_str();   //readFile("/mqttUser.txt");
    mqttPassword = readFile("/mqttPassword.txt");
  }
  else {
    topic = MAC_Address.c_str();
    mqttUser = MAC_Address.c_str();
    mqttPassword = "I_HAVE_NO_PASSWORD";
    if (debug) Serial.println("requesting to get new topic and mqttPassword from macaddressBASE64");
  }

  if (debug) {
    if (flagymqtt == "2") {
      Serial.println("topic, mqttUser, and mqttPassword are set.");
    }
    else {
      Serial.println("device is using default parameters for topic, mqttUser, and mqttPassword.");
    }
    Serial.print("topic:");
    Serial.println(topic);
    Serial.print("mqttUser:");
    Serial.println(mqttUser);
    Serial.print("mqttPassword:");
    Serial.println(mqttPassword);
  }

  TurnOffLEDs();
  analogWrite(OrangeLED, 254);


  const char* fingerprint = "d9:07:02:01:16:12:03:45:65:be:ff:b5:fa:e0:ff:13:37:15:f2:41";  //New in 2020-01-19
  const static char* root_ca PROGMEM =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIEKzCCAxOgAwIBAgIJAN7hKmGyTq5qMA0GCSqGSIb3DQEBCwUAMIGrMQswCQYD\n"
    "VQQGEwJDQTESMBAGA1UECAwJVmFuY291dmVyMRgwFgYDVQQHDA9Ob3J0aCBWYW5j\n"
    "b3V2ZXIxGTAXBgNVBAoMEEludGVybmV0QW55d2hlcmUxJTAjBgNVBAMMHGlhYnJv\n"
    "a2VyLmludGVybmV0YW55d2hlcmUuaW8xLDAqBgkqhkiG9w0BCQEWHXNtYXJ0aG9t\n"
    "ZUBpbnRlcm5ldGFueXdoZXJlLmlvMB4XDTIwMDExOTIwNTUwMFoXDTQwMDExNDIw\n"
    "NTUwMFowgasxCzAJBgNVBAYTAkNBMRIwEAYDVQQIDAlWYW5jb3V2ZXIxGDAWBgNV\n"
    "BAcMD05vcnRoIFZhbmNvdXZlcjEZMBcGA1UECgwQSW50ZXJuZXRBbnl3aGVyZTEl\n"
    "MCMGA1UEAwwcaWFicm9rZXIuaW50ZXJuZXRhbnl3aGVyZS5pbzEsMCoGCSqGSIb3\n"
    "DQEJARYdc21hcnRob21lQGludGVybmV0YW55d2hlcmUuaW8wggEiMA0GCSqGSIb3\n"
    "DQEBAQUAA4IBDwAwggEKAoIBAQC5xfaVGcpx2NHunOUZ/zfAQ9zTMWGtkEL2QMsA\n"
    "XJhm4L0TGclgMDo+125+FPM5dX6OauhWoI7lgB92AJnwxOigSHIX2sVy+6QhqYsF\n"
    "zd16ZpniwKI3p/CNbNBgnp1hZb56G4EOje4g50IUFmmepW3i52Ysp+Xj4eRcz10r\n"
    "fPfpUidqez1AH9JPtPxMlrRfQ7ByD8Cejgno8NcqX7TD6SLVxIt+kQUGVRHEjHg5\n"
    "BIEIgORBgN40SiDh2KIx87ZSa60RrYJn8M7u8oIVGPr+CC7lMOxiqNHQxFAFQRBF\n"
    "gHgZT88T4wJqwJG4wvFgKqDeHWM0kS45vfRKWFbJHiKyKilRAgMBAAGjUDBOMB0G\n"
    "A1UdDgQWBBQPbtIGZzR+pG4XaToW88TRBYNQUjAfBgNVHSMEGDAWgBQPbtIGZzR+\n"
    "pG4XaToW88TRBYNQUjAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQBZ\n"
    "Dj6hCUkAeS6cXj9w+YnKiezSmMotvT0XXyNMd7UTlAYkEq1/3QbNm9T1dbJI3Vtp\n"
    "GRbhkmfvGJKSEw6AyYVsMagPqxy65wO42rZ5EaQFiSAa96fOcxk5GJ+IOAgIuG51\n"
    "VyZijsDFkQWnBjkH1/rK4Kp3LHIm08bUO0ueT+caECXotY7ICT6oLjCYORC3wslx\n"
    "Srl4DGnV5K89d7Jbx1c9C1aubR9LZuAvhEwwIrAq32hAkoHcRPquwlW+huA3DxgE\n"
    "YOBBNZ5gp+3AqvBPLCJUt1WBcAjEBStiqvW5KjQdEccvlqAN9s8OtJSBic7MHoar\n"
    "ZkmueQsfkXAlYhEG0hBM\n"
    "-----END CERTIFICATE-----\n";
  //espClient.allowSelfSignedCerts();
  //espClient.setFingerprint(fingerprint);
  espClient.setInsecure();
  //espClient.setCACert(root_ca);
  espClient.stop();

  if (bp == "") bp = String(mqttPort);
  client.setServer(bh, bp.toInt());
  client.setCallback(callback);
  if (debug) {
    Serial.println("******************");
    Serial.println(topic);
    Serial.println(mqttUser);
    Serial.println(mqttPassword);
    Serial.println("******************");
  }
  if (!client.connected()) {
    if (buttonPressed) {
      factory_reset();
    }
    if (WiFi.status() == !WL_CONNECTED) {
      //ConnectTowifi();
      mesh.update();
    }
    if (debug) {
      Serial.print("Attempting MQTT connection...");
    }
    if (_trymqtt == 0) {
      mqttWatchdogTimer = millis();
    }
    //mqttWatchdog.attach(30, espRestart);
    if (client.connect((char*)topic.c_str(), (char*)mqttUser.c_str(), (char*)mqttPassword.c_str())) {
      TurnOffLEDs();
      mqttconnection = 1;
      if (debug) Serial.println("connected");
      client.subscribe((char*)topic.c_str());
      //mqttWatchdog.detach();

      _trymqtt = 0;
    }
    else {
      mqttconnection = 0;
      TurnOffLEDs();
      blinkerOrangeLED.attach(4, changeStateOrangeLED);
      if (debug) {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(100);
      }
      if (millis() - mqttWatchdogTimer > 30000 && _trymqtt > 1) {
        while (!writeFile("/node.txt", "node")) {
          delay(100);
        }
        if (debug) {
          Serial.println("switching to node: can't find Wi-Fi router");
          delay(1000);
        }
        espRestart();
      }
      
      _trymqtt++;

      if (_trymqtt >= NB_TRYMQTT) {
        if (client.state() == 5) {  //This is for remove device from dashboard.
          resetMqtt();
        }
        TurnOffLEDs();
      }
    }
  }
}
/*****************************  connect to mqtt  *****************************/
/*****************************************************************************/


/*****************************************************************************/
/********************************  call back  ********************************/
void callback(char* topic, byte* payload, unsigned int length) {
  if (debug) Serial.print(">> Message arrived in topic: ");
  if (debug) Serial.println(topic);
  //if (debug) Serial.print("payload: ");
  //if (debug) Serial.println(String(payload));

  deserializeJson(doc, payload);

  if (doc.containsKey("YOUR_DEVICEID")) {
    String NEWtopic_local = doc["YOUR_DEVICEID"];
    String NEWpassword_local = doc["YOUR_PASSWORD"];
    String NEWtopic;
    String NEWpassword;

    NEWtopic = NEWtopic_local;
    NEWpassword = NEWpassword_local;

    if (debug) {
      Serial.print("YOUR_topic: ");
      Serial.println(NEWtopic);
      Serial.print("YOUR_PASSWORD: ");
      Serial.println(NEWpassword);
      delay(1000);
    }

    String flagymqtt = "2";

    writeFile("/flagMQTT.txt", flagymqtt);
    writeFile("/topic.txt", NEWtopic);
    writeFile("/mqttPassword.txt", NEWpassword);

    BLEDevice::deinit(true);
    WiFi.mode(WIFI_OFF);
    delay(1000);

    ESP.restart();

    return;
  }

  else if (doc.containsKey("RESET")) {
    if (doc["YOUR_PASSWORD"] == mqttPassword)
      resetMqtt();

    return;
  }

  //check command of server
  else if (doc.containsKey("command")) {
    const char* command = doc["command"];
    // onCommand(command, doc);
  }

  else if (doc.containsKey("FIRM_UPDATE")) {
    const char* local_test = doc["FIRM_UPDATE"];
    String FIRM_PASS = String(mqttPassword).substring(0, 10);

    if (doc["FIRM_PASSWORD_10"] == FIRM_PASS) {
      String ota_version = doc["FIRM_UPDATE"];
      if (ota_version.toInt() > firmware_version) {
        if (debug) Serial.println("START UPDATE");
        int bytesWritten;
        String FIRM_URL = doc["FIRM_URL"];
        String FINGERPRINT = doc["FINGERPRINT"];

        writeFile("/FIRM_URL.txt", FIRM_URL);
        writeFile("/FINGERPRINT.txt", FINGERPRINT);
        writeFile("/OTA.txt", "true");

        delay(200);

        BLEDevice::deinit(true);
        WiFi.mode(WIFI_OFF);
        delay(1000);

        ESP.restart();
      }
    }
  }
}
/********************************  call back  ********************************/
/*****************************************************************************/


/*****************************************************************************/
/********************************  reset mqtt  *******************************/
void resetMqtt() {
  int _size = 512;
  int j;
  for (j = 0; j < _size; j++) {
    EEPROM.write(j, '\0');
    Serial.print("E");
  }
  EEPROM.commit();

  removeFile("/SSID.txt");
  removeFile("/PASSWORD.txt");
  removeFile("/meshChannel.txt");
  removeFile("/node.txt");
  removeFile("/flagMQTT.txt");
  removeFile("/topic.txt");
  removeFile("/mqttUser.txt");
  removeFile("/mqttPassword.txt");
  removeFile("/brokerHost.txt");
  removeFile("/brokerPort.txt");
  removeFile("/FIRM_URL.txt");
  removeFile("/FINGERPRINT.txt");

  Serial.println("FLASH DONE");
  TurnOffLEDs();

  WiFi.disconnect();
  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11N);
  esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_11N);
  WiFi.setTxPower(WIFI_POWER_2dBm);
  WiFi.setSleep(WIFI_PS_MIN_MODEM);

  analogWrite(OrangeLED, 254);
  while (digitalRead(button) == LOW) {
    analogWrite(OrangeLED, 254);
    delay(10);
  }
  for (int i = 0; i < 10; i++) {
    if (LED_BLINK_ON == true) {
      analogWrite(OrangeLED, 0);
      LED_BLINK_ON = false;
    } else {
      analogWrite(OrangeLED, 254);
      LED_BLINK_ON = true;
    }
    delay(200);
  }
  delay(1000);
  ESP.restart();
}
/********************************  reset mqtt  *******************************/
/*****************************************************************************/
