#include "myhead.h"
#include "datatype.h"
#include "client_timer_pthread.h"
#include "client.h"
#include "client_motor_pthread.h"

static PROTOCOL_FEED_PLAN_ST FeedPlan[PROTOCOL_FEED_PLAN_MAX];



static UINT32 TimerIndex = TIMER_INVAKID;	/* 当前定时器所在的喂食计划索引 */

void client_timer_Set_TimerIndex(UINT32 Index)
{
	TimerIndex = Index;
	return ;
}

void client_timer_SetEmpty_FeedPlan(void)
{
	memset(FeedPlan, 0 , sizeof(FeedPlan));
	return ;
}


void client_update_feedplan(UINT8 * feedplan, UINT32 length)
{
	INT32 i = 0;  /* 当前索引位置 */
	if (length % FEED_PLAN_SIZE != 0)
	{
		ERROR("feedplan length is error!!!!length%d\n", length);
		return ;
	}
	/* 先清空上次的喂食计划 */
	memset(FeedPlan, 0, sizeof(FeedPlan));
	/* 更新当前的喂食计划 */
	for (i = 0; i < FEED_PLAN_SIZE; i++)
	{
		FeedPlan[i].uWeekDay = feedplan[i + 0];
		FeedPlan[i].uHour =feedplan[i + 1];
		FeedPlan[i].uMin = feedplan[i + 2];
		FeedPlan[i].uFeedNum = feedplan[i + 3];
		FeedPlan[i].uSwitch = feedplan[i + 4];
		DEBUG("WeekDay[%d] Hour[%d] Min[%d] FeedNum[%d] Switch[%d]\n", FeedPlan[i].uWeekDay, FeedPlan[i].uHour, FeedPlan[i].uMin, FeedPlan[i].uFeedNum, FeedPlan[i].uSwitch);
	}

	return ;
}


/* 找到距离当前时间最小的喂食计划并设置 闹钟*/

void client_update_timer(void)
{
	INT32 i = 0;
	INT32 j = 0;
	time_t NowTime = 0;
	time_t MinTime = 0x15180; /* 1天 */
	time_t TimeTmp = 0;
	time_t AlarmTime = 0;
	INT32 TimerStatus = TIMER_INVAKID;
	INT32 AlarmDay = 0;
	struct tm nowtm;
	memset(&nowtm, 0 , sizeof(nowtm));
	/* 找到属于当前周的 */
	for (i = 0; i < PROTOCOL_FEED_PLAN_MAX; i++)
	{
		NowTime = time(NULL);
		localtime_r(&NowTime, &nowtm);
		/* 今天的闹钟 获取还有多少秒 */
		for (j = 0; j <= PROTOCOL_WEEKDAY_MAX; j++)
		{
			AlarmDay = (nowtm.tm_wday + j) % PROTOCOL_WEEKDAY_MAX;	/* 寻找从今天至后面一周的闹钟 */
			/* 寻找最近的一天是否有闹钟，有的话直接返回 */
			if (FeedPlan[i].uSwitch == 0x1 && ((FeedPlan[i].uWeekDay < AlarmDay) & 0x1))
			{
				TimeTmp = 0;
				if (nowtm.tm_sec != 0)
				{
					TimeTmp = nowtm.tm_sec;
					TimeTmp += ((j * 24 + FeedPlan[i].uHour - nowtm.tm_hour) * 60 + (FeedPlan[i].uMin - nowtm.tm_min - 1)) * 60;
				}
				else
				{
					TimeTmp += ((j * 24 + FeedPlan[i].uHour - nowtm.tm_hour) * 60 + (FeedPlan[i].uMin - nowtm.tm_min)) * 60;
				}
				/* 已经过时的闹钟退出 */
				if (TimeTmp < 0)
				{
					continue;
				}

				if (MinTime > TimeTmp)
				{
					MinTime = TimeTmp;
					TimerIndex = i;
					TimerStatus = 1;
					/* 当天这一天有闹钟，退出 */
					break;
				}
			}

		}
	}

	AlarmTime = MinTime;
	/* 今日无闹钟并且第二天也没有，将定时器设置为今天的23:59:59 */
	if (TimerStatus == TIMER_INVAKID)
	{
		NowTime = time(NULL);
		localtime_r(&NowTime, &nowtm);
		nowtm.tm_hour = 23;
		nowtm.tm_min = 59;
		nowtm.tm_sec = 59;
		AlarmTime = mktime(&nowtm);
		AlarmTime = AlarmTime - NowTime; 
		ERROR("The Day is No Timer!!! #######  AlarmTimer :%d\n", AlarmTime);
		/* TimerIndex设置为无效 */
		TimerIndex = TIMER_INVAKID;
	}

	DEBUG("TimerIndex[%d] AlarmTime[%d]\n", TimerIndex, AlarmTime);

	alarm(AlarmTime);

	return ;
}

/*
    喂食计划应当使用链表还是全局数组？
    
*/

/* 
    定时器线程，主要完成的工作是，喂食计划中的定时器
 */


/* 
    1.获取到喂食计划之后，应当先将本地的喂食计划进行更新;
    2.然后找到最小值，进行定时器定时;
    3.主线程更新喂食计划之后，应当通知该定时器线程，让其去执行定时任务;
    4.该线程应当是通过调用方式来进行创建，然后去执行定时器任务;
    5. 如果有时间更近的定时器被下发，应当使用什么样的策略？  应当使用alarm定时器，然后使用接收信号的方式来进行处理
    定时器采用一个数组的方式来进行管理，当消息处理线程将数据接收完毕时通知定时器线程，定时器线程将喂食计划，通过获取距离当前时间最近的来进行alarm定时
    然后，在SIG_ALARM信号处理函数中，将喂食计划发送给motor线程


    MINIGUI的定时器线程是，每10ms 注意这里使用的是usleep 通知一次然后根据TIMER 消息流转将TIMER消息发送至对应窗口

	alarm是进程资源，所以所有线程共享
	
    2、
 */

void * client_timer_handle_proc(void * arg)
{
	/*  */
	client_timer_SetEmpty_FeedPlan();
	INT32 signo = 0;
	INT32 err;
	for ( ;; )
	{
		err = sigwait(&mask, &signo);
		if (0 != err)
		{
			ERROR("sigwait is error!!!!  err:%d\n",err);
			return NULL;
		}
		switch (signo)
		{
			case SIGALRM:
			{
				DEBUG("TIMER is OK!!!!!!!!!!!!!!!!!!!!!!!!\n");
				//SendMotorQueue(FeedPlan[TimerIndex].uFeedNum);
				if (TimerIndex != TIMER_INVAKID)
				{
					SendMotorQueue(FeedPlan[TimerIndex].uFeedNum);
				}

				client_update_timer();

				break;
			}
			default:
			{
				ERROR("sigwait recv signal :%d\n", signo);
				break;
			}
		}
	}
    return NULL;
}


/* 定时器处理线程 */
INT32 client_timer_pthread(void)
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

	err = pthread_create(&tid, &tattr, client_timer_handle_proc, NULL);
	if (err != 0) {
		ERROR_EXIT("pthread_create\n");
	}
	return err;
}