#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include <TinyGPS++.h> //NEO 6M
#include "Function_SEND.h"
#include "Function_READ.h"
#include "Function_MQTT.h"
#include "Global_Var.h"

//----------------------SETUP-----------------------------------------
void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
  Serial1.begin(38400, SERIAL_8N1, GPS_RX, GPS_TX);//NEO 6M
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  pinMode(buzzer,OUTPUT);
  Wire.begin(18, 19);
  
  //MPU6050
  while (!Serial)
  vTaskDelay(10 / portTICK_PERIOD_MS); 
  Serial.println("Adafruit MPU6050 test!");
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
  }
  Serial.println("MPU6050 Found!");
  Serial.println("");
  vTaskDelay(100 / portTICK_PERIOD_MS);

  // Gửi lệnh AT để khởi động
  sendATCommand("AT",500);
  sendATCommand("ATE0",50);
  sendATCommand("AT+CGSN",50);
  sendATCommand("ATI",100);
  sendATCommand("AT+CPIN?",300);
  sendATCommand("AT+CIMI",300);
  sendATCommand("AT+CICCID",300);
  sendATCommand("AT+CSQ",300);
  sendATCommand("AT+CPSI?",300);
  sendATCommand("AT+CMQTTDISC=0,60",300);
  sendATCommand("AT+CMQTTREL=0",300);
  sendATCommand("AT+CMQTTSTOP",300);
  sendATCommand("AT+CMQTTSTART",300);
  sendATCommand("AT+CMQTTACCQ=0,\"karatedohoang1\",0",1000);
  sendATCommand("AT+CMQTTCONNECT=clientIndex,\"tcp://<BROKER_ADDRESS>:<PORT>\",<KEEP_ALIVE>,<CLEAN_SESSION>,\"<USERNAME>\",\"<PASSWORD>\"", timeout);
  mqttConnected = isMQTTConnected();  
  digitalWrite(buzzer, LOW);

  subscribe_buzzer_MQTT();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  subscribe_Warning_MQTT();
  
}
//----------------------LOOP-----------------------------------------
void loop() {
  if (Serial2.available()) {
    while (Serial2.available()) {
      char c = Serial2.read();
      receivedBuffer += c;
    }
    Serial.print("Received: ");
    Serial.println(receivedBuffer);

    read_Warning_MQTT();
    read_buzzer_MQTT();
    readmessenger();
    digitalWrite(buzzer, LOW);
  }

  // Nếu warning bật, gửi dữ liệu liên tục mỗi 5 giây
  if(save_x !=0 || save_y !=0 || save_z!=0){
    if(mqttConnected){
      Serial.println("-----------------------------------------------------------------------------------------------------");
      Serial.println("MQTT Connected! Publishing - Subscribe  data...");

      publishMQTT();
      vTaskDelay(500 / portTICK_PERIOD_MS);
    } 
    else{
      Serial.println("MQTT Disconnected! Trying to reconnect...");
      sendATCommand("AT+CMQTTCONNECT=0,\"tcp://<BROKER_ADDRESS>:<PORT>\",60,1,\"<USERNAME>\",\"<PASSWORD>\"", 1000);
      mqttConnected = isMQTTConnected();
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }

  mpu.getEvent(&a, &g, &temp); 
  
  //Nếu warning bật, Nếu người khác di chuyển xe, xe sẽ bật cảnh báo 
  if(save_x !=0 || save_y !=0 || save_z!=0 ){
    if(abs(a.acceleration.x - save_x)>2 || abs(a.acceleration.y - save_y)>2 || abs(a.acceleration.z - save_z)>2){
      digitalWrite(buzzer,1);
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      digitalWrite(buzzer,0);
      status_war=1;
    }
  }
  else{ //Nếu warning tắt, Nếu ngã xe sẽ nhắn tin vào số điện thoại người thân
    digitalWrite(buzzer,0);
    float angleX = atan2(a.acceleration.y, a.acceleration.z) * 180/3.14;
    float angleY = atan2(a.acceleration.x, a.acceleration.z) * 180/3.14;

    Serial.println(angleX);
    Serial.println(angleY);

    if(abs(angleX) > angle || abs(angleY) > angle){
      read_LBS();
      Serial.println("xe đi");
      Serial.println(angleX);
      Serial.println(angleY);
      if(para_OK >= 0 || para_CLBS >= 0 ){
        // Tìm vị trí các dấu phẩy
        int pos1 = receivedBuffer.indexOf(',');
        int pos2 = receivedBuffer.indexOf(',', pos1 + 1);
        int pos3 = receivedBuffer.indexOf(',', pos2 + 1);
        
        if(pos1 != -1 && pos2 != -1 && pos3 != -1){
          latitude  = receivedBuffer.substring(pos1 + 1, pos2);
          longitude = receivedBuffer.substring(pos2 + 1, pos3);
          
          //send to SMS
          Send_to_SMS();
          String command = "AT+CMGS=\"+84976973325\"";

          sendATCommand2(command.c_str(),1000);
          Serial2.println("Canh bao xe nga");
          delay(700);
          Serial2.write(0x1A);
          vTaskDelay(700 / portTICK_PERIOD_MS);

          sendATCommand2(command.c_str(),1000);
          Serial2.println(latitude + ", " + longitude);
          delay(700);
          Serial2.write(0x1A);
          vTaskDelay(700 / portTICK_PERIOD_MS);
          
          sendATCommand2(command.c_str(),1000);
          Serial2.println("https://www.google.com/maps");
          delay(700);
          Serial2.write(0x1A); // ngat tin nhan
          delay(700);
          Serial.println(latitude + ", " + longitude);
        }
      }
    }
  }

  receivedBuffer = "";
}