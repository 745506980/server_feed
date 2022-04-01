#include "protocol.h"
#include "network.h"
#include "myhead.h"
/*
    校验包头
    传入的参数为 header的字节数组
    length buf的长度
    obj 获取 方向 命令 和 数据长度
    成功返回 TRUE
    失败返回 FALSE
*/
//固定头 0xFF 0xAA 0xBB 0xCC 0x1

//对数据截断进行处理

BOOL data_recv_handle(int fd, UINT8 buf[], const UINT32 length)
{

	int trunc_ret = 0;
	int recv_ret = 0;
	
	if (length < 0)
		return FALSE;
	recv_ret = recv(fd, buf, length, MSG_DONTWAIT);
	if (recv_ret == -1) {
		ERROR("recv");
		return FALSE;
	} else if(recv_ret < length){ //截断
		UINT8 trunc_buf[RECV_BUF_SIZE] = {0};
		trunc_ret = recv(fd, trunc_buf, length - recv_ret, MSG_DONTWAIT);
		memcpy(&buf[recv_ret], trunc_buf, trunc_ret);
	}
	if ((trunc_ret + recv_ret) == length)
		return TRUE;
	else
		return FALSE;
}

INT32 header_recv_handle(INT32 fd, UINT8 buf[], const UINT32 length)
{
	if (length < 0)
		return FALSE;
	INT32 trunc_ret = 0;
	INT32 header_ret = recv(fd, buf, length, MSG_DONTWAIT);
	if (header_ret == -1 && errno != 11) {
		ERROR("recv");
		return -1;
	} else if (header_ret == 0) {
		return 0;	//客户端断开连接
	} else if (header_ret < length) {	//截断
		UINT8 trunc_buf[RECV_BUF_SIZE] = {0};
		trunc_ret = recv(fd, trunc_buf, length - header_ret, MSG_DONTWAIT);
		memcpy(buf + header_ret, trunc_buf, trunc_ret);
	}
	if ((trunc_ret + header_ret) == length)
		return 1;
	else
		return -1;
}

/**************************************
*
*   校验头部数据
*
*
***************************************/

BOOL protocol_header_check(UINT8 buf[], const UINT32 length, dcl * obj)
{
	if (length != PROTOCOL_HEADER_SIZE)
		return FALSE;
	if (obj == NULL || buf == NULL)
		return FALSE;

	if (buf[HEADER_FIRST_BYTE] == HEADER_FIRST_FIELD && \
	    buf[HEADER_SECOND_BYTE] == HEADER_SECOND_FIELD && \
	    buf[HEADER_THIRD_BYTE] == HEADER_THIRD_FIELD && \
	    buf[HEADER_FOURTH_BYTE] == HEADER_FOURTH_FIELD && \
	    buf[HEADER_VERSION_BYTE] == HEADER_VERSION_FIELD) 
	{
	    //校验成功
	    obj->direction = buf[HEADER_DIRECTION_BYTE];    //获取方向
	    
		obj->command |= buf[HEADER_COMMAND0_BYTE];        //获取cmd
	    obj->command |= (buf[HEADER_COMMAND1_BYTE] << 8);

	    obj->length |= buf[HEADER_DATA_LENGTH0_BYTE];   //获取数据长度
	    obj->length |= (buf[HEADER_DATA_LENGTH1_BYTE] << 8);
	    obj->length |= (buf[HEADER_DATA_LENGTH2_BYTE] << 16);
	    obj->length |= (buf[HEADER_DATA_LENGTH3_BYTE] << 24);
	    return TRUE;
	} else {
		return FALSE;
	}
}
/* 校验数据 CRC校验*/
BOOL protocol_data_check(const UINT8 data[], const UINT32 length)
{
	INT32 i = 0;
	INT32 CRC = 0;
	if (data == NULL || length > MAX_DATA_BUF_SIZE)
	{
		return FALSE;
	}
	
	for (i = 0; i < length - 1; i++)
	{
		CRC += data[i];
	}

	if (CRC % CRC_NUMBER == data[length - 1])
	{
		return TRUE;
	}
	else
	{
		ERROR("CRC is ERROR Data CRC[%d] real CRC[%d]!!!!!!\n", CRC % CRC_NUMBER, data[length - 1]);
		return FALSE;
	}

	return TRUE;
}
/* 生成校验和 */
void protocol_data_set_crc(const UINT8 data[], const UINT32 length, UINT8 * crc)
{
	INT32 i = 0;
	INT32 crcsum = 0;
	if (crc == NULL || data == NULL || length > MAX_DATA_BUF_SIZE)
	{
		ERROR("Crc is ERROR!!!!\n");
		return ;
	}
	for (i = 0; i < length; i++)
	{
		crcsum += data[i];
	}

	*crc = crcsum % CRC_NUMBER;
	
	return ;
}
void debug_data(const UINT8 data[], int length)
{
	
	INT32 i = 0;

	for (i = 0; i < length; i++)
	{
		printf("0x%hhx ",data[i]);
	}
	printf("\n");
}

void debug_protol(INT32 fd, UINT16 command)
{
	switch (command)
	{
		case ID_MANUAL_FEED :
			//手动喂食
			DEBUG("手动喂食:");
			break;
		case ID_FEED_PLAN :
			//喂食计划
			DEBUG("喂食计划:");
			break;
		case ID_CONTROL_NIGHT_LIGHT :
			//小夜灯
			DEBUG("小夜灯:");
			break;
		case ID_TEMPERATUER_DATA_REPORT :
			//温度数据上报
			DEBUG("温度数据上报:");
			break;
		case ID_HUMIDITY_DATA_REPORT :
			//湿度数据上报 
			DEBUG("湿度数据上报:");
			break;
		case ID_GET_LOCAL_TIME :
			//获取服务器时间
			DEBUG("获取服务器时间:");
			break;
		case ID_FACILITY_INFORMATION :
			//处理设备信息 将连接进服务器的client加入到链表中 server维护两个链表，
			//一个APP链表、 一个FEED 链表
			//注册成功，服务器给客户端发送 注册成功包
			DEBUG("设备注册:");
			break;
		case ID_APP_CONN_FEED :
			//APP连接目标设备
			DEBUG("APP连接目标设备:");
			break;

		case ID_FEED_RESAULT_REPORT :
			//喂食结果
			DEBUG("喂食结果:");
			break;
		case ID_HEATBEAT_REPORT:
		{
			//心跳
			DEBUG("心跳:");
			break;
		}
		case ID_AUTO_LIGHT:
		{
			//自动亮灯
			DEBUG("自动亮灯:");
			break;
		}
		case ID_QUICK_FEED:
		{
			DEBUG("快速喂食");
			break;
		}
		default:
			break;
	}
	
}
