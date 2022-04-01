/*************************************************************************
    > File Name: server_sendmessage_thread.c
    > 作者:YJK 
    > Mail: 745506980@qq.com 
    > Created Time: Wed Nov  3 21:38:03 2021
 ************************************************************************/
#include "myhead.h"
/*
	消息发送线程
*/



int sendmsg__(PMSG pmsg)
{
	int ret = 0;
	ret = send(pmsg->fd, pmsg->msg, pmsg->length, 0);
	if (ret != pmsg->length)
	{
		ERROR("send");
	}
}

void * server_sendmessage_handle_proc(void * arg)
{
	PMSGQUEUE pMsgQueue = GetMsgQueue();
	MSG msg;
	while(GetMessage(pMsgQueue, &msg))
	{
		sendmsg__(&msg);
	}
}


int server_sendmessage_pthead(void)
{
	int err = 0;	
	pthread_t tid = 0;
	pthread_attr_t tattr = {0};
	err = pthread_attr_init(&tattr);
	if (err != 0){
		ERROR_EXIT("pthread_attr_init\n");
	}
	err = pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
	if (err != 0){
		ERROR_EXIT("pthread_attr_setdetachstate\n");
	}

	err = pthread_create(&tid, &tattr, server_sendmessage_handle_proc, NULL);
	if (err != 0) {
		ERROR_EXIT("pthread_create\n");
	}
	
	return err;
}

