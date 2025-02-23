BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
bool firstTimeScanSSID = true;
String rxBuffer;
String bluetoothCommand, bluetoothData;

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      if (debug) Serial.println("Connected");
      blinkerOrangeLED.attach(0.1, changeStateOrangeLED);
      deviceConnected = true;
      initial = true;
      initialCheck.detach();
      //pServer->getAdvertising()->start();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      initialCheck.attach(60, changeinitialCheck);
      pServer->getAdvertising()->start();
      blinkerOrangeLED.attach(2, changeStateOrangeLED);
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        //Serial.println("*********");
        //Serial.print("Received Value: ");
        rxBuffer = "";
        bluetoothCommand = "";
        bluetoothData = "";
        for (int i = 0; i < rxValue.length(); i++)
        {
          //  Serial.print(rxValue[i]);
          rxBuffer += rxValue[i];
        }
        //Serial.println();
        //Serial.println("*********");
        rxBuffer.trim();
        int indexPosition = rxBuffer.indexOf(":");
        if (indexPosition != -1) {
          bluetoothCommand = rxBuffer.substring(0, indexPosition);
          bluetoothData = rxBuffer.substring(indexPosition + 1);
          bluetoothCommand.trim();
          bluetoothData.trim();
        }
        if (rxBuffer == "Hi") {
          bluetoothPayload = "Hi\n";
          sendBluetoothData = true;
        }
        else if (bluetoothCommand == "bh") {
          writeFile("/brokerHost.txt", bluetoothData);
          bluetoothPayload = "start\rhost\rok\rend\n";
          sendBluetoothData = true;
        }

        else if (bluetoothCommand == "bp") {
          writeFile("/brokerPort.txt", bluetoothData);
          bluetoothPayload = "start\rport\rok\rend\n";
          sendBluetoothData = true;
        }
        else if (rxBuffer == "MAC") {
          bluetoothPayload = "start\rmac\r" + MAC_Address + "\rend\n";
          sendBluetoothData = true;
        }
        else if (rxBuffer == "INF") {
          bluetoothPayload = "start\rINF\r";
          doc.clear();
          doc["Type"] = "Multi Sensor";
          doc["HV"] = hardware_Version;
          doc["FV"] = firmware_version;
          doc["parameters"][0]["title"] = "Temperature";  doc["parameters"][0]["ui"] = "text"; doc["parameters"][0]["unit"] = "°C";
          doc["parameters"][1]["title"] = "Humidity";     doc["parameters"][1]["ui"] = "text"; doc["parameters"][1]["unit"] = "%";
          doc["parameters"][2]["title"] = "Door";         doc["parameters"][2]["ui"] = "text"; doc["parameters"][2]["unit"] = NULL;
          doc["parameters"][3]["title"] = "Movement";     doc["parameters"][3]["ui"] = "text"; doc["parameters"][3]["unit"] = NULL;
          doc["parameters"][4]["title"] = "Button";       doc["parameters"][4]["ui"] = "text"; doc["parameters"][4]["unit"] = NULL;
          String buff = "";
          serializeJson(doc, buff);
          bluetoothPayload += buff;
          bluetoothPayload += "\rend\n";

          sendBluetoothData = true;
        }
        else if (rxBuffer == "scanSSID") {
          // WiFi.mode(WIFI_STA);
          // WiFi.begin();
          // WiFi.disconnect();
          // esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
          // esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
          // WiFi.setTxPower(WIFI_POWER_2dBm);
          // WiFi.setSleep(WIFI_PS_MIN_MODEM);

          // int n = WiFi.scanNetworks();
        for(int i = 0; (i < 4) && (scanSSID_counter == 0); i++) {
          scanSSID_counter = WiFi.scanNetworks();
        }
        firstTimeScanSSID = false;
        wifiName = "start\rssids\r";
        for (int i = 0; i < scanSSID_counter; ++i) {
          wifiName += WiFi.SSID(i) + "\t" + WiFi.RSSI(i) + "\v";
        }
          wifiName += "\rend\n";
          scanSSID_counter = 0;
          bluetoothPayload = wifiName;
          sendBluetoothData = true;
          // WiFi.scanDelete();
          // delay(1000);
        }
        else if (rxBuffer == "deviceType") {
          bluetoothPayload = "start\rtype\r" + DeviceType + "\rend\n";
          sendBluetoothData = true;
        }
        else if (bluetoothCommand == "SSID") {
          SSID = bluetoothData;
          bluetoothPayload = "start\rssid\rok\rend\n";
          sendBluetoothData = true;
        }
        else if (bluetoothCommand == "PASS") {
          PASSWORD = bluetoothData;

          if (WiFi.status() == WL_CONNECTED) {
            WiFi.disconnect();
            delay(500);
          }
          WiFi.begin(SSID.c_str(), PASSWORD.c_str());
          WiFi.reconnect();
          bluetooth_wifi_check = true;
        }
        else if (rxBuffer == "reset" || rxBuffer == "restart") {
          writeFile("/SSID.txt", SSID);
          writeFile("/PASSWORD.txt", PASSWORD);

          ESP.restart();
        }
        else {
          bluetoothPayload = "start\rerror\rnot-defined\rend";
          sendBluetoothData = true;
        }

        Serial.println(rxBuffer);
        bluetoothCommand = "";
        bluetoothData = "";
        rxBuffer = "";
      }
    }
};

void bluetoothConfig(bool BLEstat) {
  // Initialize Wi-Fi
  WiFi.mode(WIFI_STA);  // Set Wi-Fi mode to Station
  WiFi.begin();         // Start Wi-Fi (no connection required to get MAC)
  WiFi.setTxPower(WIFI_POWER_2dBm);
  mymac = WiFi.macAddress();
  
  WiFi.disconnect();

  // TODO: Fix the base64
  char* base64_input = new char[mymac.length() + 1];    // Allocate memory
  mymac.toCharArray(base64_input, mymac.length() + 1);  // Copy content
  int base64_output_length = base64_enc_len(strlen(base64_input));
  char* base64_output = new char[base64_output_length];
  base64_encode(base64_output, base64_input, strlen(base64_input));
  MAC_Address = String(base64_output);

  BLEDevice::init("MiniSensor");
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  if (BLEstat) {
    pService->start();
    blinkerOrangeLED.attach(2, changeStateOrangeLED);
    Serial.println("Waiting a client connection to notify...");
    pServer->getAdvertising()->start();
  }
  else {
    pServer->getAdvertising()->stop();
    BLEDevice::deinit(true);
  }
}

void bluetoothLoop() {
  if (deviceConnected) {
    if (sendBluetoothData) {
      pTxCharacteristic->setValue(bluetoothPayload.c_str());
      pTxCharacteristic->notify();
      delay(10);
      if (debug) Serial.println(bluetoothPayload);
      sendBluetoothData = false;
    }
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    //ESP.restart();
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    blinkerOrangeLED.attach(2, changeStateOrangeLED);
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}
