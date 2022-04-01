
#ifndef CLIENT_MOTOR_PTHREAD__H
#define CLIENT_MOTOR_PTHREAD__H
#include <semaphore.h>
#include "datatype.h"
#define MOTOR_DEV "/dev/my_motor"

/* 一个节拍 转动 5.625*/

#define MOTOR_CMD_A _IOW('M',0,long)
#define MOTOR_CMD_B _IOW('M',1,long)
#define MOTOR_CMD_C _IOW('M',2,long)
#define MOTOR_CMD_D _IOW('M',3,long)

#define HIGH 1
#define LOW 0

#define NUMBER 10000
#define SPEED 1500

typedef struct {
    UINT8 feed_num;
}MOTORMSG;

typedef struct _MTORMSG{
    MOTORMSG msg;
    struct _MTORMSG * next; 
}QMOTORMSG;

typedef QMOTORMSG* PQMOTORMSG;

typedef struct {
    sem_t wait; //信号量
    PQMOTORMSG pFirstMsg;
	PQMOTORMSG pLastMsg;
}MOTORQUEUE;

typedef MOTORQUEUE * PMOTORQUEUE;

INT32 SendMotorQueue(UINT8 feed_num);
void Set_MotorfeedSpeed(int feedspeed);
/* 温湿度传感器数据处理线程 */
int client_motor_pthread(void);
#endif