
void read_sensor() {
  hdc1080.begin(0x40);
  /*if (debug) {
    Serial.print("Manufacturer ID=0x");
    Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
    Serial.print("Device ID=0x");
    Serial.println(hdc1080.readDeviceId(), HEX); // 0x1050 ID of the device
    }*/
  temp = hdc1080.readTemperature();
  humidity = hdc1080.readHumidity();

  // Maintain a fixed-size window
  if (tempReadings.size() >= WINDOW_SIZE) {
      tempReadings.erase(tempReadings.begin()); // Remove oldest value
  }
  
  tempReadings.push_back(temp);


  if (humidityReadings.size() >= WINDOW_SIZE) {
      humidityReadings.erase(humidityReadings.begin()); // Remove oldest value
  }
  humidityReadings.push_back(humidity);

  // Predict the next value based on the collected data
  predictedTempValue = predictNextValue(tempReadings);
  predictedHumidityValue = predictNextValue(humidityReadings);
}

// Function to compute the next predicted value using linear regression
float predictNextValue(const std::vector<float>& data) {
    int n = data.size();
    if (n < 2) return data.back(); // Not enough data to predict

    float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    for (int i = 0; i < n; i++) {
        sumX += i;
        sumY += data[i];
        sumXY += i * data[i];
        sumX2 += i * i;
    }

    float slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    float intercept = (sumY - slope * sumX) / n;

    return slope * n + intercept; // Predict next value at x = n
}
