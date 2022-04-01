#ifndef SERVER__H
#define SERVER__H

#include "datatype.h"
#include "pthread_head.h"
#include "mylink.h"
#define BACKLOG 5                 //系统进程索要入队的未完成连接请求数量

#define EPOLL_LISTEN_MAX 1024     //epoll 监听的最大数量

#define WAITING_FORVER -1


/*
    服务器端
*/

/*
    server 绑定 ip 和 port  返回值 server_fd
*/

INT32 server_bind_listen_fd(void);
/*
    服务器循环程序，进行数据处理
*/
void server_loop(void);

/*
    客户端数据处理
*/
//int server_hande(int epoll_fd, int client_fd, struct epoll_event * event);


/*
    1、断开socket
    2、从epoll监听队列中去除
*/
void fd_destory(int fd);
//链表初始化
BOOL server_init(void);

#endif

