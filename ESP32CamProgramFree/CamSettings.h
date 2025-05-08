/*
 * WiFi IP Camera Arduino program For ESP32-Cam (Free Revision)
 * Rev 1.1
 *
 * Tested with esp32 board 3.2.0 and Ai Thinker ESP32-Cam
 *
 * Camera configuration header File
 *
 * Author: M. Mahdi K. Kanan (mvtdesign@gmail.com)
 * WiCardTech Engineering Group (https://wicard.net)
 *
 * All of the revisions:
 * ESP32 WiFi Microphone (12bit@8KHz - Free Revision) --> https://wicard.net/projects/Arduino/ESP32-ESP8266/ESP32%20WiFi%20Microphone%20free%20arduino%20source%20code
 * ESP8266 WiFi Microphone (10bit@8KHz - Free Revision) --> https://wicard.net/projects/Arduino/ESP32-ESP8266/ESP8266%20WiFi%20Microphone%20free%20arduino%20source%20code
 * ESP32 WiFi Microphone and Recorder --> https://wicard.net/projects/Arduino/ESP32-ESP8266/esp32-voice-recorder
 * ESP8266 WiFi Microphone and Recorder --> https://wicard.net/projects/Arduino/ESP32-ESP8266/nodemcu-voice-recorder-esp8266
 * ESP32-Cam IP-Camera (Free Revision) --> https://wicard.net/projects/Arduino/ESP32-ESP8266/esp32cam-ipcamera-bin
 * ESP32-Cam HD DashCam --> https://wicard.net/projects/Arduino/ESP32-ESP8266/esp32-cam-DashCam
 * ESP32-Cam CCTV --> https://wicard.net/projects/Arduino/ESP32-ESP8266/ESP32-CAM%20CCTV%20IP%20Camera
 * ESP32-Cam Camera and Microphone --> https://wicard.net/projects/Arduino/ESP32-ESP8266/spycam-video-audio-esp32cam
 *
*/

#ifndef camsettings_H_
#define camsettings_H_

#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

//#define CAP_DIFF_ACCURACY 5
//#define CAP_DIFF_PERCENT  5
#define CAP_DIFF_ACCURACY 110

//EEP Memory
//162-167 is allocated for CamSettings (6 bytes)
//168-255 is free for the program (88 bytes)
#define EEP_VIDEO_SIZE    162
#define EEP_VIDEO_QUALITY 163
#define EEP_VIDEO_RECORD  164
#define EEP_VIDEO_TIME    165
#define EEP_VIDEO_MOTION  166
#define EEP_VIDEO_FLASH   167

// This project was tested with the AI Thinker ESP32-CAM
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

#define CAPTURING_DELAY   10000

//Global Vars...
bool             bVidRec;
//bool             bVidFlash;
bool             bPinMode;

unsigned char    ucVidSize;
unsigned char    ucVidQuality;
unsigned char    ucVidTime;
unsigned char    ucVidMotion;

char             jpegFileName[10];
char             memoryFileName[20];

unsigned int     lastFrame;
unsigned int     currentFrame;
unsigned int     capturing;
unsigned int     capturingFrame;
unsigned int     uiTotalJP4Len;
unsigned int     PicNumber;

File             mFile;
File             myFile;
File             dir;

camera_config_t  config;

#endif // INCLUDED
