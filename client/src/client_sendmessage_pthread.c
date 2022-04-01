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

INT32 sendmsg__(PMSG pmsg)
{
	INT32 ret = 0;
#if 1
	/* send当客户端断开时，会生成SIGPIPE信号导致进程退出， MSG_NOSIGNAL不发送SIGPIPE信号 */
	ret = send(pmsg->fd, pmsg->msg, pmsg->length, MSG_DONTWAIT | MSG_NOSIGNAL);
	if (ret < 0)
	{
		ERROR("send");
	}
	return ret;
#endif

}

void * client_sendmessage_handle_proc(void * arg)
{
	PMSGQUEUE pMsgQueue = GetMsgQueue();
	MSG msg;
	while(GetMessage(pMsgQueue, &msg))
	{
//		DEBUG("Sendmessage!!!!!!!!\n");
		sendmsg__(&msg);
	}
	return NULL;
}


INT32 client_sendmessage_pthead(void)
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

	err = pthread_create(&tid, &tattr, client_sendmessage_handle_proc, NULL);
	if (err != 0) {
		ERROR_EXIT("pthread_create\n");
	}
	
	return err;
}

