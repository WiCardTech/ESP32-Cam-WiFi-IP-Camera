/*
 * WiFi IP Camera Arduino program For ESP32-Cam (Free Revision)
 * Rev 1.1
 *
 * Tested with esp32 board 3.2.0 and Ai Thinker ESP32-Cam
 *
 * Web UI handler File
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

void rootPage()
{
  capturing = CAPTURING_DELAY;
  
  //PGM_P content = PSTR("");
  const String content = "<html><head><title>ESP32CAM-WiCard.Net</title></head>"
    "<body><input type='image' onclick='btnClick();' style='max-width:700px;width:100%;margin-left:auto;margin-right:auto;display:block;' src='' id='iimg'/><script>"
    "var intval = setInterval(loadFrame,1000),save=0,led=0,cli=0;"
    "function btnClick(){if(cli==0){cli=5;}else{led=1;cli=0;}}"
    "function loadFrame(){if(cli==1){if(led==0)save=1;cli=0;}else if(cli!=0)cli--;"
    "clearInterval(intval);"
    "if (window.XMLHttpRequest)"
    "xhr = new XMLHttpRequest(); else xhr = new ActiveXObject('Microsoft.XMLHTTP');"
    "if(led == 1){save=0;led = 2;xhr.open('GET', 'led', true);}else if(save == 1){save=2;xhr.open('GET', 'save', true);}else xhr.open('GET', 'capture', true);"
    "xhr.responseType = 'arraybuffer';"
    "xhr.setRequestHeader('Connection', 'keep-alive');"
    "xhr.onreadystatechange = function(){"
    "if (xhr.readyState == 4 && xhr.status == 200){"
    "if(led==2){led=0;intval = setInterval(loadFrame,2000);}else if(save == 2){save = 0;intval = setInterval(loadFrame,4000);} else {"
    "var uInt8Array = new Uint8Array(xhr.response);"
    "var i = uInt8Array.length;"
    "var binaryString = new Array(i);"
    "while (i--){binaryString[i] = String.fromCharCode(uInt8Array[i]);};"
    "document.getElementById(\"iimg\").src=\"data:image/png;base64,\" + window.btoa(binaryString.join(''));"
    "uInt8Array = null;binaryString = null;"
    "intval = setInterval(loadFrame,50);}}};"
    "xhr.send();}"
    "</script></body></html>";

  Server.send(200, "text/html", content);
}

void savePage()
{
  capturing = CAPTURING_DELAY;
  
  camera_fb_t * fb = NULL;

  fb = esp_camera_fb_get();
  if (!fb)
  {
#ifdef LOG_ENABLE
    Serial.println("Camera save failed");
#endif
    digitalWrite(33, LOW);
    
    Server.send(200, "text/html", "0");
  }
  else
  {
    // open a new file and immediately close it:
#ifdef LOG_ENABLE
    Serial.println("Creating jpeg file...");
#endif
    generatePicName();
    
#ifdef OLD_MEMORY_CARD
    myFile = SD_MMC.open((const char *)jpegFileName, FILE_WRITE);
#else
    myFile = SD.open((const char *)jpegFileName, FILE_WRITE);
#endif
    //write
    myFile.write(fb->buf, fb->len);
    myFile.close();
    
#ifdef OLD_MEMORY_CARD
    myFile = SD_MMC.open("/status.sta", FILE_WRITE);
#else
    myFile = SD.open("/status.sta", FILE_WRITE);
#endif
    //write
    myFile.write((const uint8_t *)&PicNumber, 4);
    myFile.close();
    
    if(bPinMode == false)
    {
      pinMode(4, OUTPUT);
      digitalWrite(4, HIGH);
    }
    
    Server.send(200, "text/html", "1");
  }
  
  esp_camera_fb_return(fb);
}

void ledPage()
{
  capturing = CAPTURING_DELAY;
  lastFrame = 0;
  currentFrame = 0;
  
  if(bPinMode)
  {
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);
    bPinMode = false;
    
    Server.send(200, "text/html", "1");
  }
  else
  {
    pinMode(4, INPUT);
    bPinMode = true;
    
    Server.send(200, "text/html", "0");
  }
}
