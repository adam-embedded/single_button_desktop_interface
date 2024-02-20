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
message compose_message(uint8_t cmd, uint8_t msg);

void buttonStart();
void buttonStop();
int8_t buttonCheck();



/* Static Variables */
static enum qos Qos;
static uint8_t samplingRunning = 0;


/* Device Functions */
void DeviceLoop(){

}


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
            if (x.msg == RDY_MSG_RTN && x.cmd == INFO_CMD) {
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

    // Check Device is online
    ReadyStatus();

    uint8_t x;
    if (Qos == MODE_1){
        x = QOS_MODE_1;
    } else {
        x = QOS_MODE_2;
    }
    uint8_t mes = encode(CTRL_CMD, x);
    mes = sendReceive(mes);
    if (mes > 0) {
        message y = decode(mes);
        printf("Quality of service confirmed by device as: MODE_%d\n", y.msg);
    } else {
        printf("Device initialisation failed. Application closing");
        exit(1);
    }
}

void buttonStart(){
    int8_t x = buttonCheck();
    if (!x){
        uint8_t set = 0;
        while(!set) {
            message y = compose_message(CTRL_CMD, SMPL_START);
            if (y.msg == SMPL_TRUE) {
                set = 1;
            }
        }
        samplingRunning = 1;
    } else {
        puts("button already started");
    }
}

void buttonStop(){
    int8_t x = buttonCheck();
    if (x){
        uint8_t set = 0;
        while(!set) {
            message y = compose_message(CTRL_CMD, SMPL_STOP);
            if (y.msg == SMPL_TRUE) {
                set = 1;
            }
        }
        samplingRunning = 0;
    } else {
        puts("Button not started");
    }
}
int8_t buttonCheck(){
    uint8_t mes = encode(INFO_CMD, SMPL_STAT);
    mes = sendReceive(mes);
    if (mes > 0){
        message y = decode(mes);
        int8_t z = 1;
        if (y.msg == SMPL_STAT_RTN_FALSE) z = 0;
        return z;
    } else {
        printf("Button Check: A fatal error has occurred");
        exit(1);
    }
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

message compose_message(uint8_t cmd, uint8_t msg){
    uint8_t mes = encode(INFO_CMD, SMPL_STAT);
    mes = sendReceive(mes);
    if (mes > 0){
        message y = decode(mes);
        return  y;
    } else {
        printf("A fatal error has occurred");
        exit(1);
    }
}