/*void ota_update() {
  pinMode(OrangeLED, OUTPUT);
  char OTA_FINGERPRINT[100];
  char OTA_URL_CHAR[200];
  initialCheck.detach();
  sendDataTicker.detach();
  removeFile("OTA.txt");
  delay(200);
  initial = true;

  blinkerOrangeLED.attach(0.05, changeStateOrangeLED);
  WiFiClientSecure otaClient;
  String fingerprintBuffer;
  fingerprintBuffer = readFile("/FINGERPRINT.txt");
  fingerprintBuffer.toCharArray(OTA_FINGERPRINT, (int)fingerprintBuffer.length());

  String OTA_URL;
  OTA_URL = readFile("/FIRM_URL.txt");

  if (debug) {
    Serial.println(OTA_FINGERPRINT);
    Serial.println(OTA_URL);
  }
  otaClient.setInsecure();

  t_httpUpdate_return ret = ESPhttpUpdate.update(OTA_URL);
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      delay(2000);
      ESP.restart();
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println(F("HTTP_UPDATE_NO_UPDATES"));
      delay(2000);
      ESP.restart();
      break;

    case HTTP_UPDATE_OK:
      Serial.println(F("HTTP_UPDATE_OK"));
      delay(2000);
      ESP.restart();
      break;
  }
}
*/