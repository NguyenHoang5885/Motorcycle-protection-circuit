#ifndef Function_SEND_H
#define Function_SEND_H
#include <Arduino.h>

void sendATCommand(const char *command, int waitTime);
void sendATCommand2(const char *command2, int waitTime2);
void Send_to_SMS();


#endif