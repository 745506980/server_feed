
#include "protocol.h"
#include "myhead.h"
#include "client_motor_pthread.h"
#include "client_timer_pthread.h"
/** @fn    Function   : client_handle_proc
 *  @brief Description: 解析服务器发送的数据
 *  @brief Auther/Time: yuanjunke / 2021-10-24
 *  @param [in]fd: 服务器fd
 *  @param [in]buf: 需要处理的数据缓冲区
 *  @param [in]direction: 方向
 *  @param [in]command: 命令
 *  @param [in]length: 数据长度
 *  @return TRUE / FALSE
 */
static BOOL client_data_analysis_handle(INT32 fd, UINT8 buf[], const UINT8 direction, const UINT16 command, const UINT32 length)
{

	//根据command和direction，进行处理
	//首先、客户端建立连接发送一个自身属性包
	BOOL ret = FALSE;
	#if 1
	switch (command) {
		case ID_MANUAL_FEED :
		{
			//手动喂食
			DEBUG("ID_MANUAL_FEED:%d\n", buf[0]);
			SendMotorQueue(buf[0]);
			break;
		}
		case ID_FEED_PLAN :
		{
			//喂食计划
			DEBUG("ID_FEED_PLAN:");
			debug_data(buf, length);
			break;
		}
		case ID_CONTROL_NIGHT_LIGHT :
			DEBUG("ID_CONTROL_NIGHT_LIGHT:");
			/* 更新喂食计划 */
			client_update_feedplan(buf, length);
			/* 更新定时器 */
			client_update_timer();
			//小夜灯
			break;
		case ID_GET_LOCAL_TIME :
		{
			//获取服务器时间
			get_local_time(buf, length);
			DEBUG("ID_GET_LOCAL_TIME:");
			debug_data(buf, length);
			break;
		}
		case ID_HEATBEAT_REPORT:
		{
			DEBUG("ID_HEATBEAT_REPORT  :%d\n", buf[0]);
			if (buf[HEATBEAT_CHECK_BYTE] == TRUE)
			{
				client_reply(fd, direction, command, TRUE);
			}
			break;
		}
		case ID_QUICK_FEED:
		{
			/* 快速喂食 */
			DEBUG("ID_QUICK_FEED :%d\n", buf[0]);
			Set_MotorfeedSpeed(buf[0]);
		}
		default :
			break;
	}
	#endif
	return ret;
}

/** @fn    Function   : client_handle_proc
 *  @brief Description: 解析服务器发送的数据
 *  @brief Auther/Time: yuanjunke / 2021-10-24
 *  @param [in]fd: 服务器fd
 *  @return none
 */
static INT32 client_data_handle(INT32 sockfd)
{
    unsigned char recv_data[RECV_BUF_SIZE] = {0};
    unsigned char recv_header[PROTOCOL_HEADER_SIZE] = {0};
    dcl obj = {0};
	BOOL status = FALSE;
	INT32 ret = 1;
    //取出所有数据
    //TCP会出现粘包、截断的情况
	while (ret > 0) {
		memset(recv_header, 0, PROTOCOL_HEADER_SIZE);
		memset(&obj, 0, sizeof(dcl));
		memset(recv_data, 0, obj.length);
		ret = recvmessage_header(sockfd, recv_header, PROTOCOL_HEADER_SIZE);
		if (ret == PROTOCOL_HEADER_SIZE) {
		//对包头进行解析，获取包头中的数据 如 方向、命令、数据长度；
			status = protocol_header_check(recv_header, PROTOCOL_HEADER_SIZE, &obj);
			if (status == TRUE) {
				//包头解析成功,获取数据
				status = recvmessage(sockfd, recv_data, obj.length);
				if (status == TRUE) {
				//数据获取成功，根据方向、命令、数据进行处理
					//do_something
					DEBUG("command :0x%x data:", obj.command);
					debug_data(recv_data, obj.length);
					client_data_analysis_handle(sockfd, recv_data, obj.direction, obj.command, obj.length);
				}
			}
		}
		else if (ret == 0)
		{
			close_socketfd(sockfd);
			DEBUG("socket is close\n");
		}
	}
	return 0;
}

/** @fn    Function   : client_handle_proc
 *  @brief Description: 接收服务器发送数据
 *  @brief Auther/Time: yuanjunke / 2021-10-24
 *  @param [in]fd: 服务器fd
 *  @return none
 */

void client_handle_proc(INT32 sockfd)
{
	INT32 ret = 1;
    struct timeval tim;
	fd_set readfds;
	for ( ;; )
	{
		FD_ZERO(&readfds);
		FD_SET(sockfd, &readfds);
		tim.tv_sec = 10;
		tim.tv_usec = 0;
		ret = select(sockfd + 1, &readfds, NULL, NULL, &tim);
		if (ret < 0)
		{
			ERROR_EXIT("select");
		}
		else if (ret == 0)
		{
			continue;
		}
		else 
		{
			client_data_handle(sockfd);
		}
	}
}