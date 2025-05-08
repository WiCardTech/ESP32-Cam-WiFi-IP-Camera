/*
 * WiFi IP Camera Arduino program For ESP32-Cam (Free Revision)
 * Rev 1.1
 *
 * Tested with esp32 board 3.2.0 and Ai Thinker ESP32-Cam
 *
 * Main File
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

#include <time.h>
#include <Arduino.h>
#include "esp_camera.h"

//#define OLD_MEMORY_CARD

#ifdef OLD_MEMORY_CARD
  #include <SD_MMC.h>
#else
  #include "SD.h"
  #define HSPI_MISO                 2
  #define HSPI_MOSI                 15
  #define HSPI_SCLK                 14
  #define HSPI_SS                   13

  #define HSPI_SPEED                20000000

  SPIClass * hspi = NULL;
#endif

#include "EEPROM.h"
#include "soc/rtc_cntl_reg.h"

#include "AC.h"
#include "CamSettings.h"

#ifdef UPDATE_IP
#include <HTTPClient.h>
#endif

#ifdef CONFIG_BROWNOUT_DET_LVL
#define BROWNOUT_DET_LVL CONFIG_BROWNOUT_DET_LVL
#else
#define BROWNOUT_DET_LVL 5
#endif //CONFIG_BROWNOUT_DET_LVL

#define CONFIG_BROWNOUT_DET_LVL_SEL_5 1

#ifdef UPDATE_IP
unsigned int  ipTimer;
HTTPClient    http;    //Declare object of class HTTPClient
#endif

bool espCamStreamInit()
{
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

  /* Size and Quality
  FRAMESIZE_96x96,    // 96x96
  FRAMESIZE_QQVGA,    // 160x120
  FRAMESIZE_QQVGA2,   // 128x160
  FRAMESIZE_QCIF,     // 176x144
  FRAMESIZE_HQVGA,    // 240x176
  FRAMESIZE_240x240,  // 240x240
  FRAMESIZE_QVGA,     // 320x240
  FRAMESIZE_CIF,      // 400x296
  FRAMESIZE_VGA,      // 640x480
  FRAMESIZE_SVGA,     // 800x600
  FRAMESIZE_XGA,      // 1024x768
  FRAMESIZE_SXGA,     // 1280x1024
  FRAMESIZE_UXGA,     // 1600x1200
  FRAMESIZE_QXGA,     // 2048*1536
  */

  if(ucVidSize == 6)
    config.frame_size = FRAMESIZE_QVGA;
  else if(ucVidSize == 7)
    config.frame_size = FRAMESIZE_CIF;
  else if(ucVidSize == 8)
    config.frame_size = FRAMESIZE_VGA;
  else //if(ucVidSize == 9)
    config.frame_size = FRAMESIZE_SVGA;

  if(ucVidQuality < 63)
    config.jpeg_quality = ucVidQuality;//10;//12 0-63 lower means higher quality
  else
    config.jpeg_quality = 63;
    
  config.fb_count = 1;

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
#ifdef LOG_ENABLE
    Serial.printf("Camera init failed with error 0x%x", err);
#endif
    digitalWrite(33, LOW);

    return false;
  }

  return true;
}

void loadCamSettings()
{
  ucVidSize = byte(EEPROM.read(EEP_VIDEO_SIZE));
  ucVidQuality = byte(EEPROM.read(EEP_VIDEO_QUALITY));
  bVidRec = byte(EEPROM.read(EEP_VIDEO_RECORD));
  ucVidTime = byte(EEPROM.read(EEP_VIDEO_TIME));
  ucVidMotion = byte(EEPROM.read(EEP_VIDEO_MOTION));
  //bVidFlash = byte(EEPROM.read(EEP_VIDEO_FLASH));
}

void setup()
{
  int i;

#ifdef UPDATE_IP
  ipTimer = 0;
#endif
  
  WiFi.mode(WIFI_OFF);
  pinMode(33, OUTPUT); //err led
  digitalWrite(33, LOW);
  pinMode(4, INPUT);

  //enable BOD to protect the Flash memory
  REG_WRITE(RTC_CNTL_BROWN_OUT_REG,
            RTC_CNTL_BROWN_OUT_ENA /* Enable BOD */
            | RTC_CNTL_BROWN_OUT_PD_RF_ENA /* Automatically power down RF */
            /* Reset timeout must be set to >1 even if BOR feature is not used */
            | (2 << RTC_CNTL_BROWN_OUT_RST_WAIT_S)
            | (BROWNOUT_DET_LVL << RTC_CNTL_DBROWN_OUT_THRES_S));
            
  delay(1000);

  DisableHotSpot = false;
  bPinMode = true;
  digitalWrite(33, HIGH);

  lastFrame = 0;
  currentFrame = 0;
  PicNumber = 0;
  
  Serial.begin(115200);
  Serial.setDebugOutput(false);
 
#ifdef LOG_ENABLE
  Serial.println(" ");
  Serial.println("** WiCardTech **");
  Serial.println("** WiFi IP Camera Arduino Firmware for ESP32-Cam (Rev 1.1) **");
  Serial.println("** https://wicard.net **");
#endif 
  //0-161 is allocated for AutoConnect (162 bytes)
  //162-167 is allocated for CamSettings (6 bytes)
  //168-255 is free for the program (88 bytes)
  EEPROM.begin(256);
  
  loadCamSettings();
  
  espCamStreamInit();

  delay(1000);
  
  //SD Init
#ifdef OLD_MEMORY_CARD
  if(!SD_MMC.begin("/sdcard", true))
#else
  hspi = new SPIClass(HSPI);

  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);

  if(!SD.begin(HSPI_SS, *hspi, HSPI_SPEED))
