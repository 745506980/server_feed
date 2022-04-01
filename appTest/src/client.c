/*************************************************************************
    > File Name: client_select.c
    > 作者:YJK
    > Mail: 745506980@qq.com
    > Created Time: 2021年05月15日 星期六 20时16分59秒
 ************************************************************************/
#include "protocol.h"
#include "myhead.h"
#include "client.h"
#include "client_sendmessage_pthread.h"
#include "client_datahandle.h"
#include "client_humiture_pthread.h"
#include "client_motor_pthread.h"
#include "client_timer_pthread.h"

INT32 sockfd = 0;
sigset_t mask;

#define FEED_CLIENT 1

static void client_init(void)
{
	MsgQueue_init();
	client_sendmessage_pthead();
}
static void Client_Active(INT8 * argv)
{	
	UINT8 id[] = "123456789";
	UINT8 tarid[] = "87654321";
	if (NULL != argv)
	{
		memset(tarid, 0, sizeof(tarid));
		memcpy(tarid, argv, strlen(argv) + 1); /* copy '\0' */
	}

	sendmessage(sockfd, APP_TO_SERVER, ID_FACILITY_INFORMATION, sizeof(id), id);
	sendmessage(sockfd, APP_TO_SERVER, ID_APP_CONN_FEED, sizeof(tarid), tarid);

	return ;
}
#if 0
/* 设置客户端的信号屏蔽集，主要屏蔽一些信号，如SIGALRM定时器信号，只有定时器线程可以接收 */
static INT32 client_SetsigMask(void)
{
	INT32 err = -1;
	sigset_t oldmask;

	sigemptyset(&mask);

	sigaddset(&mask, SIGALRM);
	/* 在创建子线程之前设置信号屏蔽集，那么所创建的子线程也会继承该信号屏蔽集的掩码 */
	/*
		How:
		SIG_BLOCK:     结果集是当前集合参数集的并集
		SIG_UNBLOCK: 结果集是当前集合参数集的差集
		SIG_SETMASK: 结果集是由参数集指向的集
	*/
	err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask);
	if (-1 == err)
	{
		ERROR("pthread_sigmask is error!!!\n");
		return -1;
	}
	return 0;
}
#endif
INT32 main(INT32 argc,char *argv[])
{
	INT32 index = 0;
	sockfd = Client_Network_Init();
	if (-1 == sockfd)
	{
		ERROR("Client network init is error!!!\n");
		return -1;
	}

	/* 初始化消息发送线程 */
	client_init();
	DEBUG("connect is success!\n");

	Client_Active(argv[1]);
	PROTOCOL_FEED_PLAN_ST FeedPlan[PROTOCOL_FEED_PLAN_MAX];
	memset(FeedPlan, 0, sizeof(FeedPlan));
#if 0
#define PROTOCOL_SUNDAY_SWITCH					(0x1)	/* 周日 */
#define PROTOCOL_MONDAY_SWITCH					(0x2) 	/* 周一 */
#define	PROTOCOL_TUESDAY_SWITCH					(0x4)	/* 周二 */
#define PROTOCOL_WEDNESDAY_SWCITCH				(0x8)	/* 周三 */
#define PROTOCOL_THURSDAY_SWITCH				(0x10)	/* 周四 */
#define PROTOCOL_FRIDAY_SWITCH					(0x20)	/* 周五 */
#define	PROTOCOL_SATURDAY_SWITCH				(0x40)	/* 周六 */
PROTOCOL_ALLDAY_SWITCH
#endif 
//	FeedPlan[0].uWeekDay = 0xFE; /* 1周7天 */
	FeedPlan[index].uWeekDay = PROTOCOL_THURSDAY_SWITCH;
	FeedPlan[index].uHour = 8;
	FeedPlan[index].uMin = 0;
	FeedPlan[index].uFeedNum = 5;
	FeedPlan[index].uSwitch = 1;
	index ++;
//	FeedPlan[1].uWeekDay = 0xFE; /* 1周7天 */
	FeedPlan[index].uWeekDay = PROTOCOL_THURSDAY_SWITCH; /* 0001 0000*/
	FeedPlan[index].uHour = 6;
	FeedPlan[index].uMin = 0;
	FeedPlan[index].uFeedNum = 5;
	FeedPlan[index].uSwitch = 1;
	index ++;
//	FeedPlan[2].uWeekDay = 0xFE; /* 1周7天 */
	FeedPlan[index].uWeekDay = PROTOCOL_THURSDAY_SWITCH;
	FeedPlan[index].uHour = 7;
	FeedPlan[index].uMin = 0;
	FeedPlan[index].uFeedNum = 5;
	FeedPlan[index].uSwitch = 1;
	index ++;
//	FeedPlan[3].uWeekDay = 0xFE; /* 1周7天 */
	FeedPlan[index].uWeekDay = PROTOCOL_ALLDAY_SWITCH;
	FeedPlan[index].uHour = 9;
	FeedPlan[index].uMin = 0;
	FeedPlan[index].uFeedNum = 5;
	FeedPlan[index].uSwitch = 1;
	index ++;
	FeedPlan[index].uWeekDay = PROTOCOL_ALLDAY_SWITCH;
	FeedPlan[index].uHour = 10;
	FeedPlan[index].uMin = 0;
	FeedPlan[index].uFeedNum = 5;
	FeedPlan[index].uSwitch = 1;
	index ++;	
	FeedPlan[index].uWeekDay = PROTOCOL_ALLDAY_SWITCH;
	FeedPlan[index].uHour = 11;
	FeedPlan[index].uMin = 0;
	FeedPlan[index].uFeedNum = 5;
	FeedPlan[index].uSwitch = 1;
	index ++;	
	FeedPlan[index].uWeekDay = PROTOCOL_ALLDAY_SWITCH;
	FeedPlan[index].uHour = 12;
	FeedPlan[index].uMin = 0;
	FeedPlan[index].uFeedNum = 5;
	FeedPlan[index].uSwitch = 1;
	index ++;

	while(1)
	{
		/* 发送喂食计划 */
		sendmessage(sockfd, APP_TO_FEEDER, ID_FEED_PLAN, sizeof(PROTOCOL_FEED_PLAN_ST) * index, (UINT8 *)FeedPlan);
		
		client_handle_proc(sockfd);
	}

	close_socketfd(sockfd);

	return 0;
}

