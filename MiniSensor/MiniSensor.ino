/*
  MiniSensor_By_MoKasaei
  Based on ESP32-C3
  Hardware_Version = 4
  Firmware_Version = 6
  ESP32 board V3.0.7
  CPU Freq = 160MHz
  Partition Scheme = No OTA (2MB APP/2MB SPIFFS)
*/

/*****************************************************************************/
/*********************************  library  *********************************/
#include <Wire.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "esp_private/wifi.h"
#include "PubSubClient.h"
#include "base64.h"
#include <DNSServer.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <FS.h>
#include "SPIFFS.h"
#include <WiFiClientSecure.h>

#include <painlessMesh.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <driver/adc.h>

// #include "soc/soc.h"
// #include "soc/rtc_cntl_reg.h"
// #include <driver/temp_sensor.h>

#include "ClosedCube_HDC1080.h"

#include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
/*********************************  library  *********************************/
/*****************************************************************************/
void ICACHE_RAM_ATTR button_pressed();
void ICACHE_RAM_ATTR door_status_changed();
void ICACHE_RAM_ATTR pir_status_changed();

void writeString(int address, String data);
String read_String(int address);
void Connect_MQTT();
void TurnOffLEDs();
void callback(char* topic, byte* payload, unsigned int length);
void changeStateOrangeLED();
void changeinitialCheck();
void resetMqtt();
//void ConnectTowifi();
void read_sensor();
void factory_reset();
// void ota_update();
void bluetoothLoop();
void bluetoothConfig(bool BLEstat);
void sendDataVoid();
void meshDataVoid();
void espRestart();

size_t getRootId(painlessmesh::protocol::NodeTree nodeTree);
bool checkForAP(String SSID_NAME);

void meshSetup(String nodeStatus);
void sendMessage(String buff);
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);
void delayReceivedCallback(uint32_t from, int32_t delay);

String readFile(String fileName);
bool writeFile(String fileName, String payload);
void removeFile(String fileName);

void readSensorVoid();

void meshStatus();

double roundFloat(double value);

String scanprocessor(const String& var);

float predictNextValue(const std::vector<float>& data);

IPAddress apIP(192, 168, 100, 100);
DNSServer dnsServer;
// WebServer webServer(80);

// AsyncWebServer server(80);

char mqttServer[100];

String DeviceType = "MINI_SENSOR";
String SSID = "", PASSWORD = "", token = "", request = "", inputData = "", wifiName = "";
int wifiTry = 0;
String flagymqtt;

boolean flagcheckOFFwifi = false, firstPayload = true, batteryDataSend = false;

int OrangeLED = 8;
int button = 1;
int PIR = 3;
int reedSwitch = 0;
int Thermistor = 4;

bool temperature_data_sent = false;
bool movement_data_sent = false;
bool button_data_sent = false;
bool door_data_sent = false;
bool mesh_data_sent = true;

Ticker blinkerOrangeLED;
Ticker initialCheck;
Ticker sendDataTicker;
Ticker readSensorTicker;
Ticker meshDataTicker;
Ticker mqttWatchdog;
long mqttWatchdogTimer;
// String mymac = WiFi.macAddress();
// String MAC_Address = base64::encode(mymac);

String mymac;
String MAC_Address;

String topic = MAC_Address.c_str(), mqttUser, mqttPassword;
String payloadMAIN;

int percentage;
int initial = true;
int mqttconnection = 0;

char *brokerHost, *brokerPort, *domainUrl;
String bh, bp, du;


WiFiClientSecure espClient;
PubSubClient client(espClient);

#define si7021Addr 0x40
float humidity = 0;
float temp = 0;

int Vo;
float R1 = 10000;
float logR2, R2, T, Tc;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

bool buttonPressed = false;

String doorStatus, movementStatus, buttonStatus = "NOT Pressed";

bool ledFade = false;
bool ledGoBriht = true;
int ledBrightness = 0;

int resetCounter = 0;
int buttonCounter = 0;
bool startNewButtonCounter = false;
unsigned long oldButtonPressTimer = -50;

