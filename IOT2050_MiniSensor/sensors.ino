#include <Wire.h>
#include <Adafruit_Si7021.h>

#define Si7021_SDA 6
#define Si7021_SCL 7

Adafruit_Si7021 si7021 = Adafruit_Si7021();

// Function to reinitialize I2C on new pins
void reinitializeI2C(uint8_t sda, uint8_t scl) {
  Wire.end();            // Deinitialize I2C
  Wire.begin(sda, scl);  // Reinitialize I2C on new pins
  delay(100);            // Small delay for I2C bus stabilization
}

void setup_sensors() {
  // Step 1: Initialize I2C for Si7021
  reinitializeI2C(Si7021_SDA, Si7021_SCL);
  if (!si7021.begin()) {
  } else {
    si7021.heater(false);
  }
}

void read_sensors() {
  // Read Si7021 data
  temp = si7021.readTemperature();
  humidity = si7021.readHumidity();

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

