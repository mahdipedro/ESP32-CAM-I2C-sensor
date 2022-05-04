# Interfacing ESP32-CAM using I2C with a sensor
I will show you how to take pictures from the ESP32-CAM camera and MLX90640 thermal camera and store them on the module's SD card. There are certain steps that need to be taken for this project:
### 1- Activating software I2C for MLX90640
```
#define I2C_SDA 1 // SDA Connected to GPIO 14
#define I2C_SCL 3 // SCL Connected to GPIO 15
```
### 2- Read the data from both RGB and thermal cameras continuously
### 3- Store the collected data into the SD card
