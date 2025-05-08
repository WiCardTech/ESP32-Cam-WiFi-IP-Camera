/*
 * WiFi IP Camera Arduino program For ESP32-Cam (Free Revision)
 * Rev 1.1
 *
 * Tested with esp32 board 3.2.0 and Ai Thinker ESP32-Cam
 *
 * WiFi config File
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

#include "AC.h"

void configPage_AC()
{
  capturing = CAPTURING_DELAY;
  
  int i;
  
  if(Server.arg("s") != NULL) //Cam settings
  {
    //size
    EEPROM.write(EEP_VIDEO_SIZE, (Server.arg("s")[0] - '0'));

    //quality
    EEPROM.write(EEP_VIDEO_QUALITY, (Server.arg("s")[0] - '0'));
    /*if(Server.arg("q")[0] == '0')
      i = 0;
    else if(Server.arg("q")[0] == '1')
      i = 10;
    else
      i = 32;
    EEPROM.write(EEP_VIDEO_QUALITY, i);*/

    //Recording
    if(Server.arg("r") != NULL)
      EEPROM.write(EEP_VIDEO_RECORD, 1);
    else
      EEPROM.write(EEP_VIDEO_RECORD, 0);

    //time (total frames)
    if(Server.arg("t")[0] == '5')
      i = 50;
    else if(Server.arg("t")[0] == '1')
      i = 100;
    else
      i = 200;
    EEPROM.write(EEP_VIDEO_TIME, i);
    
    //motion sensing
    i = Server.arg("p")[0] - '0';

    if(i < 3)
      i = 3;
    else if(i > 10)
      i = 10;
    EEPROM.write(EEP_VIDEO_MOTION, i);
      
    //white led
    if(Server.arg("f") != NULL)
      EEPROM.write(EEP_VIDEO_FLASH, 1);
    else
      EEPROM.write(EEP_VIDEO_FLASH, 0);
      
    EEPROM.commit();
    
    loadCamSettings();
  
    //const String content = "";
    PGM_P content = PSTR("<html><head><title>Settings</title></head><body>Settiings Saved! Please re-start the module.</body></html>");
    
    Server.send(200, "text/html", content);
  }
  else if(Server.arg("aps") != NULL)
  {
    for(i = EEP_AP_SSID; i < (EEP_AP_SSID+EEP_AP_SSID_LEN); i++)
    {
      APSsid[i-EEP_AP_SSID] = Server.arg("aps")[i-EEP_AP_SSID];
      EEPROM.write(i, APSsid[i-EEP_AP_SSID]);
    }

    if(Server.arg("app") != NULL)
    {
      for(i = EEP_AP_PASS; i < (EEP_AP_PASS+EEP_AP_PASS_LEN); i++)
      {
        APPassword[i-EEP_AP_PASS] = Server.arg("app")[i-EEP_AP_PASS];
        EEPROM.write(i, APPassword[i-EEP_AP_PASS]);
      }
    }

    if(Server.arg("l") != NULL)
    {
      for(i = EEP_SECURE; i < (EEP_SECURE+EEP_SECURE_LEN); i++)
      {
        if(Server.arg("l")[i-EEP_SECURE] == 0)
        {
          secureLink[i-EEP_SECURE] = 0;
          
          EEPROM.write(i, 0);

          break;
        }
        else if((Server.arg("l")[i-EEP_SECURE] >= '0' && Server.arg("l")[i-EEP_SECURE] <= '9')
          || (Server.arg("l")[i-EEP_SECURE] >= 'A' && Server.arg("l")[i-EEP_SECURE] <= 'Z')
          || (Server.arg("l")[i-EEP_SECURE] >= 'a' && Server.arg("l")[i-EEP_SECURE] <= 'z'))
        {
          secureLink[i-EEP_SECURE] = Server.arg("l")[i-EEP_SECURE];
          
          EEPROM.write(i, secureLink[i-EEP_SECURE]);
        }
        else
        {
          secureLink[0] = 0;
          
          EEPROM.write(EEP_SECURE, 0);
  
          break;
        }
      }
    }

    if(Server.arg("hap") != NULL)
      i = 1;
    else
      i = 0;
      
    if(Server.arg("dap") != NULL)
      i |= 2;

    EEPROM.write(EEP_AC_CONFIG, (i&0xff));
    
    EEPROM.commit();

    //const String content = "";
    PGM_P content = PSTR("<html><head><title>AutoConnect</title></head><body>Hot Spot Saved! Please re-start the module.</body></html>");
    
    Server.send(200, "text/html", content);
  }
  else if(Server.arg("sts") != NULL)
  {
    for(i = EEP_ST_SSID; i < (EEP_ST_SSID+EEP_ST_SSID_LEN); i++)
    {
      STSsid[i-EEP_ST_SSID] = Server.arg("sts")[i-EEP_ST_SSID];
      EEPROM.write(i, STSsid[i-EEP_ST_SSID]);
    }
      
    for(i = EEP_ST_PASS; i < (EEP_ST_PASS+EEP_ST_PASS_LEN); i++)
    {
      STPassword[i-EEP_ST_PASS] = Server.arg("stp")[i-EEP_ST_PASS];
      EEPROM.write(i, STPassword[i-EEP_ST_PASS]);
    }
    
    EEPROM.commit();

#ifdef LOG_ENABLE
    Serial.print("ST SSID:");
    Serial.println(STSsid);
      
    Serial.print("ST PASS:");
    Serial.println(STPassword);
#endif
    //const String content = "
    PGM_P content = PSTR("<html><head><meta http-equiv='refresh' content='100; url=/config'><title>Connecting...</title></head><body>Please Wait...</body></html>");
    
    Server.send(200, "text/html", content);
  }
  else
  {
    String content = "<html><head><title>ESP32-CAM Configuration</title><style type='text/css'>body{background:#00E0F0;color:#777}.container{max-width:400px;width:100%;margin:0 auto}.inner{background:#F9F9F9;padding:30px 25px;margin:70px 0;box-shadow:0 0 20px 0 rgba(0, 0, 0, 0.2), 0 5px 5px 0 rgba(0,0,0,0.24)}fieldset{border:medium none;margin:0 0 10px;min-width:100%;width:100%;padding:0}input{width:100%;margin:5px 0 10px;padding:10px}input[type='text']{border:1px solid #ccc;background:#FFF}input[type='text']:focus{outline:0;border:1px solid #25a8e1}input[type='submit']{background:#25a8e1;cursor:pointer;border:none;color:#FFF;font-size:15px}input[type='submit']:focus{background:#1974BB;border:none}.switch{position:relative;display:inline-block;width:60px;height:34px}.switch input{opacity:0;width:0;height:0;margin-top:8px}.slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;border-radius:34px;background-color:#ccc;-webkit-transition: .4s;transition: .4s}.slider:before{position:absolute;content:'';height:26px;width:26px;left:4px;bottom:4px;border-radius:50%;background-color:white;-webkit-transition: .4s;transition: .4s}input:checked+.slider{background-color:#25a8e1}input:checked+.slider:before{-webkit-transform:translateX(26px);-ms-transform:translateX(26px);transform:translateX(26px)}.ap{border:1px solid #ccc;padding:5px;background:#9ff;margin:0 0 10px}.rssi{padding:3px;background:#9dd;margin-left:10px}</style></head><body><div class='container'><div class='inner'><div style='text-align:center'> <a href='https://store.wicard.net'><img src='data:image/png;base64,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' /></a></div><form method='get' action='' id='f2'><h2>Modem Configuration</h2><fieldset>Connected to:";

    if(WiFi.status() == WL_CONNECTED)
    {
      content += STSsid;
      content += " [";
      content += WiFi.RSSI();
      content += "]";
    }
    else
      content +="-";

    content += "</fieldset><fieldset>IP: ";
    
    if(WiFi.status() == WL_CONNECTED)
      content += WiFi.localIP().toString();
    else
      content +="-";

    content += "</fieldset><h3>Available APs</h3>";
    
#ifdef LOG_ENABLE
    Serial.println("start scan");
#endif
    
    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();

#ifdef LOG_ENABLE
    Serial.println("scan done");
#endif
    
    if(n != 0)
    {
#ifdef LOG_ENABLE
      Serial.print(n);
      Serial.println(" networks found");
#endif
      
      for(int i = 0; i < n; ++i)
      {
        content += "<div class='ap'><b onclick='ap(this.innerHTML);'>" + WiFi.SSID(i) + "</b><b class='rssi'>" + WiFi.RSSI(i) + "</b></div>";
        // Print SSID and RSSI for each network found
#ifdef LOG_ENABLE
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.println(")");
        delay(10);
#endif
      }
    }
#ifdef LOG_ENABLE
    else
    {
      Serial.println("no networks found");
    }
#endif
    
    byte mac[6];

    WiFi.macAddress(mac);

#ifdef LOG_ENABLE
    Serial.print("MAC: ");
    Serial.print(mac[0],HEX);
    Serial.print(":");
    Serial.print(mac[1],HEX);
    Serial.print(":");
    Serial.print(mac[2],HEX);
    Serial.print(":");
    Serial.print(mac[3],HEX);
    Serial.print(":");
    Serial.print(mac[4],HEX);
    Serial.print(":");
    Serial.println(mac[5],HEX);
#endif
    
    content += "<fieldset>MAC: "+String(mac[0],HEX)+":"+String(mac[1],HEX)+":"+String(mac[2],HEX)+":"+String(mac[3],HEX)+":"+String(mac[4],HEX)+":"+String(mac[5],HEX)+"</fieldset>";
    
    content += "<fieldset>SSID:<input id='ssid' type='text' name='sts' value='";

    content += STSsid;

    content += "'/></fieldset><fieldset>PASSWORD:<input type='text' name='stp' /></fieldset><fieldset><input type='submit' value='Save' /></fieldset></form><form method='get' action='' id='f1'><h2>Device HotSpot Configuration</h2><fieldset>SSID:<input type='text' name='aps' value='";

    content += APSsid;

    content += "'/></fieldset><fieldset>PASSWORD:<input type='text' name='app' value='";

    content += APPassword;
    
    content += "' /></fieldset><fieldset>SECURE LINK:192.168.x.x/<input type='text' name='l' value='";
    
    content += secureLink;
    
    content += "' /></fieldset><span style='margin-top:-45px;'><label class='switch'> <input type='checkbox' name='hap' ";

    if(byte(EEPROM.read(EEP_AC_CONFIG)) & 0x01)
      content += "checked";

    content += "><span class='slider round'></span></label></span>Hidden HotSpot<br /><br /><span style='margin-top:-45px;'><label class='switch'> <input type='checkbox' name='dap' ";

    if(byte(EEPROM.read(EEP_AC_CONFIG)) & 0x02)
      content += "checked";

    content += "><span class='slider round'></span></label></span>Disable HotSpot when is connected to the modem<fieldset><input type='submit' value='Save' /></fieldset></form>";

    //Cam Settings
    content += "<form method='get' action='' id='f3'><h2>Video Streaming Settings</h2><fieldset>Size<select size='1' name='s'>";

    n = byte(EEPROM.read(EEP_VIDEO_SIZE));
    content += "<option value='6'";
    if(n == 6)
      content += " selected";
    content += ">QVGA 320x240</option>";
    
    content += "<option value='7'";
    if(n == 7)
      content += " selected";
    content += ">CIF 400x296</option>";
    
    content += "<option value='8'";
    if(n == 8)
      content += " selected";
    content += ">VGA 640x480</option>";
    
    content += "<option value='9'";
    if(n == 9)
      content += " selected";
    content += ">SVGA 800x600</option>";

    content += "</select></fieldset><fieldset>Quality<select size='1' name='q'>";
    
    n = byte(EEPROM.read(EEP_VIDEO_QUALITY));
    content += "<option value='5'";
    if(n == 0)
      content += " selected";
    content += ">High</option>";
    
    content += "<option value='10'";
    if(n == 10)
      content += " selected";
    content += ">Medium</option>";
    
    content += "<option value='15'";
    if(n == 32)
      content += " selected";
    content += ">Low</option>";

    content += "</select></fieldset><h2>Video Recording Settings</h2><fieldset><span style='margin-top:-45px;'><label class='switch'><input type='checkbox' name='r'";
    if(byte(EEPROM.read(EEP_VIDEO_RECORD)))
      content += " checked";
      
    content += "><span class='slider round'></span></label></span>Enable Recording</fieldset><fieldset>Record Time<select size='1' name='t'>";

    n = byte(EEPROM.read(EEP_VIDEO_TIME));
    content += "<option value='5'";
    if(n == 50)
      content += " selected";
    content += ">50 Frames</option>";
    
    content += "<option value='1'";
    if(n == 100)
      content += " selected";
    content += ">100 Frames</option>";
    
    content += "<option value='2'";
    if(n == 200)
      content += " selected";
    content += ">200 Frames</option>";

    content += "</select></fieldset><fieldset>Motion Sensivity<select size='1' name='p'>";

    n = byte(EEPROM.read(EEP_VIDEO_MOTION));
    content += "<option value='3'";
    if(n == 3)
      content += " selected";
    content += ">8</option>";
    
    content += "<option value='4'";
    if(n == 4)
      content += " selected";
    content += ">7</option>";
    
    content += "<option value='5'";
    if(n == 5)
      content += " selected";
    content += ">6</option>";
    
    content += "<option value='6'";
    if(n == 6)
      content += " selected";
    content += ">5</option>";
    
    content += "<option value='7'";
    if(n == 7)
      content += " selected";
    content += ">4</option>";
    
    content += "<option value='8'";
    if(n == 8)
      content += " selected";
    content += ">3</option>";
    
    content += "<option value='9'";
    if(n == 9)
      content += " selected";
    content += ">2</option>";
    
    content += "<option value='10'";
    if(n == 10)
      content += " selected";
    content += ">1</option>";

    content += "</select></fieldset>";
    /*content += "<fieldset><span style='margin-top:-45px;'><label class='switch'><input type='checkbox' name='f'";
    if(byte(EEPROM.read(EEP_VIDEO_FLASH)))
      content += " checked";
      
    content += "><span class='slider round'></span></label></span>Flash Light</fieldset>*/
    content += "<fieldset><input type='submit' value='Save' /></fieldset></form>";

    content += "</div></div> <script>function ap(name) {document.getElementById('ssid').value=name;}</script> </body></html>";
    
    Server.send(200, "text/html", content);
  }
}

