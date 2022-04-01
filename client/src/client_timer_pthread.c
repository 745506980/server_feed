#include "myhead.h"
#include "datatype.h"
#include "client_timer_pthread.h"
#include "client.h"
#include "client_motor_pthread.h"


static PROTOCOL_FEED_PLAN_ST FeedPlan[PROTOCOL_FEED_PLAN_MAX];

static UINT32 FeedLength = 0;	/* 当前喂食计划的有效个数 */

static UINT32 TimerIndex = TIMER_INVAKID;	/* 当前定时器所在的喂食计划索引 */

static INT32 fd; /* 喂食计划文件fd */
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
	INT32 err = 0;

	if (length % FEED_PLAN_SIZE != 0)
	{
		ERROR("feedplan length is error!!!!length%d\n", length);
		return ;
	}
	/* 定位到文件开始 */
	lseek(fd, 0, SEEK_SET);

	FeedLength = length / FEED_PLAN_SIZE;
	/* 先清空上次的喂食计划 */
	memset(FeedPlan, 0, sizeof(PROTOCOL_FEED_PLAN_ST)); //
	/* 更新当前的喂食计划 */
	
	for (i = 0; i < FeedLength; i++)
	{
		FeedPlan[i].uWeekDay = 	feedplan[i * FEED_PLAN_SIZE + 0];
		FeedPlan[i].uHour	 =	feedplan[i * FEED_PLAN_SIZE + 1];
		FeedPlan[i].uMin 	 = 	feedplan[i * FEED_PLAN_SIZE + 2];
		FeedPlan[i].uFeedNum = 	feedplan[i * FEED_PLAN_SIZE + 3];
		FeedPlan[i].uSwitch  = 	feedplan[i * FEED_PLAN_SIZE + 4];
		DEBUG("[%d]WeekDay[%d] Hour[%d] Min[%d] FeedNum[%d] Switch[%d]\n" ,i ,FeedPlan[i].uWeekDay, FeedPlan[i].uHour, FeedPlan[i].uMin, FeedPlan[i].uFeedNum, FeedPlan[i].uSwitch);
	}

	/* 写入文件中 */
	err = write(fd, FeedPlan, sizeof(PROTOCOL_FEED_PLAN_ST) * FeedLength);
	if (-1 == err)
	{
		ERROR("write is error fd:%d\n", fd);
		return ;
	}
	return ;
}

/* 找到距离当前时间最小的喂食计划并设置 闹钟*/

