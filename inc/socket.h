//
// Created by adam slaymark on 20/02/2024.
//

#ifndef HEARING_TEST_BUTTON_SOCKET_H
#define HEARING_TEST_BUTTON_SOCKET_H

#include <stdint.h>

int initialiseSocket(char* server_ip, int16_t PORT);
void closeSocket();
void sendDataSocket(char payload);

#endif //HEARING_TEST_BUTTON_SOCKET_H
