/*
 * WiFi IP Camera Arduino program For ESP32-Cam (Free Revision)
 * Rev 1.1
 *
 * Tested with esp32 board 3.2.0 and Ai Thinker ESP32-Cam
 *
 * Memory handler File
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

void memoryPage()
{
  capturing = CAPTURING_DELAY;

  if(PicNumber == 0)
      Server.send(200, "text/html", "microSD Error!");
  else if(Server.arg("p") != NULL)
  {
    unsigned int i;

    if(memoryFileName[8] == '4')
    {
      unsigned char len[4];
  
      if(mFile.available())
      {
        mFile.read(len, 4);
        
        i = len[0];
        i|= len[1] << 8;
        i|= len[2] << 16;
        i|= len[3] << 24;

        if(i > 100000)
        {
          i = 0;
        }
        else
        {
          uiTotalJP4Len += i;
          
          if(uiTotalJP4Len > mFile.size())
            i = 0;
        }
      }
      else
        i = 0;
    }
    else
      i = mFile.size();

#ifdef LOG_ENABLE
    Serial.print("Reading ");
    Serial.println(i,DEC);
#endif

    if(i)
    {
      unsigned char content[4000];
      
      Server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
      Server.sendHeader("Pragma", "no-cache");
      Server.sendHeader("Expires", "-1");
      
      //Server.setContentLength(CONTENT_LENGTH_UNKNOWN);
      Server.setContentLength(i);
      
      Server.send(200, "image/jpeg", "");
      while(i > 4000)
      {
        mFile.read(content, 4000);
        Server.sendContent_P((const char *)content,4000);
        i -= 4000;
      }
      mFile.read(content, i);
      Server.sendContent_P((const char *)content,(size_t)i);

      //Server.sendContent(F("")); // transfer is done
      
      Server.client().stop();
    }
    else
    {
      mFile.seek(0);
      uiTotalJP4Len = 0;
      
      Server.send(200, "text/html", "0");
    }
  }
  else if(Server.arg("d") != NULL)
  {
    const String content = "<html><head><meta http-equiv='refresh' content='1; url=?n=5'></head><body>File Deleted!</body></html>";
    
#ifdef OLD_MEMORY_CARD
    SD_MMC.remove(memoryFileName);
#else
    SD.remove(memoryFileName);
#endif
    
    Server.send(200, "text/html", content);
  }
  else if(Server.arg("s") != NULL)
  {
    if(!mFile)
      Server.send(200, "text/html", "0");
    else
    {
      uiTotalJP4Len = 0;
      strcpy(memoryFileName, mFile.name());
      memoryFileName[9] = 0;
      Server.send(200, "text/html", &memoryFileName[1]);
    }
  }
  else
  {
    if(Server.arg("f") != NULL)
    {
      if((Server.arg("f")[5] == 'J' || Server.arg("f")[5] == 'j')
        && (Server.arg("f")[6] == 'P' || Server.arg("f")[6] == 'p')
        && (Server.arg("f")[7] == 'G' || Server.arg("f")[7] == 'g' || Server.arg("f")[7] == '4'))
      {
#ifdef OLD_MEMORY_CARD
        mFile = SD_MMC.open("/"+Server.arg("f"), FILE_READ);
#else
        mFile = SD.open("/"+Server.arg("f"), FILE_READ);
#endif

        if(!mFile)
          goto _not_found;

        memoryFileName[0] = '/';
        memoryFileName[1] = Server.arg("f")[0];
        memoryFileName[2] = Server.arg("f")[1];
        memoryFileName[3] = Server.arg("f")[2];
        memoryFileName[4] = Server.arg("f")[3];
        memoryFileName[5] = Server.arg("f")[4];
        memoryFileName[6] = Server.arg("f")[5];
        memoryFileName[7] = Server.arg("f")[6];
        memoryFileName[8] = Server.arg("f")[7];
        memoryFileName[9] = 0;
      }
      else
      {
_not_found:
        Server.send(200, "text/html", "<html><head></head><body>File Not Found!</body></html>");
        
        goto _function_end;
      }
    }
    else
    {
      if(Server.arg("n") == NULL)
#ifdef OLD_MEMORY_CARD
        dir = SD_MMC.open("/");
#else
        dir = SD.open("/");
#endif
      
      while(1)
      {
        mFile = dir.openNextFile();
        
        if(!mFile)
        {
          Server.send(200, "text/html", "<html><head></head><body>No More File!</body></html>");
          
          goto _function_end;
        }
        strcpy(memoryFileName, mFile.name());
        memoryFileName[9] = 0;
#ifdef LOG_ENABLE
        Serial.print("open ");
        Serial.println(memoryFileName);
#endif
        if((memoryFileName[6] == 'J' || memoryFileName[6] == 'j')
          && (memoryFileName[8] == '4' || memoryFileName[8] == 'G' || memoryFileName[8] == 'g'))
          break;
      }
    }
    const String content = "<html><head><title>Memory</title><style type='text/css'>body{background:#00E0F0;color:#777}.container{width:95%;margin:0 auto}.inner{background:#F9F9F9;padding:10px;box-shadow:0 10px 10px 0 rgba(0, 0, 0, 0.2), 0 5px 5px 0 rgba(0,0,0,0.24)}input[type='submit']{width:30%;background:#25a8e1;cursor:pointer;padding:10px;border:none;color:#FFF;font-size:15px}input[type='submit']:focus{background:#1974BB;border:none}.stream{max-width:700px;width:100%;margin-left:auto;margin-right:auto;display:block}</style></head><body><div class='container'><div class='inner'> <input type=\"submit\" value=\"NEXT\" onclick=\"window.location.href='?n=3'\" /> <input type=\"submit\" style=\"background:#F00\" value=\"REMOVE\" onclick=\"window.location.href='?d=0'\" /> <a id='lbtn'><a/> <br /> <input type='image' onclick='btnClick();' class='stream' src='' id='iimg'/></div></div> <script>var intval=setInterval(loadFrame,1000),play=0,status=0,link;function btnClick() {if(play==1) {intval=setInterval(loadFrame,200);play=2;} else if(play==2) {play=1;}} function loadFrame() {clearInterval(intval);if(window.XMLHttpRequest) {xhr=new XMLHttpRequest();} else {xhr=new ActiveXObject('Microsoft.XMLHTTP');} if(status==0) {xhr.open('GET','?s=1',true);} else {xhr.open('GET','?p=2',true);} xhr.responseType='arraybuffer';xhr.setRequestHeader(\"Connection\",\"keep-alive\");xhr.onreadystatechange=function() {if(xhr.readyState==4&&xhr.status==200) {var uInt8Array=new Uint8Array(xhr.response);var i=uInt8Array.length;var bstr=new Array(i);while(i--) {bstr[i]=String.fromCharCode(uInt8Array[i]);};if(status==0) {status=1;if(bstr[7]=='4'||bstr[7]=='G') {var l=document.getElementById('lbtn');if(bstr[7]=='4') {play=1;} link=bstr[0]+bstr[1]+bstr[2]+bstr[3]+bstr[4]+bstr[5]+bstr[6]+bstr[7];l.innerHTML=link;l.href='?f='+link;intval=setInterval(loadFrame,50);} else if(bstr[0]=='0') {alert('Memory Empty');} else {alert('Memory Error!');}} else {if(bstr[0]=='0') play=1;else {document.getElementById('iimg').src='data:image/png;base64,'+window.btoa(bstr.join(''));}if(play==2) {intval=setInterval(loadFrame,50);}} uInt8Array=null;binaryString=null;}};xhr.send();}</script> </body></html>";

    Server.send(200, "text/html", content);
  }

_function_end:
  ;
}

void generatePicName()
{
  jpegFileName[0] = '/';
  jpegFileName[1] = (char)((PicNumber >> 24) & 0xff);
  jpegFileName[2] = (char)((PicNumber >> 16) & 0xff);
  jpegFileName[3] = (char)((PicNumber >> 8) & 0xff);
  jpegFileName[4] = (char)((PicNumber >> 0) & 0xff);
  jpegFileName[5] = '.';
  jpegFileName[6] = 'j';
  jpegFileName[7] = 'p';
  jpegFileName[8] = 'g';
  jpegFileName[9] = 0;
  
  if(((PicNumber >> 0) & 0xff) == 0x5A)
  {
    PicNumber &= 0xFFFFFF00;
    PicNumber |= 0x00000041;
    
    if(((PicNumber >> 8) & 0xff) == 0x5A)
    {
      PicNumber &= 0xFFFF00FF;
      PicNumber |= 0x00004100;
      
      if(((PicNumber >> 16) & 0xff) == 0x5A)
      {
        PicNumber &= 0xFF00FFFF;
        PicNumber |= 0x00410000;
        
        if(((PicNumber >> 24) & 0xff) == 0x5A)
        {
#ifdef LOG_ENABLE
          Serial.println("Out of jpeg file...");
#endif
          PicNumber = 0x41414141;
        }
        else
          PicNumber += 0x1000000;
      }
      else
        PicNumber += 0x10000;
    }
    else
      PicNumber += 0x100;
  }
  else
    PicNumber +=1;

  pinMode(4, INPUT);
    
#ifdef OLD_MEMORY_CARD
  myFile = SD_MMC.open("/status.sta", FILE_WRITE);
#else
  myFile = SD.open("/status.sta", FILE_WRITE);
#endif
  //write
  myFile.write((const uint8_t *)&PicNumber, 4);
  myFile.close();
}
