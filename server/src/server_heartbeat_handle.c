/*************************************************************************
    > File Name: server_heartbeat_handle.c
    > 作者:YJK 
    > Mail: 745506980@qq.com 
    > Created Time: Sun Oct 24 22:37:20 2021
 ************************************************************************/

#include "myhead.h"
#include "server_heartbeat_handle.h"
#include "protocol.h"

void send_heartbeat(void)
{
	Link_node * apphead = app_head_node_point->next;
	Link_node * feedhead = feed_head_node_point->next;
	char status = TRUE;
	BOOL ret = FALSE;
	MUTEX_LOCK(&linkmutex);
	while (feedhead)
	{
		ret = sendmessage(feedhead->fd, APP_TO_SERVER, ID_HEATBEAT_REPORT,sizeof(status),&status);
		if (ret == TRUE)
		{
			if (feedhead->num < MAX_HEATBEAT_NUM)
			{
				feedhead->num++;
			}
			else
			{
				close_socketfd(feedhead->fd);
				link_delete(feed_head_node_point, feedhead->fd);
			}
		}
		feedhead = feedhead->next;
	}
	MUTEX_UNLOCK(&linkmutex);

	
	MUTEX_LOCK(&linkmutex);
	while (apphead)
	{
		ret = sendmessage(apphead->fd, APP_TO_SERVER, ID_HEATBEAT_REPORT,sizeof(status),&status);
		if (ret == TRUE)
		{
			if (apphead->num < MAX_HEATBEAT_NUM)
			{
				apphead->num++;
			}
			else
			{
				close_socketfd(apphead->fd);
				link_delete(app_head_node_point, apphead->fd);
			}
		}
		/*发送feedhead的连接状态*/
		feedhead = get_id_node(feed_head_node_point, apphead->target_id);
		if (feedhead != NULL)
		{
			if (feedhead->num < MAX_HEATBEAT_NUM)
			{
				status = TRUE;
				sendmessage(apphead->fd, APP_TO_SERVER, ID_FEED_CONN_STATUS,sizeof(status),&status);
			}
			else
			{
				status = FALSE;
				sendmessage(apphead->fd, APP_TO_SERVER, ID_FEED_CONN_STATUS,sizeof(status),&status);
			}
		}
		else
		{
			status = FALSE;
			sendmessage(apphead->fd, APP_TO_SERVER, ID_FEED_CONN_STATUS,sizeof(status),&status);
		}
		
		apphead = apphead->next;
	}
	MUTEX_UNLOCK(&linkmutex);

}

/*
	服务器心跳包发送线程，每5S向所有连接服务器的客户端发送心跳包，
	如果超过三次未收到心跳包回复，那么则认为该客户端已断开连接，
	此时需关闭socket，并释放资源。
	该线程只进行数据发送操作
*/
void * server_heartbeat_handle_proc(void *arg)
{
	for ( ;; )
	{
		DEBUG("heartbeat ptread\n");
	
		send_heartbeat();
		
		sleep(5);
	}
}

int server_heartbeat_handle_pthead(void)
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

	err = pthread_create(&tid, &tattr, server_heartbeat_handle_proc, NULL);
	if (err != 0) {
		ERROR_EXIT("pthread_create\n");
	}
	
	return err;
}

