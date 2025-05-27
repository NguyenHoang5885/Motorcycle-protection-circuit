#include <Adafruit_MPU6050.h>
#include <Wire.h>
#include <TinyGPS++.h> //NEO 6M
#include "Function_SEND.h"
#include "Function_READ.h"
#include "Function_MQTT.h"
#include "Global_Var.h"

//----------------------MQTT-----------------------------------------
// Kiểm tra nếu MQTT đã kết nối
bool isMQTTConnected() {
    return receivedBuffer.indexOf("+CMQTTCONNECT: 0,0") != -1;
}

// Gửi dữ liệu lên MQTT
void publishMQTT() {
  latitude = "";
  longitude = "";
  bool validCoordinates = false;

  // Đọc dữ liệu từ GPS
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
    if (gps.location.isUpdated()) {
      // Kiểm tra nếu GPS có tín hiệu đáng tin cậy
      if (gps.satellites.value() >= 4 && gps.hdop.value() < 200 && abs(gps.location.lat()) > 0.0001 && abs(gps.location.lng()) > 0.0001) {
        latitude = String(gps.location.lat(), 6);
        longitude = String(gps.location.lng(), 6);
        validCoordinates = true;
        Serial.print("Latitude= "); 
        Serial.print(latitude);
        Serial.print(" Longitude= "); 
        Serial.println(longitude);
        Serial.println("NEO 6M");
        Serial.print("Satellites= ");
        Serial.println(gps.satellites.value());
        Serial.print("HDOP= ");
        Serial.println(gps.hdop.value());
      }
    }
  }

  // Nếu không có tọa độ hợp lệ từ GPS, thử LBS
  if (!validCoordinates) {
    Serial.println("GPS không có tín hiệu, thử dùng LBS...");
    read_LBS();
    if (para_OK >= 0 || para_CLBS >= 0) {
      int pos4 = receivedBuffer.indexOf(',');
      int pos5 = receivedBuffer.indexOf(',', pos4 + 1);
      int pos6 = receivedBuffer.indexOf(',', pos5 + 1);
      Serial.println("LBSSSSSSSS"); 
      if (pos4 != -1 && pos5 != -1 && pos6 != -1) {
        latitude = receivedBuffer.substring(pos4 + 1, pos5);
        longitude = receivedBuffer.substring(pos5 + 1, pos6);
        validCoordinates = true;
        Serial.print("LBS Latitude= "); 
        Serial.print(latitude);
        Serial.print(" Longitude= "); 
        Serial.println(longitude);
      } else {
        Serial.println("LBS không trả về dữ liệu hợp lệ.");
      }
    } else {
      Serial.println("LBS không phản hồi thành công.");
    }
  }

  // Kiểm tra trước khi gửi dữ liệu lên MQTT
  float lat = latitude.toFloat();
  float longi = longitude.toFloat();
  if (!validCoordinates || latitude == "" || longitude == "" || abs(lat) < 0.0001 || abs(longi) < 0.0001) {
    Serial.println("Không có tọa độ hợp lệ để gửi lên MQTT.");
    return; // Thoát hàm nếu không có tọa độ hợp lệ
  }

  float elevation = 0;

  // Chuyển thành chuỗi với định dạng chính xác
  char lat_str[15];
  char lon_str[15];
  char ele_str[15];
  dtostrf(lat, 9, 6, lat_str);
  dtostrf(longi, 9, 6, lon_str);
  dtostrf(elevation, 4, 2, ele_str);

  // Tạo chuỗi JSON
  char jsonBuffer[100];
  sprintf(jsonBuffer, "{\"lat\":%s,\"lon\":%s,\"ele\":%s}", lat_str, lon_str, ele_str);

  // Gửi lên MQTT
  sendATCommand("AT+CMQTTTOPIC=0,27", 300);
  sendATCommand("AT+CMQTTPUB=0,\"<USERNAME>/feeds/<FEED_NAME>\",0,0", timeout);
  sendATCommand("AT+CMQTTPAYLOAD=0,47", 300);
  sendATCommand(jsonBuffer, 300);
  sendATCommand("AT+CMQTTPUB=0,1,60,0", 500);
}

void subscribe_buzzer_MQTT(){
  char topic[]= "<USERNAME>/feeds/<FEED_NAME_0>";
  int length= strlen(topic);

  char length_topic[50];
  sprintf(length_topic,"AT+CMQTTSUBTOPIC=0,%d,1",length);

  sendATCommand(length_topic,300);
  sendATCommand(topic,300);
  sendATCommand("AT+CMQTTSUB=0",300);
  Serial.println("Subscribed to topic: <USERNAME>/feeds/<FEED_NAME_0>");
}

void subscribe_Warning_MQTT(){
  char topic1[]= "<USERNAME>/feeds/<FEED_NAME_1>";
  int length1= strlen(topic1);

  char length_topic1[50];
  sprintf(length_topic1,"AT+CMQTTSUBTOPIC=0,%d,1",length1);

  sendATCommand(length_topic1,500);
  sendATCommand(topic1,500);
  sendATCommand("AT+CMQTTSUB=0",500);
  Serial.println("Subscribed to topic: <USERNAME>/feeds/<FEED_NAME_1>");
}