void reset_AC()
{
  capturing = CAPTURING_DELAY;
  
  int i;

#ifdef LOG_ENABLE
  Serial.print("EE Reset...");
#endif
  
  STSsid[0] = 0;
  STPassword[0] = 0;
  APSsid[0] = 'W';
  APSsid[1] = 'i';
  APSsid[2] = 'C';
  APSsid[3] = 'a';
  APSsid[4] = 'r';
  APSsid[5] = 'd';
  APSsid[6] = 'I';
  APSsid[7] = 'P';
  APSsid[8] = 'C';
  APSsid[9] = 'a';
  APSsid[10] = 'm';
  APSsid[11] = 0;
  
  APPassword[0] = '1';
  APPassword[1] = '2';
  APPassword[2] = '3';
  APPassword[3] = '4';
  APPassword[4] = '5';
  APPassword[5] = '6';
  APPassword[6] = '7';
  APPassword[7] = '8';
  APPassword[8] = 0;

  EEPROM.write(EEP_AC_KEY, MEM_SAVED);
  EEPROM.write(EEP_ST_SSID, 0);
  EEPROM.write(EEP_ST_PASS, 0);
  
  for(i = EEP_AP_SSID; i < (EEP_AP_SSID+EEP_AP_SSID_LEN); i++)
    EEPROM.write(i, APSsid[i-EEP_AP_SSID]);
    
  for(i = EEP_AP_PASS; i < (EEP_AP_PASS+EEP_AP_PASS_LEN); i++)
    EEPROM.write(i, APPassword[i-EEP_AP_PASS]);
  
  EEPROM.write(EEP_SECURE, 0);
  
  //EEPROM.write(EEP_AC_CONFIG, 0);
  EEPROM.write(EEP_AC_CONFIG, 2);

  //Default Cam Settings
  //size
  EEPROM.write(EEP_VIDEO_SIZE, 9);

  //quality
  EEPROM.write(EEP_VIDEO_QUALITY, 5);

  //Recording
  EEPROM.write(EEP_VIDEO_RECORD, 0);

  //time (total frames)
  EEPROM.write(EEP_VIDEO_TIME, 50);
    
  //motion sensing
  EEPROM.write(EEP_VIDEO_MOTION, 5);
      
  //white led
  EEPROM.write(EEP_VIDEO_FLASH, 0);
   
  EEPROM.commit();

  loadCamSettings();
}

