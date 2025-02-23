/*****************************************************************************/
/****************************  connect to Wi-Fi  *****************************/
//void ConnectTowifi() {
  /*blinkerOrangeLED.attach(0.2, changeStateOrangeLED);
  if (user != "") {
    initial = true;
    wifiTry = 0;
    WiFi.mode(WIFI_STA);
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B); // WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
    esp_wifi_config_80211_tx_rate(WIFI_IF_STA, WIFI_PHY_RATE_1M_L);//WIFI_PHY_RATE_1M_L|WIFI_PHY_RATE_2M_S|WIFI_PHY_RATE_MCS0_LGI|WIFI_PHY_RATE_MCS0_SGI);
    //esp_wifi_internal_set_fix_rate(WIFI_IF_STA, true, WIFI_PHY_RATE_1M_L);
    //esp_wifi_set_max_tx_power(8);
    WiFi.setTxPower(WIFI_POWER_5dBm);
    WiFi.setSleep(WIFI_PS_MIN_MODEM);

    if (debug) {
      uint8_t protocol_bitmap;
      esp_wifi_get_protocol(WIFI_IF_STA, &protocol_bitmap);
      Serial.print("protocol_bitmap: ");
      Serial.println(protocol_bitmap);
    }

    delay(100);
    WiFi.disconnect();
    WiFi.begin(SSID.c_str(), PASSWORD.c_str());
    WiFi.reconnect();
    //WiFi.setTxPower(WIFI_POWER_19_5dBm);

    //WiFi.enableLongRange(true);
    //WiFi.setSleep(WIFI_PS_MIN_MODEM);//WIFI_PS_NONE);//WIFI_PS_MIN_MODEM);
    //WiFi.enableLongRange(false);
    int wifiPower;
    wifiPower = WiFi.getTxPower();

    if (debug) {
      Serial.println(wifiPower * 0.25);
      Serial.println(WiFi.getSleep());
    }

    while (WiFi.status() != WL_CONNECTED && wifiTry <= 600) {
      if (buttonPressed) factory_reset();
      wifiTry++;
      delay(50);
      if (wifiTry == 100) {
        WiFi.disconnect();
        //WiFi.mode(WIFI_OFF);
        delay(500);
        //WiFi.mode(WIFI_STA);
        WiFi.reconnect();
        //WiFi.setTxPower(WIFI_POWER_19_5dBm);
      }
    }

    if (debug) Serial.println("wifi status= " + WiFi.status());
    if (wifiTry >= 600 && WiFi.status() != WL_CONNECTED) {
      if (debug) Serial.println("wifi could not connect");
      bluetoothConfig(true);
    }
    else {
      flagcheckOFFwifi = false;
      if (debug) Serial.println("wifi connected");
      bluetoothConfig(false);
      TurnOffLEDs();
      initial = false;

      //esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B);
      //  esp_wifi_set_inactive_time(WIFI_IF_STA, 30);
    }
  }
  else {
    initial = true;
    bluetoothConfig(true);
  }*/
//}
/****************************  connect to Wi-Fi  *****************************/
/*****************************************************************************/
