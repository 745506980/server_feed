/*************************************************************************
    > File Name: server_protocol.c
    > 作者:YJK
    > Mail: 745506980@qq.com
    > Created Time: 2021年07月20日 星期二 15时21分22秒
 ************************************************************************/

#include "protocol.h"
#include "myhead.h"


BOOL server_data_forward(int fd, unsigned char data[], const unsigned char direction, const unsigned short command, const unsigned int length)
{
	if (data == NULL)
	{
		ERROR("data is null!!!!");
		return FALSE;
	}
	//首先在feed链表中找到连接当前feed设备的app的id, 然后查找其fd
	Link_node * feed_node = NULL;
	Link_node * app_node = NULL;
	BOOL ret = FALSE;
	switch (direction)
	{
		case FEEDER_TO_APP:
		{
			feed_node = get_fd_node(feed_head_node_point, fd);
			if (feed_node != NULL) {
				app_node = get_id_node(app_head_node_point, feed_node->target_id);
				if (app_node != NULL) {
					//向APP发送数据
					ret = sendmessage(app_node->fd, direction, command, length, data);
					if (ret == TRUE) 
						return TRUE;
					else 
						return FALSE;
				} else {
					DEBUG("app_node id:%s, not found\n", feed_node->target_id);
					return FALSE;
				}
			} 
			break;
		}
		case APP_TO_FEEDER:
		{
			app_node = get_fd_node(app_head_node_point, fd);
			if (app_node != NULL) {
				feed_node = get_id_node(feed_head_node_point, app_node->target_id);
				if (feed_node != NULL) {
					//向喂食器发送数据
					ret = sendmessage(feed_node->fd, direction, command, length, data);
					if (ret == TRUE) 
						return TRUE;
					else 
						return FALSE;
				} else {
					DEBUG("feed_node id:%s, not found\n", app_node->target_id);
					return FALSE;
				}
			} 
			break;
		}
		default:
			break;
	}


}
BOOL facilty_information_handle(int fd, unsigned char id[], const unsigned short command, const unsigned char direction, const unsigned int length)
{
	//根据direction 确定是APP还是feed
//	debug_data(id, length);
	BOOL ret;
	switch (direction) {
		case APP_TO_SERVER :
		{
			//将APP info 添加到APP链表中去
			ret = link_append(app_head_node_point, app_end_node_point, fd, id, length);
			if (ret == TRUE)
				server_reply(fd, direction, command, TRUE);
			else
				server_reply(fd, direction, command, FALSE);

			pri_all_node(app_head_node_point);
			break;
		}
		case FEEDER_TO_SERVER :
		{
			//将FEED info 添加到FEED链表中去
			ret = link_append(feed_head_node_point, feed_end_node_point, fd, id, length);
			if (ret == TRUE)
				server_reply(fd, direction, command, TRUE);
			else
				server_reply(fd, direction, command, FALSE);

			pri_all_node(feed_head_node_point);
			break;
		}
	}
	
	if (ret == TRUE) {	//给客户端回复
		
	}
	return ret;
}

BOOL app_conn_feed(int fd, const unsigned char direction, const unsigned short command, unsigned char target_id[],const unsigned int length)
{
	if (update_targetid_node(app_head_node_point, fd, target_id, length) == TRUE){
		//send conn success
		server_reply(fd, direction, command, TRUE);
		return TRUE;
	} else {
		server_reply(fd, direction, command, FALSE);
		return FALSE;
	}
}
static BOOL send_local_time(int fd, unsigned char direction)
{
	//根据方向进行处理
	unsigned char buf[64] = {0};
	int index = 0;
	BOOL ret = FALSE;
	time_t ti;	//time_t long long 64位,使用8Byte进行存储
	time(&ti); //自1970.1.1 0:0:0:00经过的秒数
	struct tm * tm = localtime(&ti);	//转成 struct tm类型
	buf[TIME_STATUS] = TRUE;
	buf[YEAR_BYTE]	 = tm->tm_year;
	buf[MONTH_BYTE]  = tm->tm_mon;
	buf[DAY_BYTE]	 = tm->tm_mday;
	buf[HOUR_BYTE]	 = tm->tm_hour;
	buf[MINUTE_BYTE] = tm->tm_min;
	buf[SECOND_BYTE] = tm->tm_sec;
	//8Byte
	ret = sendmessage(fd, direction, ID_GET_LOCAL_TIME, TIME_BYTE_END, buf);
	if (ret == TRUE)
		return TRUE;
	else
		return FALSE;
}

