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
#include "client_weight_pthread.h"

INT32 sockfd = 0;
sigset_t mask;

#define FEED_CLIENT 1

static void client_init(void)
{
	MsgQueue_init();
	log_init("/home/feedlog", DEBUG_ERROR | DEBUG_INFO);
	client_sendmessage_pthead();
}
static void Client_Active(INT8 * argv)
{	
	UINT8 tarid[] = "87654321";
	UINT8 c = 0xFF;
	if (NULL != argv)
	{
		memset(tarid, 0, sizeof(tarid));
		memcpy(tarid, argv, strlen(argv) + 1); /* copy '\0' */
	}
	sendmessage(sockfd, FEEDER_TO_SERVER, ID_FACILITY_INFORMATION, sizeof(tarid), tarid);
	sendmessage(sockfd, FEEDER_TO_SERVER, ID_GET_LOCAL_TIME, sizeof(c), &c);
	return ;
}
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
INT32 main(INT32 argc,char *argv[])
{
	sockfd = Client_Network_Init();
	if (-1 == sockfd)
	{
		ERROR("Client network init is error!!!\n");
		return -1;
	}
	/* 设置当前的信号屏蔽集 下面的子线程将继承该信号屏蔽集*/
	if (-1 == client_SetsigMask())
	{
		return -1;
	}

	client_init();
	/* 初始化消息发送线程 */
	DEBUG("connect is success!\n");

	Client_Active(argv[1]);
	#if 1
	/* 温湿度线程 */
	client_humiture_pthread();
	/* 电机线程 */
	client_motor_pthread();
	/* MCU交互线程 */
	client_weight_pthread();
	#endif
	/* 定时器线程 信号处理 */
	client_timer_pthread();

	client_handle_proc(sockfd);

	close_socketfd(sockfd);

	return 0;
}

