#ifndef Function_MQTT_H
#define Function_MQTT_H
#include <Arduino.h>

bool isMQTTConnected();
void publishMQTT();
void subscribe_buzzer_MQTT();
void subscribe_Warning_MQTT();


#endif
