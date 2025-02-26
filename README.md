# Hardware - MiniSensor
A package combining temperature and humidity sensors, a motion detector, a door sensor, and a panic button.

# Technical Specifications
- Product Name: MiniSensor
- Size:
  - Big Part: 7 × 2 × 2.5 cm
  - Small Part: 5 × 1 × 1 cm
- Weight:
  - Big Part: 22 g
  - Small Part: 4 g
- Communication: 2.4 GHz Wi-Fi
- Operating Voltage: 5V to 12V DC
- Current Consumption: 500 mA
- Power Consumption: 0.6 Wh
- Operating Temperature Range: -10°C to +60°C
- Humidity Range: 1% to 100% RH

# Folder Description
- BIN_MiniSensor: Contains the binary files of the MiniSensor source code.
- MiniSensor: The source code for the MiniSensor to operate standalone and send data via Wi-Fi. The code is developed using Arduino IDE.
- IOT2050_MiniSensor: The source code for the MiniSensor to operate with Siemens IoT2050 and send data via its USB-CDC UART port. The code is developed using Arduino IDE.
- PCB: Contains the schematic and PCB design of the MiniSensor, developed using Altium Designer.
- BIN_MiniSensor.zip: A compressed version of the BIN_MiniSensor folder.

Next Action:
- Create a GSD file to integrate the MiniSensor into Siemens TIA Portal.