bool bluetooth_wifi_check = false;
String bluetoothPayload;
bool sendBluetoothData = false;

bool LED_BLINK_ON = false;

#define   MESH_SSID       "MeshNetwork"
#define   MESH_PASSWORD   "TWVzaE5ldHdvcms="
#define   MESH_PORT       5555
#define   HOSTNAME        "FDS_Bridge"
Scheduler userScheduler;

Task checkMeshStatus(TASK_SECOND * 1, TASK_FOREVER, &meshStatus);

bool foundAP = false;
int WiFi_RSSI = -100;
int WiFi_channel = 0;

bool meshHasRoot = false;
bool root = true;
bool rootFlag = false;
painlessMesh  mesh;
String nodeStatus;
bool rootData = false;

bool calc_delay = false;
SimpleList<uint32_t> nodes;

bool onFlag = false;

ClosedCube_HDC1080 hdc1080;

DynamicJsonDocument doc(2048);

unsigned long Millis = 0;

bool startAsNode = true;
bool oneTime = true;
bool partOfMesh = false;
bool rootIsavailable = false;
unsigned int Seconds = 0;

IPAddress getlocalIP();
IPAddress myIP(0, 0, 0, 0);

int scanSSID_counter = 0;

std::vector<float> tempReadings;
std::vector<float> humidityReadings;
float predictedTempValue;
float predictedHumidityValue;
const int WINDOW_SIZE = 10; // Number of previous readings used for prediction

const int hardware_Version = 4;
const int firmware_version = 6;
bool debug = false;

/*****************************************************************************/
/**********************************  setup  **********************************/
void setup() {
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  if (debug) Serial.println("-> setup()");
  EEPROM.begin(512);

  pinMode(OrangeLED, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(reedSwitch, INPUT);
  pinMode(PIR, INPUT);
  if (digitalRead(button) == LOW) {
    buttonPressed = true;
  }
  analogWrite(OrangeLED, 0);

  attachInterrupt(digitalPinToInterrupt(reedSwitch), door_status_changed, CHANGE);
  attachInterrupt(digitalPinToInterrupt(button), button_pressed, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIR), pir_status_changed, CHANGE);
  read_sensor();

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }


  // MAC_Address = base64::encode(mymac);
  // mymac = WiFi.macAddress();

  analogWrite(OrangeLED, 254);

  SSID = readFile("/SSID.txt");
  PASSWORD = readFile("/PASSWORD.txt");

  bh = readFile("/brokerHost.txt");
  bp = readFile("/brokerPort.txt");

  //flagymqtt = readFile("/flagMQTT.txt");

  if (debug) {
    Serial.println("ssid= " + SSID);
    Serial.println("pass= " + PASSWORD);
    Serial.println("brokerHost= " + bh);
    Serial.println("brokerPort= " + bp);
    Serial.println("flagymqtt= " + flagymqtt);
  }

  nodeStatus = readFile("/node.txt");

  if (nodeStatus == "") {
    nodeStatus = "root";
    writeFile("/node.txt", nodeStatus);
  }

  TurnOffLEDs();
  sendDataTicker.attach(10, sendDataVoid);
  initialCheck.attach(60, changeinitialCheck);

  if (flagymqtt == "2") {
    topic = readFile("/topic.txt");
  }


  if (digitalRead(reedSwitch) == HIGH) doorStatus = "Open";
  else doorStatus = "Close";

  if (digitalRead(PIR) == HIGH) movementStatus = "Detected";
  else movementStatus = "Scanning...";

  // temp_sensor_config_t temp_sensor = {
  //   .dac_offset = TSENS_DAC_L2,
  //   .clk_div = 6,
  // };
  // temp_sensor_set_config(temp_sensor);
  // temp_sensor_start();

  // if (readFile("OTA.txt") == "true") {
  //   ota_update();
  // }

  read_sensor();
  meshSetup(nodeStatus);

  if (debug) Serial.println("MAC=" + mymac);
  
  char* base64_input = new char[mymac.length() + 1];    // Allocate memory
  mymac.toCharArray(base64_input, mymac.length() + 1);  // Copy content
  int base64_output_length = base64_enc_len(strlen(base64_input));
  char* base64_output = new char[base64_output_length];
  base64_encode(base64_output, base64_input, strlen(base64_input));
  MAC_Address = String(base64_output);

  tempReadings.reserve(WINDOW_SIZE);
  humidityReadings.reserve(WINDOW_SIZE);
  for (int i = 0; i < WINDOW_SIZE; i++) {
    read_sensor();
  }

  if (nodeStatus == "root") {
    meshDataTicker.attach(15, meshDataVoid);
    rootData = true;
  }
  else {
    rootData = false;
  }
  if (initial) {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.setTxPower(WIFI_POWER_2dBm);
    delay(100);
    scanSSID_counter = WiFi.scanNetworks();
  }
  else {
    readSensorTicker.attach(10, readSensorVoid);
  }
}
/**********************************  setup  **********************************/
/*****************************************************************************/


