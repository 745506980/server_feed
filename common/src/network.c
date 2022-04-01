/*************************************************************************
    > File Name: network.c
    > 作者:YJK 
    > Mail: 745506980@qq.com 
    > Created Time: 2021年10月14日 星期四 22时07分17秒
 ************************************************************************/

#include "myhead.h"
#include "network.h"
#include "protocol.h"
/*数据接收
*/
static INT32 connect_try(INT32 sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	INT32 num_sec;
	for(num_sec = 1; num_sec <= MAXSLEEP; num_sec <<= 1)
	{
	
		DEBUG("connect_try\n");
		if (connect(sockfd, addr, addrlen) == 0)
			// 成功连接
			return 0;
		if (num_sec <= MAXSLEEP/2)	//休眠，然后再次重连
			sleep(num_sec);
	}
	return -1;  //超时， 返回 -1
}
INT32 Client_Network_Init(void)
{
	int sockfd = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // IPV4 tcp
	if (sockfd == -1)
	{
		ERROR("socket\n");
		return -1;
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET; //IPV4 需要与socket中的指定一致
	addr.sin_addr.s_addr = inet_addr(SERVER_IP1);
	addr.sin_port = htons(SERVER_PORT);//大于1024
	memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
	if (connect_try(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		ERROR("connect\n");
		return -1;
	}
	return sockfd;
}

BOOL recvmessage(int fd, UINT8 buf[], const UINT32 length)
{
	if (length < 0)
		return FALSE;
	int trunc_ret = 0;
	int recv_ret = recv(fd, buf, length, MSG_DONTWAIT);
	if (recv_ret == -1) {
		ERROR("recv");
		return FALSE;
	} else if(recv_ret < length){ //截断
		UINT8 trunc_buf[RECV_BUF_SIZE] = {0};
		trunc_ret = recv(fd, trunc_buf, length - recv_ret, MSG_DONTWAIT);
		memcpy(&buf[recv_ret], trunc_buf, trunc_ret);
	}
//	DEBUG("recemessage:");
//	debug_data(buf, length);
	if ((trunc_ret + recv_ret) == length)
	{
		return TRUE;
	}
	else
		return FALSE;
}

int recvmessage_header(int fd, UINT8 buf[], const UINT32 length)
{
	if (length < 0 || NULL == buf)
		return FALSE;
	int trunc_ret = 0;
	int recv_ret = recv(fd, buf, length, MSG_DONTWAIT);
	if (recv_ret == -1) {
		return recv_ret;
	} else if(recv_ret < length){ //截断
		UINT8 trunc_buf[RECV_BUF_SIZE] = {0};
		trunc_ret = recv(fd, trunc_buf, length - recv_ret, MSG_DONTWAIT);
		memcpy(&buf[recv_ret], trunc_buf, trunc_ret);
	}
//	DEBUG("com1:%x, com2:%x, length:%d\n", buf[HEADER_COMMAND0_BYTE], buf[HEADER_COMMAND1_BYTE], trunc_ret + recv_ret);
	return trunc_ret + recv_ret;
}

/*数据发送*/
BOOL sendmessage(int fd, const UINT8 direction,const UINT16 command,const  UINT32 _length, const UINT8 data[])
{
	UINT8 buf[MAX_DATA_BUF_SIZE] = {0};
	INT32 length = _length;
	UINT8 Crc = 0;
	//固定头
	if (data == NULL)
	{
		return FALSE;
	}
	buf[HEADER_FIRST_BYTE] = HEADER_FIRST_FIELD;
	buf[HEADER_SECOND_BYTE] = HEADER_SECOND_FIELD;
	buf[HEADER_THIRD_BYTE] = HEADER_THIRD_FIELD;
	buf[HEADER_FOURTH_BYTE] = HEADER_FOURTH_FIELD;
	buf[HEADER_VERSION_BYTE] = HEADER_VERSION_FIELD;
	//方向
	buf[HEADER_DIRECTION_BYTE] = direction;
	//命令
	buf[HEADER_COMMAND0_BYTE] = command;
	buf[HEADER_COMMAND1_BYTE] = (command >> 8);
	//数据长度
	buf[HEADER_DATA_LENGTH0_BYTE] = length + 1; //低8位
	buf[HEADER_DATA_LENGTH1_BYTE] = (length + 1) >> 8;
	buf[HEADER_DATA_LENGTH2_BYTE] = (length + 1) >> 16;
	buf[HEADER_DATA_LENGTH3_BYTE] = (length + 1) >> 24;
	//数据
//	printf("com1:%x, com2:%x\n", buf[HEADER_COMMAND0_BYTE], buf[HEADER_COMMAND1_BYTE]);

	memcpy(buf + PROTOCOL_HEADER_SIZE, data, length);

	protocol_data_set_crc(data, length, &Crc);

	buf[PROTOCOL_HEADER_SIZE + length] = Crc;

		//发送
	return SendQueue(fd, buf, PROTOCOL_HEADER_SIZE + length + 1);
	
	//DEBUG("sendmessage:");
	//debug_data(buf, length + PROTOCOL_HEADER_SIZE);
}
/** @fn    Function   : server_reply
 *  @brief Description: 处理完客户端请求后回复客户端
 *  @brief Auther/Time: yuanjunke / 2021-10-24
 *  @param [in]fd: 客户端fd
 *  @param [in]command: 命令
 *  @param [in]data: TRUE / FALSE
 *  @return none
 */

BOOL server_reply(int fd, const UINT8 direction, const UINT16 command, const UINT8 data)
{
	return sendmessage(fd, direction, command, 1, &data);
}
BOOL client_reply(int fd, const UINT8 direction, const UINT16 command, const UINT8 data)
{
	return sendmessage(fd, direction, command, 1, &data);
}
void close_socketfd(int sockfd)
{
	shutdown(sockfd, SHUT_RDWR);
	close(sockfd);
}
