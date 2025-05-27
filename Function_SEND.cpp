#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include <TinyGPS++.h> //NEO 6M
#include "Function_SEND.h"
#include "Function_READ.h"
#include "Function_MQTT.h"
#include "Global_Var.h"

//--------------------------SEND-------------------------------------
void sendATCommand(const char *command, int waitTime) {
  Serial2.println(command);
  Serial.print("Sent: ");
  Serial.println(command);
  vTaskDelay(waitTime / portTICK_PERIOD_MS);
  receivedBuffer = "";  

  while (Serial2.available()) {
    char c = Serial2.read();
    receivedBuffer += c;
  }
  Serial.print("Response: ");
  Serial.println(receivedBuffer);

  // read Warning button----------------------------------------------------------------
  read_Warning_MQTT();
  mpu.getEvent(&a, &g, &temp); 
  if(status_war==1)
   {
     save_x = a.acceleration.x;
     save_y = a.acceleration.y;
     save_z = a.acceleration.z;
     status_war=2;
   }
  Serial.println("Giá trị ");
  Serial.println(a.acceleration.x - save_x);
  Serial.println(a.acceleration.y - save_y);
  Serial.println(a.acceleration.z - save_z);

  //chế độ bảo vệ lucs xe dừng---------------------------------------------------------
  if(save_x !=0 || save_y !=0 || save_z!=0 ){
    if(abs(a.acceleration.x - save_x)>2 || abs(a.acceleration.y - save_y)>2 || abs(a.acceleration.z - save_z)>2){
      digitalWrite(buzzer,1);
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      digitalWrite(buzzer,0);
      status_war=1;
    }
  }
  else{
    digitalWrite(buzzer,0);
  }
  read_buzzer_MQTT();
  readmessenger();
}

void sendATCommand2(const char *command2, int waitTime2) {
  Serial2.println(command2);
  Serial.print("Sent: ");
  Serial.println(command2);
       
  vTaskDelay(waitTime2 / portTICK_PERIOD_MS);
  receivedBuffer = "";
  while (Serial2.available()>0){
  char m = Serial2.read();
  receivedBuffer = receivedBuffer + m;
  }
  Serial.print("Response: ");
  Serial.println(receivedBuffer);
}

void Send_to_SMS() {
  sendATCommand("ATE0",300);
  sendATCommand("AT+CGSN",300);
  sendATCommand("ATI",300);
  sendATCommand("AT+CPIN?",300);
  sendATCommand("AT+CIMI",300);
  sendATCommand("AT+CICCID",300);
  sendATCommand("AT+CSQ",300);
  sendATCommand("AT+CPSI?",300);
  sendATCommand("AT+CMGF?",500);
  sendATCommand("AT+CSCS=\"GSM\"",1500);//"AT+CSCS=\"GSM\""
}