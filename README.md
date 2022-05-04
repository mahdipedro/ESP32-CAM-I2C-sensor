# Interfacing ESP32-CAM using I2C with a sensor
I will show you how to take pictures from the ESP32-CAM camera and MLX90640 thermal camera and store them on the module's SD card. There are certain steps that need to be taken for this project:
### 1- Activating software I2C for MLX90640
We need to define two pins to activate I2C functions based on those (clock and data). According to ESP32S datasheet (https://www.es.co.th/Schemetic/PDF/ESP32.PDF) I selected GPIO1 and GPIO3 because we use other pins for the camera and SPI protocol which is required for writing the data into the SD card. The selected pins are initially used for UART communication which is useful if you want to connect the board to your computer or any other platform using UART. However, we did not need the communication since the device was supposed to collect RGB and thermal data continuously and store the data on the SD card. Therefore, you can change the fincionality of these two pins to I2C protocol.
```
#define I2C_SDA 1 // SDA Connected to GPIO 1
#define I2C_SCL 3 // SCL Connected to GPIO 3
```
### 2- Read the data from both RGB and thermal cameras continuously
### 3- Store the collected data into the SD card
