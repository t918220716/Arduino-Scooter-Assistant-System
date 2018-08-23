#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

SoftwareSerial BT(10,11); //RX TX
SoftwareSerial mp3(8, 9); //RX TX

DFRobotDFPlayerMini myDFPlayer; 
void printDetail(uint8_t type, int value);
char code;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //BT.begin(9600);
  mp3.begin(9600);
  myDFPlayer.begin(mp3);
 /* Serial.println(F("DFRobot DFPlayer Mini Demo"));   // 印出DFRobot DFPl
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mp3))                 // 如果DFPlayer Mini回應不正確.
    {  //Use softwareSerial to communicate with mp3.   // 印出下面3行字串
        Serial.println(F("Unable to begin:"));                      // 然後程式卡死.
        Serial.println(F("1.Please recheck the connection!"));
        Serial.println(F("2.Please insert the SD card!"));
        while (true);                                     
    }*/
   //Serial.println(F("DFPlayer Mini online.")); 

   myDFPlayer.setTimeOut(500); // 設定通訊逾時為500ms
   myDFPlayer.volume(10);     // 設定音量, 範圍0~30
   myDFPlayer.EQ(DFPLAYER_EQ_NORMAL); 
   myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);  // 設定SD卡
   myDFPlayer.enableDAC();
   myDFPlayer.play(1); //開啟安全帽聲音
   //pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
/* char val;
  if (BT.available()) {
      val = BT.read();
      if (val == '1') {
      digitalWrite(LED_BUILTIN, HIGH);
      // 回應命令發送端，告知「已開燈」
      BT.println("LED ON");
    } else if (val == '0') {
      digitalWrite(LED_BUILTIN, LOW);
      // 回應命令發送端，告知「已關燈」
      BT.println("LED OFF");
    }    
    
  }*/
  BT.begin(9600);
  if(BT.available()){
    
    code = BT.read();
    //Serial.print(code);
    if(code == '1'){
       myDFPlayer.play(2);  
       delay(1000);//連接上藍芽聲音
    }else if(code == '2'){
      myDFPlayer.play(6);
      delay(1000);
    }else if(code == '3'){
      myDFPlayer.play(3);
      myDFPlayer.play(4);
      myDFPlayer.play(5);
       delay(1000);
    }else if(code == '4'){
      myDFPlayer.play(3);
      myDFPlayer.play(4);
      delay(1000);
    }else if(code == '5'){
      myDFPlayer.play(3);
      myDFPlayer.play(5);
      delay(1000);
    }else if(code == '6'){
      myDFPlayer.play(4);
      myDFPlayer.play(5);
      delay(1000);
    }else if(code == '7'){
      myDFPlayer.play(3);
      delay(1000);
    }else if(code == '8'){
      myDFPlayer.play(4);
      delay(1000);
    }else if(code == '9'){
      myDFPlayer.play(5);
      delay(1000);
    }
      //delay(5000);
  }
}
