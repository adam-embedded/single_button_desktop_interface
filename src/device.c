//
// Created by adam slaymark on 20/02/2024.
//

/* Includes */
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#endif

//private includes
#include "device.h"
#include "serial.h"

/* Private Function Prototypes */
message decode(uint8_t encoded);
uint8_t encode(uint8_t cmd, uint8_t msg);

static enum qos Qos;


/* Messaging Functions */

// Check that the device is connected
// - This function is blocking
void ReadyStatus(){
    int connected = 0;
    int8_t count = 0;

    while (!connected) {
        printf("Checking for device... Try no. %i", count);

        uint8_t mes = encode(INFO_CMD, RDY_MSG);
        uint8_t data = sendReceive(mes);
        if (data > 0) {
            message x = decode(data);
            if (x.msg == RDY_MSG_RTN) {
                connected = 1;
            }
        }
        if (count > 3) {
            printf("Failed to detect device 3 times. Please check connections\n");
            exit(1);
        } else {
            count += 1;
        }

#ifdef _WIN32
        sleep(1000);
#elif __unix__ || __APPLE__
        sleep(1);
#endif
    }
}

// Send all setings over to device
void initialiseDevice(){

}

void buttonStart(){

}

void buttonStop(){

}

void setQos(int s){
    Qos = s;
}
enum qos getQos(){
    return Qos;
}

// Helper functions
message decode(uint8_t encoded){
    message data;

    data.cmd = (encoded >> 4) & 0xF;
    data.msg = encoded & 0xF;

    return data;
}

uint8_t encode(uint8_t cmd, uint8_t msg){
    uint8_t x = (cmd << 4) | msg;
    return x;
}