# Interfacing ESP32-CAM using I2C with a sensor
I will show you how to take pictures from the ESP32-CAM camera and MLX90640 thermal camera and store them on the module's SD card. There are certain steps that need to be taken for this project:
### 1- Activating software I2C for MLX90640
We need to define two pins to activate I2C functions based on those (clock and data). According to ESP32S datasheet (https://www.es.co.th/Schemetic/PDF/ESP32.PDF), any GIPO pins can be used for I2C function. As a result, I selected GPIO1 and GPIO3 because we use other pins for the camera and SPI protocol which is required for writing the data into the SD card. 


  <p align="center">
  <img width="500" height="360" src="https://user-images.githubusercontent.com/45086751/166619952-ab1e3956-f4c5-4df9-8a16-346f8e809a74.jpg">
  </p> 
  
  
The selected pins are initially used for UART communication which is useful if you want to connect the board to your computer or any other platform using UART. However, we did not need the communication since the device was supposed to collect RGB and thermal data continuously and store the data on the SD card. Therefore, you can change the fincionality of these two pins to I2C protocol. The following code will change the functionality of the two mentioned pins and then we define our I2C bus:
```
#define I2C_SDA 1 // SDA Connected to GPIO 1
#define I2C_SCL 3 // SCL Connected to GPIO 3
TwoWire I2CSensors = TwoWire(0);
```
Then, In the setup, initialize the I2C communication with the pins that we defined earlier (GIPO1 and GPIO3). The third parameter is the clock frequency:

```
I2CSensors.begin(I2C_SDA, I2C_SCL, 100000);
```
Finally, initialize object with your sensor address and your TwoWire object!

```
if (! mlx.begin(0x33, &I2CSensors)) {
 //   Serial.println("MLX90640 not found!"); Commented because we cannot use UART since the pins are used for MLX now!
    while (1) delay(10);
  }
```

### 2- Read the data from both RGB and thermal cameras continuously
### 3- Store the collected data into the SD card