#endif
  {
#ifdef LOG_ENABLE
    Serial.println("MicroSD ram initialization failed!");
#endif
    digitalWrite(33, LOW);
  }
  else
  {
#ifdef LOG_ENABLE
    Serial.println("MicroSD ram initialization done.");
#endif

#ifdef OLD_MEMORY_CARD
    if (SD_MMC.exists("/status.sta"))
#else
    if (SD.exists("/status.sta"))
#endif
    {
#ifdef LOG_ENABLE
      Serial.println("status.sta exists.");
#endif

#ifdef OLD_MEMORY_CARD
      myFile = SD_MMC.open("/status.sta", FILE_READ);
#else
      myFile = SD.open("/status.sta", FILE_READ);
#endif
      myFile.read((uint8_t*)&PicNumber, 4);
      myFile.close();
    }
    else
    {
#ifdef LOG_ENABLE
      Serial.println("status.sta doesn't exist.");
      
      // open a new file and immediately close it:
      Serial.println("Creating status.sta...");
#endif

#ifdef OLD_MEMORY_CARD
      myFile = SD_MMC.open("/status.sta", FILE_WRITE);
#else
      myFile = SD.open("/status.sta", FILE_WRITE);
#endif
      //write
      PicNumber = 0x41414141;
      myFile.write((const uint8_t *)&PicNumber, 4);
      myFile.close();
    }
#ifdef OLD_MEMORY_CARD
    dir = SD_MMC.open("/");
#else
    dir = SD.open("/");
#endif
  }

  delay(1000);
  
  init_AC();
  
  // Configuring Internal Pages
  if(secureLink[0] == 0)
  {
    Server.on("/", rootPage);
    Server.on("/capture", capturePage);
    Server.on("/save", savePage);
    Server.on("/led", ledPage);
    Server.on("/memory", memoryPage);
  }
  else
  {
    Server.on("/"+((String)secureLink)+"/", rootPage);
    Server.on("/"+((String)secureLink)+"/capture", capturePage);
    Server.on("/"+((String)secureLink)+"/save", savePage);
    Server.on("/"+((String)secureLink)+"/led", ledPage);
    Server.on("/"+((String)secureLink)+"/memory", memoryPage);
  }
  
  delay(1000);
  
  Server.begin();
#ifdef LOG_ENABLE
  Serial.println("HTTP server started");
#endif

  capturing = 0;
  capturingFrame = 0;
  
#ifdef LOG_ENABLE
  Serial.println("Setup Done!");
#endif

  delay(1000);
}

//Main Loop!
void Delay10Sec()
{
  unsigned int i, j;
  
  for(i = 0; i < 10; i++)
  {
    if(capturingFrame)
    {
      for(j = 0; j < LOOP_10_S_CAPTURING; j++)
      {
        if(capturing)
          capturing--;
        
        Server.handleClient();
      }
    }
    else
    {
      for(j = 0; j < LOOP_10_S; j++)
      {
        if(capturing)
          capturing--;
        
        Server.handleClient();
      }
    }
    //delay(1);
      
    if(bVidRec && PicNumber != 0 && capturing == 0)
    {
      capture();
      
      if(capturingFrame == 0 && lastFrame != 0 && currentFrame != 0) //every 1 second
      {
        j = (currentFrame * CAP_DIFF_ACCURACY) / (currentFrame + lastFrame);
        
        if(j > (50+ucVidMotion) || j < (50-ucVidMotion))
          capturingFrame = ucVidTime;
          
        lastFrame = currentFrame;
        currentFrame = 0;
      }
    }
  }
}

void loop()
{
  Delay10Sec();
#ifdef LOG_ENABLE
  Serial.print("-");
#endif
  handle_AC();

#ifdef UPDATE_IP
  if(WiFi.status() == WL_CONNECTED)
  {
    if(ipTimer == 100)
    {
      if(capturingFrame == 0)
      {
        http.begin("http://YOUR-WEBSITE");
        ipTimer = http.GET();
#ifdef LOG_ENABLE
        Serial.println("Updating IP");
        if(ipTimer>0)
        {
          Serial.print("HTTP Response code: ");
          Serial.println(ipTimer);
          Serial.println(http.getString());
        }
        else
        {
          Serial.print("Error code: ");
          Serial.println(ipTimer);
        }
#endif
        ipTimer = 0;
        http.end();  //Close connection
      }
    }
    else
      ipTimer++;
  }
#endif
}