void init_AC()
{
  capturing = CAPTURING_DELAY;
  
#ifdef LOG_ENABLE
  Serial.print("Autoconnect Init!");
#endif
  
  int i;
  
  DisableHotSpot = false;
  
  //EEPROM.begin(256);
  
  if(byte(EEPROM.read(EEP_AC_KEY)) == MEM_SAVED)
  {
#ifdef LOG_ENABLE
    Serial.print("EE OK!");
#endif
    for(i = EEP_SECURE; i < (EEP_SECURE+EEP_SECURE_LEN); i++)
      secureLink[i-EEP_SECURE] = EEPROM.read(i);
    
    for(i = EEP_ST_SSID; i < (EEP_ST_SSID+EEP_ST_SSID_LEN); i++)
      STSsid[i-EEP_ST_SSID] = EEPROM.read(i);
      
    for(i = EEP_ST_PASS; i < (EEP_ST_PASS+EEP_ST_PASS_LEN); i++)
      STPassword[i-EEP_ST_PASS] = EEPROM.read(i);
      
    for(i = EEP_AP_SSID; i < (EEP_AP_SSID+EEP_AP_SSID_LEN); i++)
      APSsid[i-EEP_AP_SSID] = EEPROM.read(i);
      
    for(i = EEP_AP_PASS; i < (EEP_AP_PASS+EEP_AP_PASS_LEN); i++)
      APPassword[i-EEP_AP_PASS] = EEPROM.read(i);
    
    if(STSsid[0] != 0)
    {
      if(byte(EEPROM.read(EEP_AC_CONFIG)) & 0x02)
        WiFi.mode(WIFI_STA);
      else
        WiFi.mode(WIFI_AP_STA);

      WiFi.begin(STSsid, STPassword);

#ifdef LOG_ENABLE
      Serial.print("Connecting To AP:");
      Serial.print(STSsid);
#endif
    }
  }
  else
  {
#ifdef LOG_ENABLE
    Serial.print("EE Init...");
#endif

    reset_AC();
  }

  if(secureLink[0] == 0)
    Server.on("/config", configPage_AC);
  else
    Server.on("/"+((String)secureLink)+"/config", configPage_AC);
  
  if(byte(EEPROM.read(EEP_AC_CONFIG)) & 0x01)
    WiFi.softAP(APSsid, APPassword, 1, true);
  else
    WiFi.softAP(APSsid, APPassword);

#ifdef LOG_ENABLE
  Serial.print("Autoconnect Init Done!");
#endif
}

void handle_AC()
{
  if(WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    
    WiFi.mode(WIFI_AP);
    
    delay(200);
    
    if(STSsid[0] != 0)
    {
#ifdef LOG_ENABLE
      Serial.println("Reconnecting");
#endif
      Delay10Sec();
      Delay10Sec();
      Delay10Sec();
      //Delay10Sec();
      //Delay10Sec();
      //Delay10Sec();
#ifdef LOG_ENABLE
      Serial.print("Connecting To AP:");
      Serial.print(STSsid);
#endif
      delay(200);
      WiFi.mode(WIFI_AP_STA);
      delay(200);
      WiFi.begin(STSsid, STPassword);
      delay(200);

      Delay10Sec();
      Delay10Sec();
    }
    DisableHotSpot = false;
  }
  else if(DisableHotSpot == false)
  {
#ifdef LOG_ENABLE
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
#endif

    if(byte(EEPROM.read(EEP_AC_CONFIG)) & 0x02)
      WiFi.mode(WIFI_STA);

    delay(200);

    DisableHotSpot = true;
  }
}
