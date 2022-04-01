
#include "protocol.h"
#include "myhead.h"
#include "client_motor_pthread.h"
#include "client_timer_pthread.h"
#include "client.h"


/** @fn    Function   : get_local_time
 *  @brief Description: 消息发送队列
 *  @brief Auther/Time: yuanjunke / 2021-10-24
 *  @param [in]fd: 客户端fd
 *  @param [in]command: 命令
 *  @param [in]data: TRUE / FALSE
 *  @return none
 */
static BOOL get_local_time(UINT8 time_[], UINT32 length)
{
	DEBUG("###################DATE################\n");
	if (time_[TIME_STATUS] == TRUE)
	{
		struct tm stpr;
		stpr.tm_year = time_[YEAR_BYTE] + YEAR_START;//0x00 代表2021年  自1900年经过了多少年
		stpr.tm_mon = time_[MONTH_BYTE]; 	// 从0 - 11
		stpr.tm_mday = time_[DAY_BYTE];   // 格林威治时区比北京时间多了8小时
		stpr.tm_hour = time_[HOUR_BYTE];
		stpr.tm_min = time_[MINUTE_BYTE];
		stpr.tm_sec = time_[SECOND_BYTE];
		struct timeval tv;
		tv.tv_sec = mktime(&stpr);	 //将st结构体数据 解析为自1970年1月1日00:00:00以来的秒数

		tv.tv_usec = 0;
		settimeofday(&tv, NULL);	//更新系统时间

/*		time_t result = time(NULL);
		printf("%s%ju secs since the Epoch\n",
                asctime(localtime(&result)),
                    (unsigned long int)result);
*/
		return TRUE;
	}

	return FALSE;
}


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
	switch (command) {
		case ID_MANUAL_FEED :
		{
			//手动喂食
			
			DEBUG("ID_MANUAL_FEED:%d length %d\n", buf[0], length);
			SendMotorQueue(buf[0]);
			break;
		}
		case ID_FEED_PLAN :
		{
			//喂食计划
			DEBUG("ID_FEED_PLAN:");
			debug_data(buf, length);
			/* 更新喂食计划 */
			client_update_feedplan(buf, length);
			/* 更新定时器 */
			client_update_timer();
			break;
		}
		case ID_CONTROL_NIGHT_LIGHT :
			DEBUG("ID_CONTROL_NIGHT_LIGHT:");
			/* 小夜灯模块，
			1、自动亮灯开关这个需要更新rgb小灯驱动，对光敏电阻传感器的中断响应进行处理;
			2、小夜灯可以配置颜色  设置不同电平的组合可以调出不同的颜色;
			*/
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
			/* 心跳处理 */
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
//					DEBUG("command :0x%x data:", obj.command);
//					debug_data(recv_data, obj.length);
					if (protocol_data_check(recv_data, obj.length))
					{
						/* 如果是喂食器到服务器数据，则CRC校验和只有一个 */
						/* 如果是APP到喂食器，那么CRC校验数据则会有两个，length-2 是APP生成的CRC， length - 1 是服务器生成的CRC */
						if (obj.direction == APP_TO_FEEDER)
						{
							client_data_analysis_handle(sockfd, recv_data, obj.direction, obj.command, obj.length - 2);
						}
						else if (obj.direction == FEEDER_TO_SERVER)
						{
							client_data_analysis_handle(sockfd, recv_data, obj.direction, obj.command, obj.length - 1);
						}
						else
						{
							ERROR("direction is error [%d]\n", obj.direction);
						}

						 //减去CRC校验数据
					}
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