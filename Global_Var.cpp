#include "Global_Var.h"

// GPS
TinyGPSPlus gps;

// MPU
float save_x = 0, save_y = 0, save_z = 0;
float angle= 60.0;
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;

// Các biến toàn cục khác
String receivedBuffer = "";
bool mqttConnected = false;
int para_gps = 0, para_ring = 0, para_CMTI = 0, para_OK = 0, para_CLBS = 0;
String sdt = "";
String latitude = "";
String longitude = "";
uint8_t status_war=0;