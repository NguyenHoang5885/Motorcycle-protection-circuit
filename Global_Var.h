#ifndef Global_Var_H
#define Global_Var_H
#include <Arduino.h>
#include <TinyGPS++.h>
#include <Adafruit_MPU6050.h>
#include <Wire.h>

// GPS
#define GPS_RX 27
#define GPS_TX 26
extern TinyGPSPlus gps;

// UART2 & button
#define RX2 16  // Chân RX của UART2
#define TX2 17  // Chân TX của UART2
#define buzzer 4

// MPU
extern float save_x, save_y, save_z;
extern float angle;
extern Adafruit_MPU6050 mpu;
extern sensors_event_t a, g, temp;

// Các biến toàn cục khác
extern String receivedBuffer;
extern bool mqttConnected;
extern int para_gps, para_ring, para_CMTI, para_OK, para_CLBS;
extern String sdt;
extern String latitude;
extern String longitude;
extern uint8_t status_war;
#endif