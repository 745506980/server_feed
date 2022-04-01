#ifndef SERVER_CLIENT_HEANDLE__H
#define SERVER_CLIENT_HEANDLE__H

#include "datatype.h"
#include "lstLib.h"

#define ID_SIZE 16

extern pthread_mutex_t linkmutex;
extern pthread_mutex_t linkappmutex;

typedef struct {
    NODE node;
    INT32 fd;                 		//文件描述符
    UINT8 id[ID_SIZE];              //设备ID
    UINT8 target_id[ID_SIZE];       //目标设备ID
    UINT32 length;		 	        //设备ID长度
    UINT8 num;				        //计数器，服务器每次发送心跳包num会+1， 如果num >= 3 则认为客户端无响应此时应释放该socket的资源
}LINK_NODE;

/*链表初始化*/
void List_Init(void);
/* APP连接喂食器处理 */

BOOL app_conn_feed(INT32 fd, const UINT8 direction, const UINT16 command, UINT8 target_id[],const UINT32 length);
/* 心跳消息处理 */
BOOL heatbeat_report_handle(INT32 fd, UINT8 direction, const UINT8 heatbeat[], const UINT32 length);

/* 心跳发送处理 */
void send_heartbeat(void);

/* 设备注册 */
BOOL facilty_information_handle(INT32 fd, UINT8 id[], const UINT16 command, const UINT8 direction, const UINT32 length);

/* 数据转发 */
BOOL server_data_forward(INT32 fd, UINT8 data[], const UINT8 direction, const UINT16 command, const UINT32 length);
/* 向客户端发送服务器时间 */
BOOL send_local_time(int fd, UINT8 direction);

#endif