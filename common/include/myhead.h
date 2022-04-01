/*************************************************************************
    > File Name: myhead.h
    > 作者:YJK 
    > Mail: 745506980@qq.com 
    > Created Time: 2021年10月14日 星期四 22时23分21秒
 ************************************************************************/

#ifndef MYHEAD__H
#define MYHEAD__H


#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "pthread_head.h"
#include "datatype.h"
#include "debug.h"
#include "network.h"
#include "queue.h"
#include "protocol.h"
#endif
/*防止头文件重复定义*/
