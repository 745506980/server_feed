/*************************************************************************
    > File Name: server_heartbeat_handle.h
    > 作者:YJK 
    > Mail: 745506980@qq.com 
    > Created Time: Sun Oct 24 22:37:10 2021
 ************************************************************************/

#ifndef SERVER_HEARTBEAT_HANDLE__H
#define SERVER_HEARTBEAT_HANDLE__H
#include "datatype.h"


#define SERVER_HEARTBEAT_HANDLE_TIMER           (5)   /* 心跳5S一次 */



/*心跳包处理线程*/

INT32 server_heartbeat_handle_pthead(void);


#endif
/*防止头文件重复定义*/