BOOL heatbeat_report_handle(int fd, unsigned char direction, const unsigned char heatbeat[], const unsigned int length)
{
	if (heatbeat[HEATBEAT_CHECK_BYTE] == TRUE)
	{
		MUTEX_LOCK(&linkmutex);
		Link_node * node = get_fd_node(app_head_node_point, fd);
		if (node == NULL)
		{
			node = get_fd_node(feed_head_node_point, fd);
		}
		node->num = 0;
		MUTEX_UNLOCK(&linkmutex);
		DEBUG("ID:%s heatbeat is OK!\n", node->id);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
		
}

BOOL feed_resault_handle(int fd, unsigned char feedresult[], const unsigned char direction, const unsigned short command, const unsigned int length)
{
	MUTEX_LOCK(&linkmutex);
	server_data_forward(fd, feedresult, direction, command, length);
	MUTEX_UNLOCK(&linkmutex);		
}
BOOL temperatuer_data_handle(int fd, unsigned char temperatuerdata[], const unsigned char direction, const unsigned short command, const unsigned int length)
{
	MUTEX_LOCK(&linkmutex);
	server_data_forward(fd, temperatuerdata, direction, command, length);
	MUTEX_UNLOCK(&linkmutex);		

}
BOOL humdity_data_handle(int fd, unsigned char humditydata[], const unsigned char direction, const unsigned short command, const unsigned int length)
{	
	MUTEX_LOCK(&linkmutex);
	server_data_forward(fd, humditydata, direction, command, length);
	MUTEX_UNLOCK(&linkmutex);		
}
BOOL control_night_light_handle(int fd, unsigned char lightdata[], const unsigned char direction, const unsigned short command, const unsigned int length)
{
	MUTEX_LOCK(&linkmutex);
	server_data_forward(fd, lightdata, direction, command, length);
	MUTEX_UNLOCK(&linkmutex);		
}
BOOL feed_plan_handle(int fd, unsigned char lightdata[], const unsigned char direction, const unsigned short command, const unsigned int length)
{
	MUTEX_LOCK(&linkmutex);
	server_data_forward(fd, lightdata, direction, command, length);
	MUTEX_UNLOCK(&linkmutex);		
}
BOOL manual_feed_handle(int fd, unsigned char lightdata[], const unsigned char direction, const unsigned short command, const unsigned int length)
{
	MUTEX_LOCK(&linkmutex);
	server_data_forward(fd, lightdata, direction, command, length);
	MUTEX_UNLOCK(&linkmutex);	
}



BOOL data_handle(int fd, unsigned char buf[], const unsigned char direction, const unsigned short command, const unsigned int length)
{
	//根据command和direction，进行处理
	//首先、客户端建立连接发送一个自身属性包
	BOOL ret;
	switch (command) {
		case ID_MANUAL_FEED :
			//手动喂食
			ret = manual_feed_handle (fd, buf, direction, command, length);
			break;
		case ID_FEED_PLAN :
			//喂食计划
			ret = feed_plan_handle (fd, buf, direction, command, length);
			break;
		case ID_CONTROL_NIGHT_LIGHT :
			//小夜灯
			ret = control_night_light_handle (fd, buf, direction, command, length);
			break;
		case ID_TEMPERATUER_DATA_REPORT :
			//温度数据上报
			ret = temperatuer_data_handle (fd, buf, direction, command, length);
			break;
		case ID_HUMIDITY_DATA_REPORT :
			//湿度数据上报 
			ret = humdity_data_handle (fd, buf, direction, command, length);
			break;
		case ID_GET_LOCAL_TIME :
			//获取服务器时间
			ret = send_local_time(fd, direction);
			break;
		case ID_FACILITY_INFORMATION :
			//处理设备信息 将连接进服务器的client加入到链表中 server维护两个链表，
			//一个APP链表、 一个FEED 链表
			//注册成功，服务器给客户端发送 注册成功包
			MUTEX_LOCK(&linkmutex);
			ret = facilty_information_handle(fd, buf, command, direction, length);
			MUTEX_UNLOCK(&linkmutex);
			break;
		case ID_APP_CONN_FEED :
			//APP连接目标设备
			ret = app_conn_feed(fd, direction, command, buf, length);
			break;

		case ID_FEED_RESAULT_REPORT :
			//喂食结果
			ret = feed_resault_handle(fd, buf, direction, command, length);
			break;
		case ID_HEATBEAT_REPORT:
		{
			DEBUG("ID_HEATBEAT_REPORT  :%d\n", buf[0]);
			ret = heatbeat_report_handle(fd, direction, buf, length);
			break;
		}
		default:
			break;
	}

	return ret;
}

