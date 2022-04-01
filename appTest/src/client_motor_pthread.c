//SPEED 越小转速越快
#include "client_motor_pthread.h"
#include "myhead.h"
#include "client.h"
static PMOTORQUEUE __pMotorMsgQueue = NULL;

static int __feedspeed = 0;  // 1 高速  0 低速
/* 设置喂食速率 */
void Set_MotorfeedSpeed(int feedspeed)
{
    __feedspeed = feedspeed;
    return ;
}
/* 获取当前的喂食速率 */
static int Get_MotorfeedSpeed(void)
{  
    return __feedspeed;
}

static PMOTORQUEUE GetMotorMsgQueue(void)
{
	return __pMotorMsgQueue;
}
static BOOL MotorMsgQueue_init(void)
{
	__pMotorMsgQueue = (PMOTORQUEUE)malloc(sizeof(MOTORQUEUE));
	if (__pMotorMsgQueue != NULL)
	{
		__pMotorMsgQueue->pFirstMsg = NULL;
		__pMotorMsgQueue->pLastMsg = NULL;
			
		sem_init (&__pMotorMsgQueue->wait, 0, 0);
		return TRUE;
	}
	else
	{
		ERROR("malloc");
		return FALSE;
	}
}

INT32 SendMotorQueue(UINT8 feed_num)
{
    PMOTORQUEUE pMotorMsgQueue = GetMotorMsgQueue();
    if (pMotorMsgQueue == NULL)
    {
        ERROR("pMotorMsgQueue is NULL\n");
        return -1;
    }
    PQMOTORMSG  pqmsg = NULL;
    pqmsg = (PQMOTORMSG)malloc(sizeof(QMOTORMSG));
	if (pqmsg == NULL)
	{
		ERROR("malloc\n");
		return FALSE;
	}
    pqmsg->msg.feed_num = feed_num;
    pqmsg->next = NULL;
    if (pMotorMsgQueue->pFirstMsg == NULL) {
		
        pMotorMsgQueue->pFirstMsg = pMotorMsgQueue->pLastMsg = pqmsg;
    }
    else {
        pMotorMsgQueue->pLastMsg->next = pqmsg;
        pMotorMsgQueue->pLastMsg = pqmsg;
    }

	POST_MSGQ(pMotorMsgQueue);
    return TRUE;
}

static int GetMotorMessage(PMOTORQUEUE MsgQueue, MOTORMSG * pmsg)
{
	if (NULL == MsgQueue || pmsg == NULL)
	{
		return FALSE;
	}
    PMOTORQUEUE pMotorMsgQueue = MsgQueue;
	PQMOTORMSG phead;
	WAIT_MSGQ(pMotorMsgQueue);
    if (pMotorMsgQueue->pFirstMsg) {
        phead = pMotorMsgQueue->pFirstMsg;
        
        pmsg->feed_num = phead->msg.feed_num;

        pMotorMsgQueue->pFirstMsg = phead->next;
        free (phead);
    }
	return TRUE;
}

static void motor_off(INT32 fd)
{
    ioctl(fd, MOTOR_CMD_A, LOW);
    ioctl(fd, MOTOR_CMD_B, LOW);
    ioctl(fd, MOTOR_CMD_C, LOW);
    ioctl(fd, MOTOR_CMD_D, LOW);
    return ;
}
static INT32 meter_turn(INT32 fd, INT32 n)
{
    switch(n)
    {
        case 0:
            ioctl(fd, MOTOR_CMD_A, LOW);
            ioctl(fd, MOTOR_CMD_B, HIGH);
            ioctl(fd, MOTOR_CMD_C, HIGH);
            ioctl(fd, MOTOR_CMD_D, HIGH);
            break;
        case 1:
            ioctl(fd, MOTOR_CMD_A, LOW);
            ioctl(fd, MOTOR_CMD_B, LOW);
            ioctl(fd, MOTOR_CMD_C, HIGH);
            ioctl(fd, MOTOR_CMD_D, HIGH);
            break;
        case 2:
            ioctl(fd, MOTOR_CMD_A, HIGH);
            ioctl(fd, MOTOR_CMD_B, LOW);
            ioctl(fd, MOTOR_CMD_C, HIGH);
            ioctl(fd, MOTOR_CMD_D, HIGH);
            break;
        case 3:
            ioctl(fd, MOTOR_CMD_A, HIGH);
            ioctl(fd, MOTOR_CMD_B, LOW);
            ioctl(fd, MOTOR_CMD_C, LOW);
            ioctl(fd, MOTOR_CMD_D, HIGH);
            break;
        case 4:
            ioctl(fd, MOTOR_CMD_A, HIGH);
            ioctl(fd, MOTOR_CMD_B, HIGH);
            ioctl(fd, MOTOR_CMD_C, LOW);
            ioctl(fd, MOTOR_CMD_D, HIGH);
            break;
        case 5:
            ioctl(fd, MOTOR_CMD_A, HIGH);
            ioctl(fd, MOTOR_CMD_B, HIGH);
            ioctl(fd, MOTOR_CMD_C, LOW);
            ioctl(fd, MOTOR_CMD_D, LOW);
            break;
        case 6:
            ioctl(fd, MOTOR_CMD_A, HIGH);
            ioctl(fd, MOTOR_CMD_B, HIGH);
            ioctl(fd, MOTOR_CMD_C, HIGH);
            ioctl(fd, MOTOR_CMD_D, LOW);
            break;
        case 7:
            ioctl(fd, MOTOR_CMD_A, LOW);
            ioctl(fd, MOTOR_CMD_B, HIGH);
            ioctl(fd, MOTOR_CMD_C, HIGH);
            ioctl(fd, MOTOR_CMD_D, LOW);
            break;
    }
    return 0;
}

