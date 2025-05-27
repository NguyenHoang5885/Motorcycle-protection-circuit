#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include <TinyGPS++.h> //NEO 6M
#include "Function_SEND.h"
#include "Function_READ.h"
#include "Function_MQTT.h"
#include "Global_Var.h"

//----------------------READ-----------------------------------------
void read_LBS(){
  receivedBuffer = "";
  sendATCommand("AT+CSQ",300); //check signal
  sendATCommand("AT+CREG?",300); // check network status
  sendATCommand("AT+CGREG?",300); //
  sendATCommand("AT+SIMEI=864424040019280",300);
  sendATCommand("AT+CLBS=1",4000);// LBS
    
  para_CLBS = receivedBuffer.indexOf("+CLBS:");
  para_OK = receivedBuffer.indexOf("OK");
    
  while (Serial2.available()) {
  char ch = Serial2.read();
  receivedBuffer += ch;
  }
}

void read_buzzer_MQTT(){
  if(receivedBuffer.indexOf("ON") != -1){
    Serial.println(" -----------------------buzzer HIGH-----------------xxxxxxxxxxxxxx");
    digitalWrite(buzzer,1);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    digitalWrite(buzzer,0);
  }
  else if (receivedBuffer.indexOf("OFF") != -1){
    Serial.println("------------------------buzzer LOW---------------------xxxxxxxxxxxxxx");
    digitalWrite(buzzer,0);
  }
}

void read_Warning_MQTT(){ 
  if(receivedBuffer.indexOf("BAT") != -1){
    Serial.println("-----------------------Protect ON ---------------------!!!!!!!!!!!!!!");
    mpu.getEvent(&a, &g, &temp); 
    save_x = a.acceleration.x;
    save_y = a.acceleration.y;
    save_z = a.acceleration.z;
    Serial.print("Save: ");
    Serial.print(save_x);
    Serial.print(" ");
    Serial.print(save_y);
    Serial.print(" ");
    Serial.print(save_z);
    Serial.println("");
    receivedBuffer = "";
  }
  else if (receivedBuffer.indexOf("TAT") != -1){
    Serial.println("----------------------Protect OFF---------------------!!!!!!!!!!!!!!");
    save_x = 0;
    save_y = 0;
    save_z = 0;
    digitalWrite(buzzer,0);
    status_war=0;
    receivedBuffer = "";
  }
}

void readmessenger(){
  // SMS
  para_CMTI= receivedBuffer.indexOf("+CMT:");
  para_gps = receivedBuffer.indexOf("gps");
  para_ring = receivedBuffer.indexOf("ring");

  if(para_CMTI >=0 ){ //đọc sđt
    int s1 = receivedBuffer.indexOf('"');
    int s2 = receivedBuffer.indexOf('"',s1+1);
    if(s1 != -1 && s2 != -1){
      sdt = receivedBuffer.substring(s1+1,s2);
      Serial.println("sdt: "+ sdt);
    }

    if(para_gps>=0){ // Nếu c = "gps" sẽ gửi lại tin nhắn có vị trí của xe
      read_LBS();

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
          String command = "AT+CMGS=\""+ sdt +"\"";
          sendATCommand2(command.c_str(),1000);
          Serial2.println(latitude + ", " + longitude);
          delay(700);
          Serial2.write(0x1A);
          vTaskDelay(700 / portTICK_PERIOD_MS);
         
          sendATCommand2(command.c_str(),1000);
          Serial2.println("https://www.google.com/maps");
          delay(700);
          Serial2.write(0x1A);
          delay(700);
          Serial.println(latitude + ", " + longitude);
        }
      }
    }
    else if(para_ring >=0 ){// Nếu c = "ring" sẽ bật chuông
      digitalWrite(buzzer, LOW);
      Send_to_SMS();
      String command = "AT+CMGS=\""+ sdt +"\"";
      sendATCommand2(command.c_str(),1000);
      Serial2.println("BUZZER TURN ON"); 
      delay(700);
      Serial2.write(0x1A);
      delay(700);
      digitalWrite(buzzer, HIGH);
      delay(5000);
      Serial.println("BUZZER TURN OFF");
      digitalWrite(buzzer, LOW);
  }
    else{
      //send to SMS
      Send_to_SMS();
      String command = "AT+CMGS=\""+ sdt +"\"";
      sendATCommand2(command.c_str(),1000);
      Serial2.println("Sai cau lenh : ring hoac gps"); 
      delay(500);
      Serial2.write(0x1A);
      delay(500);
    }
  }
}