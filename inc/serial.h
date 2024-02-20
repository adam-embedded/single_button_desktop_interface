//
// Created by adam slaymark on 20/02/2024.
//

#ifndef HEARING_TEST_BUTTON_SERIAL_H
#define HEARING_TEST_BUTTON_SERIAL_H

void serialInit(char* port, int baud);
uint8_t sendReceive(uint8_t message);
void closeSerial();
uint8_t serialRead();
void clearSerial();


#endif //HEARING_TEST_BUTTON_SERIAL_H