/*
    发送喂食记录
    [1Byte喂食份数]+[1Byte年份(自2020年起)]+[1Byte月份]+[1Byte周]+[1Byte小时]+[1Byte分钟]+[1Byte秒]
    例如2021年6月20日12:30:30喂食2份  数据表示为0x020106130C1E1E
 */
static void sendfeed_result(UINT8 feednum)
{
    time_t time_ = 0;
    struct tm tm_;
    UINT8 FeedResult[7] = {0};
    time_ = time(NULL);
    memset(&tm_, 0, sizeof(tm_));
    /* 这里应使用 localtime_r 而不是 localtime 因为localtime是不可重入的*/
    localtime_r(&time_, &tm_);
    FeedResult[0] = feednum;
    FeedResult[YEAR_BYTE]   = tm_.tm_year - YEAR_START;   /* 0x00标识2021年 */
    FeedResult[MONTH_BYTE]  = tm_.tm_mon;
	FeedResult[DAY_BYTE]    = tm_.tm_mday;
	FeedResult[HOUR_BYTE]	= tm_.tm_hour;
	FeedResult[MINUTE_BYTE] = tm_.tm_min;
	FeedResult[SECOND_BYTE] = tm_.tm_sec;

    /* 喂食结果上报 */
    sendmessage(sockfd, FEEDER_TO_APP, ID_FEED_RESAULT_REPORT, sizeof(FeedResult), FeedResult);
    return ;
}
/* 电机控制 */
static void motor_on(INT32 fd, const UINT8 direction, UINT8 feed_num)
{
	INT32 i = 0;
	INT32 step = 0;
	INT32 round = 2048 * feed_num; /*一份半圈*/
	INT32 speed = 0;
    INT32 slow = Get_MotorfeedSpeed();
	printf("slow %d\n", slow);
	if (slow == 1)
		speed = 3000;  //慢速喂食
	else
		speed = 1500;  //快速喂食
	if (direction == 'R')
		step = -1;
	else
		step = 8;
	for (i = 0; i < round; i++)
	{
		if (direction == 'R')
		{
			step++;
			if (step > 7)
				step = 0;
		}
		else
		{
			if (step == 0)
				step = 8;
			step--;
		}

		meter_turn(fd, step);
		usleep(speed);
	}
	/*完毕 关闭各个IO*/
	motor_off(fd);
	//喂食结果上报
    sendfeed_result(feed_num);
}
/* 
    电机控制线程
    1、该电机控制数据由消息队列进行处理；
    目前只有两个地方向该消息队列投递消息 
    (1) 手动喂食消息处理 将喂食数量投递至消息队列
    (2) 定时器线程，当喂食定时器到达时，则向消息队列投递消息
 */
static void * client_motor_handle_proc (void * arg)
{
    MOTORMSG msg = {0};
    INT32 fd = 0;
    MotorMsgQueue_init();
    DEBUG("MotorMsgQueue init is success!!!\n");
    fd = open(MOTOR_DEV, O_RDWR);
    if (-1 == fd)
    {
        ERROR("open motor is err!!!\n");
        return NULL;
    }
    while(GetMotorMessage(__pMotorMsgQueue, &msg))
    {
        if (msg.feed_num > 0)
        {
            motor_on(fd, 'L', msg.feed_num);
        }
    }
    return NULL;
}
/* 温湿度传感器数据处理线程 */
INT32 client_motor_pthread(void)
{
    INT32 err = 0;	
	pthread_t tid = 0;
	pthread_attr_t tattr;
	memset(&tattr, 0, sizeof(tattr));
	err = pthread_attr_init(&tattr);
	if (err != 0){
		ERROR_EXIT("pthread_attr_init\n");
	}
	err = pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
	if (err != 0){
		ERROR_EXIT("pthread_attr_setdetachstate\n");
	}

	err = pthread_create(&tid, &tattr, client_motor_handle_proc, NULL);
	if (err != 0) {
		ERROR_EXIT("pthread_create\n");
	}
	return err;
}