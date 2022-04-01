/*************************************************************************
    > File Name: client.h
    > 作者:YJK
    > Mail: 745506980@qq.com
    > Created Time: 2021年07月20日 星期二 15时47分54秒
 ************************************************************************/

#ifndef CLIENT__H
#define CLIENT__H
#include "datatype.h"


#define BUFSIZE 1024

extern INT32 sockfd;
extern sigset_t mask;

INT32 client_handle(INT32 sockfd);

#define CLIENT_TEST 1


#endif
/*防止头文件重复定义*/