/*****************************************************************************/
/***********************************  loop  **********************************/
void loop() {
  if (initial) {
    if (buttonPressed) factory_reset();
    if (bluetooth_wifi_check) {
      if (WiFi.status() != WL_CONNECTED && wifiTry <= 60) {
        if (buttonPressed) factory_reset();
        if (debug) Serial.print(".");
        wifiTry++;
        delay(500);
        if (wifiTry == 10) {
          WiFi.disconnect();
          WiFi.reconnect();
          //WiFi.setTxPower(WIFI_POWER_19_5dBm);
        }
      }
      if (wifiTry >= 60 && WiFi.status() != WL_CONNECTED) {
        wifiTry = 0;
        if (debug) Serial.println("NOTCONNECTED");
        bluetooth_wifi_check = false;
        bluetoothPayload = "start\rpass\rfalse\rend\n";
        sendBluetoothData = true;
      } else if (WiFi.status() == WL_CONNECTED) {
        wifiTry = 0;
        if (debug) Serial.println("CONNECTED");
        bluetooth_wifi_check = false;
        bluetoothPayload = "start\rpass\rtrue\rend\n";
        sendBluetoothData = true;
      }
    }
    bluetoothLoop();
    return;
  }

  mesh.update();

  if (nodeStatus == "root") {
    if (WiFi.status() == WL_CONNECTED) {
      client.loop();
      if (!client.connected()) {
        Connect_MQTT();
      }
    }
  }

  if (firstPayload == true && flagymqtt != "2") {
    firstPayload = false;
    doc.clear();
    doc["nodeID"] = mesh.getNodeId();
    doc["from"] = MAC_Address;
    doc["to"] = bh;
    doc["data"]["FV"] = firmware_version;
    doc["data"]["HV"] = hardware_Version;
    doc["data"]["Root"] = rootData;
    payloadMAIN = "";
    serializeJson(doc, payloadMAIN);
    if (mqttconnection == 1) {
      client.publish((char*)topic.c_str(), (char*)payloadMAIN.c_str());
    }
    if (nodeStatus == "node") {
      if (debug) {
        Serial.print("Sending Broadcast: ");
        Serial.println(payloadMAIN);
      }
      sendMessage(payloadMAIN);
    }
  }
  if (flagymqtt != "2" && temperature_data_sent == false) {
    read_sensor();
    doc.clear();
    doc["nodeID"] = mesh.getNodeId();
    doc["from"] = MAC_Address;
    doc["to"] = bh;
    doc["data"]["FV"] = firmware_version;
    doc["data"]["HV"] = hardware_Version;
    doc["data"]["Root"] = rootData;
    doc["data"]["Temperature"] = roundFloat(predictedTempValue);
    doc["data"]["Humidity"] = roundFloat(predictedHumidityValue);
    doc["data"]["Movement"] = movementStatus;
    doc["data"]["Door"] = doorStatus;
    doc["data"]["Button"] = buttonStatus;
    payloadMAIN = "";
    serializeJson(doc, payloadMAIN);
    if (mqttconnection == 1) {
      client.publish((char*)topic.c_str(), (char*)payloadMAIN.c_str());
    }
    if (nodeStatus == "node") {
      if (debug) {
        Serial.print("Sending Broadcast: ");
        Serial.println(payloadMAIN);
      }
      sendMessage(payloadMAIN);
    }
    TurnOffLEDs();
    if (debug) Serial.println(payloadMAIN);
    temperature_data_sent = true;
  }
  if (flagymqtt != "2" && mesh_data_sent == false) {
    doc.clear();
    doc["nodeID"] = mesh.getNodeId();
    doc["from"] = MAC_Address;
    doc["to"] = bh;
    doc["meshNetwork"] = mesh.subConnectionJson(false);
    doc["wifiName"] = SSID;
    doc["deviceIP"] = myIP.toString();
    doc["data"]["FV"] = firmware_version;
    doc["data"]["HV"] = hardware_Version;
    doc["data"]["Root"] = rootData;
    doc["data"]["Temperature"] = roundFloat(predictedTempValue);
    doc["data"]["Humidity"] = roundFloat(predictedHumidityValue);
    doc["data"]["Movement"] = movementStatus;
    doc["data"]["Door"] = doorStatus;
    doc["data"]["Button"] = buttonStatus;
    payloadMAIN = "";
    serializeJson(doc, payloadMAIN);
    if (mqttconnection == 1) {
      client.publish((char*)topic.c_str(), (char*)payloadMAIN.c_str());
    }
    if (nodeStatus == "node") {
      if (debug) {
        Serial.print("Sending Broadcast: ");
        Serial.println(payloadMAIN);
      }
      sendMessage(payloadMAIN);
    }
    TurnOffLEDs();
    if (debug) Serial.println(payloadMAIN);
    mesh_data_sent = true;
  }
  if (flagymqtt != "2" && movement_data_sent == false) {
    doc.clear();
    doc["nodeID"] = mesh.getNodeId();
    doc["from"] = MAC_Address;
    doc["to"] = bh;
    doc["data"]["FV"] = firmware_version;
    doc["data"]["HV"] = hardware_Version;
    doc["data"]["Root"] = rootData;
    doc["data"]["Temperature"] = roundFloat(predictedTempValue);
    doc["data"]["Humidity"] = roundFloat(predictedHumidityValue);
    doc["data"]["Movement"] = movementStatus;
    doc["data"]["Door"] = doorStatus;
    doc["data"]["Button"] = buttonStatus;
    payloadMAIN = "";
    serializeJson(doc, payloadMAIN);
    if (mqttconnection == 1) {
      client.publish((char*)topic.c_str(), (char*)payloadMAIN.c_str());
    }
    if (nodeStatus == "node") {
      if (debug) {
        Serial.print("Sending Broadcast: ");
        Serial.println(payloadMAIN);
      }
      sendMessage(payloadMAIN);
    }
    TurnOffLEDs();
    if (debug) Serial.println(payloadMAIN);
    movement_data_sent = true;
  }
  if (flagymqtt != "2" && door_data_sent == false) {
    if (digitalRead(reedSwitch) == HIGH) doorStatus = "Open";
    else doorStatus = "Close";
    doc.clear();
    doc["nodeID"] = mesh.getNodeId();
    doc["from"] = MAC_Address;
    doc["to"] = bh;
    doc["data"]["FV"] = firmware_version;
    doc["data"]["HV"] = hardware_Version;
    doc["data"]["Root"] = rootData;
    doc["data"]["Temperature"] = roundFloat(predictedTempValue);
    doc["data"]["Humidity"] = roundFloat(predictedHumidityValue);
    doc["data"]["Movement"] = movementStatus;
    doc["data"]["Door"] = doorStatus;
    doc["data"]["Button"] = buttonStatus;
    payloadMAIN = "";
    serializeJson(doc, payloadMAIN);
    if (mqttconnection == 1) {
      client.publish((char*)topic.c_str(), (char*)payloadMAIN.c_str());
    }
    if (nodeStatus == "node") {
      if (debug) {
        Serial.print("Sending Broadcast: ");
        Serial.println(payloadMAIN);
      }
      sendMessage(payloadMAIN);
    }
    TurnOffLEDs();
    if (debug) Serial.println(payloadMAIN);
    door_data_sent = true;
  }
  if (flagymqtt != "2" && button_data_sent == false) {
    if (buttonPressed) factory_reset();
    doc.clear();
    doc["nodeID"] = mesh.getNodeId();
    doc["from"] = MAC_Address;
    doc["to"] = bh;
    doc["data"]["FV"] = firmware_version;
    doc["data"]["HV"] = hardware_Version;
    doc["data"]["Root"] = rootData;
    doc["data"]["Temperature"] = roundFloat(predictedTempValue);
    doc["data"]["Humidity"] = roundFloat(predictedHumidityValue);
    doc["data"]["Movement"] = movementStatus;
    doc["data"]["Door"] = doorStatus;
    doc["data"]["Button"] = buttonStatus;
    doc["data"]["proof"] = "['0x1f8860197285a3ddfbc89f5a2aa2cbb4cd210ad28bf09cd5008bac5421f9561c', '0x19547b93d12bf7a3d010aee87a1062e38e27cb9afff12fe9042d626834a920c2'],[['0x03fffba13c24f0a7b739de0e926ae7f4f5c0d07faf3d7802d9fe537155de84a7', '0x22b4496c8d7ba69dff30aa79c50ca2325548f8927b8fa6548223da7dffbed375'],['0x0d5830c0da1666254bda8efdbb144bcdfead2806116146e40d69e231bb0d1e43', '0x0faf1409faac394c0ba73fddc3ab0dd8311d187713365caf89fdbc58a91c91b4']],['0x00edc9bcbeb8d724be20194c42df57da2cc98d1fad415db34ad617563feaf9c6', '0x0fad053f3d9c409eee48e98a3a4b55ac89e9f60994ec2ed7d32efb3d0fa55b70'],['0x2f54fa38f46e70bc2972399eab1561f328de7f3fd318df6c068b635ccd6ebc46']";
    payloadMAIN = "";
    serializeJson(doc, payloadMAIN);
    if (mqttconnection == 1) {
      client.publish((char*)topic.c_str(), (char*)payloadMAIN.c_str());
    }
    if (nodeStatus == "node") {
      if (debug) {
        Serial.print("Sending Broadcast: ");
        Serial.println(payloadMAIN);
      }
      sendMessage(payloadMAIN);
    }
    TurnOffLEDs();
    if (debug) Serial.println(payloadMAIN);
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
      if (ledBrightness >= 150) ledGoBriht = false;
    } else {
      ledBrightness--;
      if (ledBrightness <= 0) ledGoBriht = true;
    }
    analogWrite(OrangeLED, ledBrightness);
    delay(50);
  }

  if (myIP != getlocalIP())
  {
    myIP = getlocalIP();
    if (debug)Serial.println("My IP is " + myIP.toString());
  }

  /*if (debug) {
    read_sensor();
    float tsens_out;
    temp_sensor_read_celsius(&tsens_out);
    Serial.print("ESP32-C3: ");
    Serial.print(tsens_out);
    Serial.print(" ℃        ");

    Serial.print("sensor: ");
    Serial.print(temp);
    Serial.print(" ℃        ");

    Serial.print("Thermistor: ");
    Serial.print(Tc);
    Serial.println(" ℃");
    }*/
}
/***********************************  loop  **********************************/
/*****************************************************************************/


/*****************************************************************************/
/*******************************  on command  ********************************/
void onCommand(const char* command, StaticJsonDocument<256> doc) {
  if (debug) Serial.print("onCommand > ");
  if (debug) Serial.println(command);
}
/*******************************  on command  ********************************/
/*****************************************************************************/
