#include <Adafruit_MLX90640.h>
#include "esp_camera.h"
#include <Wire.h>
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "SD.h"
#include <SPI.h>
#include <Arduino.h>
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h> 
//#include "Adafruit_VCNL4010.h"
 


#define I2C_SDA 1 // SDA Connected to GPIO 1
#define I2C_SCL 3 // SCL Connected to GPIO 3
TwoWire I2CSensors = TwoWire(0);
//Adafruit_VCNL4010 vcnl;
File myFile;
#define EEPROM_SIZE 2
// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int pictureNumber = 0;
int i;
String delim = "," ;
Adafruit_MLX90640 mlx;
float frame[32*24]; // buffer for full frame of temperatures
float t;
unsigned long myTime;


// uncomment *one* of the below
//#define PRINT_TEMPERATURES
#define PRINT_ASCIIART

void setup() {
  //esp_model_sleep_start();
 WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
// pinMode(4, INPUT);
 //digitalWrite(4, LOW);
 rtc_gpio_hold_dis(GPIO_NUM_4);
 // while (!Serial) delay(10);
 // Serial.begin(115200);
  delay(100);
  I2CSensors.begin(I2C_SDA, I2C_SCL, 100000);
//  Serial.println("Adafruit MLX90640 Simple Test");
  if (! mlx.begin(0x33, &I2CSensors)) {
 //   Serial.println("MLX90640 not found!");
    while (1) delay(10);
  }
//  Serial.println("Found Adafruit MLX90640");

//  Serial.print("Serial number: ");
//  Serial.print(mlx.serialNumber[0], HEX);
//  Serial.print(mlx.serialNumber[1], HEX);
//  Serial.println(mlx.serialNumber[2], HEX);

  
  //mlx.setMode(MLX90640_INTERLEAVED);
  mlx.setMode(MLX90640_CHESS);
 // Serial.print("Current mode: ");
  if (mlx.getMode() == MLX90640_CHESS) {
 //   Serial.println("Chess");
  } else {
 //   Serial.println("Interleave");    
  }

  mlx.setResolution(MLX90640_ADC_18BIT);
//  Serial.print("Current resolution: ");
  mlx90640_resolution_t res = mlx.getResolution();
//  switch (res) {
//    case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
//    case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
//    case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
//    case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
//  }

  mlx.setRefreshRate(MLX90640_4_HZ);
 // Serial.print("Current frame rate: ");
  mlx90640_refreshrate_t rate = mlx.getRefreshRate();
//  switch (rate) {
//    case MLX90640_0_5_HZ: Serial.println("0.5 Hz"); break;
//    case MLX90640_1_HZ: Serial.println("1 Hz"); break; 
//    case MLX90640_2_HZ: Serial.println("2 Hz"); break;
//    case MLX90640_4_HZ: Serial.println("4 Hz"); break;
//    case MLX90640_8_HZ: Serial.println("8 Hz"); break;
//    case MLX90640_16_HZ: Serial.println("16 Hz"); break;
//    case MLX90640_32_HZ: Serial.println("32 Hz"); break;
//    case MLX90640_64_HZ: Serial.println("64 Hz"); break;
//  }
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }


  //Serial.println("Starting SD Card");
  if(!SD_MMC.begin()){
 //   Serial.println("SD Card Mount Failed");
    return;
  }
  
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
 //   Serial.println("No SD Card attached");
    return;
  }
  
}

void loop() {
  //delay(10);
    myTime = millis();
   ++pictureNumber;
   camera_fb_t * fb = NULL;
  // Take Picture with Camera
  fb = esp_camera_fb_get();  
  String path = "/picture" + String(pictureNumber) +".jpg";
  fs::FS &fs = SD_MMC; 
  Serial.printf("Picture file name: %s\n", path.c_str());
  
  myFile = fs.open(path.c_str(), FILE_WRITE);
  myFile.write(fb->buf, fb->len); // payload (image), payload length
  //  Serial.printf("Saved file to path: %s\n", path.c_str());
  //myFile.close();
  esp_camera_fb_return(fb); 
   
  if (mlx.getFrame(frame) != 0) {
 //   Serial.println("Failed");
    return;
  }
   // fs::FS &fs = SD_MMC;
    myFile = fs.open("/test.txt", FILE_APPEND);
 // Serial.println();
  for (uint8_t h=0; h<24; h++) {
    for (uint8_t w=0; w<32; w++) {
       t = frame[h*32 + w];
     
    myFile.print(t + delim);
    //myFile.write(t);

}      
}
     
     myFile.println();
     myFile.print(myTime+ delim);
     myFile.close();
  }
