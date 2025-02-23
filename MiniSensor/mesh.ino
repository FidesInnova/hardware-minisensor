size_t getRootId(painlessmesh::protocol::NodeTree nodeTree) {
  if (nodeTree.root) return nodeTree.nodeId;
  for (auto && s : nodeTree.subs) {
    auto id = getRootId(s);
    if (id != 0) return id;
  }
  return 0;
}


bool checkForAP(String SSID_NAME) {
  int apNum = WiFi.scanNetworks();
  foundAP = false;
  if (apNum == 0) {
    Serial.println("Found no networks");
  }
  else {
    int MESH_SSID_counter = 0;
    for (int index = 0; index < apNum && foundAP == false; index++) {
      String ssid = WiFi.SSID(index);
      if (!strcmp((const char *)&ssid[0], SSID_NAME.c_str())) {
        WiFi_RSSI = WiFi.RSSI(index);
        WiFi_channel = WiFi.channel(index);
        if (debug) {
          Serial.print("Found gateway AP: ");
          Serial.print(SSID_NAME);
          Serial.print(" RSSI: ");
          Serial.print(WiFi_RSSI);
          Serial.print(" Channel: ");
          Serial.println(WiFi_channel);
        }
        if (!strcmp(MESH_SSID, SSID_NAME.c_str())) {
          MESH_SSID_counter++;
          if (MESH_SSID_counter > 0) {
            foundAP = true;
          }
        }
        else {
          foundAP = true;
        }
      }
    }
  }
  return foundAP;
}


void meshSetup(String nodeStatus) {
  if (debug) {
    Serial.print("this device is: ");
    Serial.println(nodeStatus);
  }
  blinkerOrangeLED.attach(0.2, changeStateOrangeLED);
  if (SSID != "") {
    initial = false;

    if (debug) {
      mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION | DEBUG);
    }
    WiFi.disconnect();
    delay(100);
    //WiFi.reconnect();
    //WiFi.mode(WIFI_AP_STA);
    mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, WiFi_channel);

    //WiFi.disconnect();
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
    esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
    WiFi.setTxPower(WIFI_POWER_5dBm);
    WiFi.setSleep(WIFI_PS_MIN_MODEM);

    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
    mesh.onNodeDelayReceived(&delayReceivedCallback);

    if (nodeStatus == "root") { // && !startAsNode) {
      if (debug)Serial.println("I AM ROOT");
      rootFlag = true;
      mesh.setRoot(true);

      //mesh.setContainsRoot(true);

      mesh.stationManual(SSID.c_str(), PASSWORD.c_str());

      //WiFi.begin(SSID.c_str(), PASSWORD.c_str());
      //WiFi.reconnect();

      mesh.setHostname(HOSTNAME);
    }
    else if (nodeStatus == "node") {
      if (debug)Serial.println("I AM NODE");
    }

    userScheduler.addTask(checkMeshStatus);
    checkMeshStatus.enable();

    TurnOffLEDs();

    
    mymac = WiFi.macAddress();
  }
  else {
    initial = true;
    bluetoothConfig(true);
  }
}


void sendMessage(String buff) {
  mesh.sendBroadcast(buff);

  if (calc_delay) {
    SimpleList<uint32_t>::iterator node = nodes.begin();
    while (node != nodes.end()) {
      mesh.startDelayMeas(*node);
      node++;
    }
    calc_delay = false;
  }
  if (debug) Serial.printf("Sending message: %s\n", buff.c_str());
}


void receivedCallback(uint32_t from, String & msg) {
  if (debug) Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  if (msg.indexOf("RSSI") != -1) {
    deserializeJson(doc, msg);
    if (doc.containsKey("RSSI")) {
      String rssi_from_other_noeds = doc["RSSI"];
      if (WiFi_RSSI < rssi_from_other_noeds.toInt()) {
        root = false;
      }
      else if (WiFi_RSSI == rssi_from_other_noeds.toInt()) {
        if (from > mesh.getNodeId()) {
          root = false;
        }
      }
      if (nodeStatus == "root" || meshHasRoot == true) {
        Serial.print("RSSI ");
        Serial.print(from);
        Serial.print(": ");
        Serial.println(String(rssi_from_other_noeds));
      }
    }
  }
  else if (msg.indexOf("I AM ROOT") != -1) {
    rootIsavailable = true;
  }
  else {
    if ( mqttconnection == 1) {
      client.publish((char*)topic.c_str(), (char*)msg.c_str());
    }
  }
}


void newConnectionCallback(uint32_t nodeId) {
  if (debug) Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  if (debug) Serial.printf("--> startHere: New Connection, %s\n", mesh.subConnectionJson(true).c_str());

  partOfMesh = true;
}

void changedConnectionCallback() {
}


void nodeTimeAdjustedCallback(int32_t offset) {
}


void delayReceivedCallback(uint32_t from, int32_t delay) {
}





void meshStatus() {
  client.loop();
  mesh.update();
  if (debug) {
    Serial.print("Seconds: ");
    Serial.println(Seconds);
  }
  Seconds++;
  if (nodeStatus == "root") {
    if (partOfMesh) {
      if (Seconds % 2 == 0) {
        sendMessage("I AM ROOT");
        if (debug) Serial.println("sendMessage: I AM ROOT");
      }
    }
    else if (Seconds % 15 == 0) {
      if (checkForAP(MESH_SSID)) {
        while (!writeFile("/node.txt", "node")) {
          delay(100);
        }
        if (debug) {
          Serial.println("switching to node: found another mesh network");
          delay(1000);
        }
        ESP.restart();
      }
      if (WiFi.status() != WL_CONNECTED) {
        if (!checkForAP(SSID)) {
          while (!writeFile("/node.txt", "node")) {
            delay(100);
          }
          if (debug) {
            Serial.println("switching to node: can't find Wi-Fi router");
            delay(1000);
          }
          ESP.restart();
        }
      }
    }
  }
  else if (nodeStatus == "node") {
    if (Seconds % 10 == 0) {
      if (!partOfMesh) {
        if (!checkForAP(MESH_SSID)) {
          if (checkForAP(SSID)) {
            while (!writeFile("/node.txt", "root")) {
              delay(100);
            }
            if (debug) {
              Serial.println("switching to root: can't find a mesh network");
              delay(1000);
            }
            ESP.restart();
          }
        }
      }
    }
    if (Seconds % 15 == 0) {
      if (rootIsavailable) {
        rootIsavailable = false;
      }
      else {
        if (debug) {
          Serial.println("voting to choose root");
          delay(1000);
        }
        if (checkForAP(SSID)) {
          while (!writeFile("/node.txt", "root")) {
            delay(100);
          }
          if (debug) {
            Serial.println("switching to root: can't find a ROOT in the mesh network");
            delay(1000);
          }
          ESP.restart();
        }
      }
    }
  }
}