void client_update_timer(void)
{
	INT32 i = 0;
	INT32 j = 0;
	time_t NowTime = 0;
	time_t MinTime = TIMER_ONEWEEK; /* 一周 */
	time_t TimeTmp = 0;
	time_t AlarmTime = 0;
	INT32 TimerStatus = TIMER_INVAKID;
	INT32 AlarmDay = 0;
	struct tm nowtm;
	memset(&nowtm, 0 , sizeof(nowtm));
	/* 找到属于当前周的 这里需要循环8次 第0次是今天的闹钟，从今天到下周的今天*/

	for (i = 0; i <= PROTOCOL_WEEKDAY_MAX; i++)
	{
		NowTime = time(NULL);
		localtime_r(&NowTime, &nowtm);
		/* 获取到闹钟，退出 */
		if (1 == TimerStatus)
		{
			break;
		}
		/* 解析喂食计划中的数据，找到距离今天最近的喂食计划 */
		for (j = 0; j < FeedLength; j++)
		{
			DEBUG("Clock!!!!!i[%d] j[%d] TimeTmp[%d] AlarmDay[%d] \n",i , j, TimeTmp, AlarmDay);
			AlarmDay = (nowtm.tm_wday + i) % PROTOCOL_WEEKDAY_MAX;	/* 寻找从今天至后面一周的闹钟 */
			/* 寻找最近的一天是否有闹钟，有的话直接返回 */
			if (FeedPlan[j].uSwitch == 1 && (FeedPlan[j].uWeekDay & (0x1 << AlarmDay)))
			{
				TimeTmp = 0;
				if (nowtm.tm_sec != 0)
				{
					TimeTmp = nowtm.tm_sec;
					/* 距离第 i 天的闹钟所间隔的秒数 */
					TimeTmp += ((i * 24 + FeedPlan[j].uHour - nowtm.tm_hour) * 60 + (FeedPlan[j].uMin - nowtm.tm_min - 1)) * 60;
				}
				else
				{
					TimeTmp += ((i * 24 + FeedPlan[j].uHour - nowtm.tm_hour) * 60 + (FeedPlan[j].uMin - nowtm.tm_min)) * 60;
				}
				/* 已经过时的闹钟退出 */
				if (TimeTmp < 0)
				{
					continue;
				}
				/* 更新距离当前时间最近的闹钟 */
				if (MinTime > TimeTmp)
				{
					MinTime = TimeTmp;
					TimerIndex = j;
					TimerStatus = 1;
					/* 当天这一天有闹钟，退出 */
					DEBUG("Clock!!!!! TimerIndex[%d] AlarmTime[%d] WeekDay[%d] %d:%d FeedNum[%d] \n",
					TimerIndex, MinTime, AlarmDay + 1, FeedPlan[j].uHour, FeedPlan[j].uMin, FeedPlan[j].uFeedNum);
				}
			}
		}
	}

	AlarmTime = MinTime;
	/* 今日无闹钟并且第二天也没有，将定时器设置为今天的23:59:59 */
	if (TimerStatus == TIMER_INVAKID)
	{
		#if 0
		NowTime = time(NULL);
		localtime_r(&NowTime, &nowtm);
		nowtm.tm_hour = 23;
		nowtm.tm_min = 59;
		nowtm.tm_sec = 59;
		AlarmTime = mktime(&nowtm);
		AlarmTime = AlarmTime - NowTime; 
		#endif
		/* 无闹钟那么闹钟时间为一周 */
		ERROR("The Day is No Timer!!! #######  AlarmTimer :%d\n", AlarmTime);
		/* TimerIndex设置为无效 */
		TimerIndex = TIMER_INVAKID;

	}

	DEBUG("TimerIndex[%d] AlarmTime[%d]\n", TimerIndex, AlarmTime);
	
	/* 如果无喂食计划，那么定时器设置为下周的当前时间 当有喂食计划到来时，该alarm闹钟会被更新 */

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

	INT32 signo = 0;
	INT32 ret = 0;
	INT32 i = 0;

	client_timer_SetEmpty_FeedPlan();
	
	fd = open(FEEDPLAN_FILEPATH, O_CREAT | O_RDWR, 0777);
	if (-1 == fd)
	{
		ERROR("open %s is error!!\n", FEEDPLAN_FILEPATH);
		return NULL;
	}
	/* 获取喂食计划 */
	do{		
		ret = read(fd, FeedPlan, sizeof(FeedPlan));
	}while(ret == -1 && errno==EINTR);
	if (-1 == ret)
	{
		ERROR("read is error!!! fd[%d]\n", fd);
		return NULL;
	}
	/* 获取当前喂食计划的数量 */
	FeedLength = ret / sizeof(PROTOCOL_FEED_PLAN_ST); 
	DEBUG("##################FEED PLAN#############\n");
	for (i = 0; i < FeedLength; i++)
	{
		DEBUG("[%d]WeekDay[%d] Hour[%d] Min[%d] FeedNum[%d] Switch[%d]\n" , 
		i ,FeedPlan[i].uWeekDay, FeedPlan[i].uHour, FeedPlan[i].uMin, FeedPlan[i].uFeedNum, FeedPlan[i].uSwitch);
	}
	/* 设置定时器 */
	client_update_timer();

	for ( ;; )
	{
		ret = sigwait(&mask, &signo);
		if (0 != ret)
		{
			ERROR("sigwait is error!!!!  err:%d\n",ret);
			return NULL;
		}
		switch (signo)
		{
			case SIGALRM:
			{
				DEBUG("###############CLOCK IS OK################\n");
			
				if (TimerIndex != TIMER_INVAKID)
				{
					DEBUG("FeedPlan[%d] WeekDay[%d] Hour[%d] Min[%d] FeedNum[%d] Switch[%d]\n" , 
					TimerIndex ,FeedPlan[TimerIndex].uWeekDay, FeedPlan[TimerIndex].uHour, FeedPlan[TimerIndex].uMin, FeedPlan[TimerIndex].uFeedNum, FeedPlan[TimerIndex].uSwitch);
					SendMotorQueue(FeedPlan[TimerIndex].uFeedNum);
				}

				/* 更新数据 */
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

