#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <semphr.h>

#define KS103ADD_L  0x74
#define KS103ADD_R  0x75

SoftwareSerial PM2_5(10,11); //RX TX
SoftwareSerial BT(8,9);

SemaphoreHandle_t xBlueSemaphore;


void Sensor(void* pvParameters);
void BSM(void* pvParameters);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //PM2_5.begin(2400);
  
  //Serial.println("BT is ready!");
  

  Wire.begin();
  Wire.beginTransmission(KS103ADD_L); 
  Wire.write(byte(0x02));     
  Wire.write(0x72);   // ������誘    
  Wire.endTransmission();
  
  delay(1000);  
  
  Wire.beginTransmission(KS103ADD_R); 
  Wire.write(byte(0x02));     
  Wire.write(0x72);   // ������誘    
  Wire.endTransmission();
 
  if ( xBlueSemaphore == NULL )  // Check to confirm that the Serial Semaphore has not already been created.
  {
    xBlueSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( xBlueSemaphore ) != NULL )
      xSemaphoreGive( ( xBlueSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }

  
  xTaskCreate(BSM, "BSM_LEFT", 256, NULL, 2, NULL);
  xTaskCreate(Sensor, "Sensor", 256, NULL, 3, NULL);
 // xTaskCreate(MyIdleTask, "IdleTask", 100, NULL, 0, NULL);
  Serial.print("c3");
 
}

void loop() {
  // put your main code here, to run repeatedly:

}

void BSM(void* pvParameters){
  int Distance_L, Distance_R;
  Serial.println("c BSM!");
  for(;;){
    Distance_L = KS_103_L();
    Distance_R = KS_103_R();
    Serial.print(Distance_L);
    Serial.print(" L mm ");
     Serial.print(Distance_R);
    Serial.print(" R mm ");
    Serial.println(" ");
    if((Distance_L < 1000) || (Distance_R < 1000)){
      if ( xSemaphoreTake( xBlueSemaphore, ( TickType_t ) 5 ) == pdTRUE )
        {
           //Serial.println("cehck in bsm");
          BT.begin(9600); 
          BT.print('2');
          xSemaphoreGive( xBlueSemaphore ); // Now free or "Give" the Serial Port for others.
        }
       delay(2000);
    }
    
  }
}

void Sensor(void* pvParameters){
  
  TickType_t xLastWakeTime;  
  TickType_t xdelay= 60000 / portTICK_PERIOD_MS;
  xLastWakeTime = xTaskGetTickCount(); 
  
  int  pm_byte[7], z = 0, sum = 0, pm_count = 0, i = 0, data = 0;
  float pm_value = 0,uv_value = 0,light_value = 0;
  bool pm_mode = false;
  bool uv_mode = false;
  bool light_mode = false;
  
  while(1){
    pm_mode = false;
    uv_mode = false;
    light_mode = false;
    pm_value = 0;
    uv_value = 0;
    light_value = 0;
    pm_count = 0;
    vTaskDelayUntil( &xLastWakeTime,xdelay ); 
    
    PM2_5.begin(2400);
    //////pm2.5  start//////////
      i = 20000;
      while(i){
        //Serial.println(i);
         //Serial.print(i);
        //Serial.println(" c SENSOR!");
        if(PM2_5.available()>0){
          data = PM2_5.read();
          
          if(data == 170){
            z = 0;
            pm_byte[z] = data;
            }
           else{
            z++;
            pm_byte[z] = data;     
              }

          if(z == 6){
            sum = pm_byte[1] + pm_byte[2] + pm_byte[3] + pm_byte[4];
         
            if(pm_byte[5] == sum && pm_byte[6] == 255){
              pm_value += ((pm_byte[1] * 256.0 + pm_byte[2]) / 1024.0 * 2.5*200);
              pm_count = pm_count+1;
              //Serial.println(pm_count);
              }
            else{
                z = 0;
                PM2_5.flush();
                data = 0;
                for (int m = 0; m < 7; m++) {
                  pm_byte[m] = 0;
                }      
            }  
            z = 0;
          }
        }
        i--;   
      }
        
      pm_value = pm_value/pm_count;
      Serial.print(" pm data ");
       Serial.print( pm_value,6);
      Serial.println(" ");
      if(pm_value > 15){
        pm_mode = true;
      }
     //////pm2.5  end//////////
     //////uv&light start//////
     i = 10;
     while(i){
      uv_value += analogRead(A0);
      light_value += analogRead(A1);
      i--;
     }
     uv_value = uv_value/10;
     light_value = light_value/10;
     /* Serial.print(" uv_value ");
       Serial.print( uv_value,6);
      Serial.println(" ");/*
      Serial.print(" light_value ");
       Serial.print( light_value,6);
      Serial.println(" ");
*/     
     if(uv_value > 4){
      uv_mode = true;
      }
     if(light_value > 500){
      light_mode = true;
      }
     //////uv&light end///////

     if(pm_mode || uv_mode || light_mode){
        if ( xSemaphoreTake( xBlueSemaphore, ( TickType_t ) 5 ) == pdTRUE )
          {
            BT.begin(9600);
            if(pm_mode && uv_mode && light_mode)
                BT.print("3");
            else if(pm_mode && uv_mode)
                BT.print("4");
            else if(pm_mode && light_mode)
                BT.print("5");
            else if(uv_mode && light_mode)
                BT.print("6");
            else if(pm_mode)
                BT.print("7");
            else if(uv_mode)
                BT.print("8"); 
            else if(light_mode)
                BT.print("9");
            xSemaphoreGive( xBlueSemaphore ); // Now free or "Give" the Serial Port for others.
          }
     }
  }
}

int KS_103_L(){ 
  int distance = 0;
  Wire.beginTransmission(KS103ADD_L);
  Wire.write(byte(0x02));     
  Wire.write(0xbc);     //���挽蝵桐蛹10m 撣行萱摨西‘�
  Wire.endTransmission();     
  delay(1);                
  //Wire.beginTransmission(KS103ADD); 
  //Wire.write(byte(0x02));  
  //Wire.endTransmission();   
  Wire.requestFrom(KS103ADD_L, 2);  
  if(2 <= Wire.available())   
  {
    distance = Wire.read(); 
    distance =  distance << 8; 
    distance |= Wire.read();
  }
  return distance;
}

int KS_103_R(){ 
  int distance = 0;
  Wire.beginTransmission(KS103ADD_R);
  Wire.write(byte(0x02));     
  Wire.write(0xbc);     //���挽蝵桐蛹10m 撣行萱摨西‘�
  Wire.endTransmission();     
  delay(1);                
     
  Wire.requestFrom(KS103ADD_R, 2);  
  if(2 <= Wire.available())   
  {
    distance = Wire.read(); 
    distance =  distance << 8; 
    distance |= Wire.read();
  }
  return distance;
}

static void MyIdleTask(void* pvParameters)
{
  while(1)
  {
    Serial.println(F("Idle state"));
    delay(500);
  }
}

