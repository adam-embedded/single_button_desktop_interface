//
// Created by adam slaymark on 20/02/2024.
//

#ifndef HEARING_TEST_BUTTON_DEVICE_H
#define HEARING_TEST_BUTTON_DEVICE_H

void ReadyStatus();
void setQos(int s);
enum qos getQos();
void initialiseDevice();
void DeviceLoop();

// Define control commands
#define INFO_CMD 0x01
#define CTRL_CMD 0x02
#define BTN_CMD 0x03

// Define messages for info command
#define RDY_MSG 0x01
#define RDY_MSG_RTN 0x01

#define SMPL_STAT 0x02
#define SMPL_STAT_RTN_TRUE 0x01
#define SMPL_STAT_RTN_FALSE 0x02

#define QOS_MSG 0x03
#define QOS_RTN_1 0x01
#define QOS_RTN_2 0x02

// Define messages for control commands
#define SMPL_TRUE 0x01
#define SMPL_FALSE 0x02

#define SMPL_START 0x01
#define SMPL_STOP 0x02

#define QOS_MODE_1 0x03
#define QOS_MODE_2 0x04


// Define messages for button commands
#define BTN_CNFRM 0x01
#define BTN_RTN_ON 0x02
#define BTN_RTN_OFF 0x00

// Private Typedefs
typedef struct {
    uint8_t cmd;
    uint8_t msg;
}message;

enum qos {
    MODE_1,
    MODE_2
};

#endif //HEARING_TEST_BUTTON_DEVICE_H
