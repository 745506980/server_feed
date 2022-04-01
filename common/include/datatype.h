/*************************************************************************
    > File Name: datatype.h
    > 作者:YJK
    > Mail: 745506980@qq.com
    > Created Time: 2021年07月19日 星期一 10时19分00秒
 ************************************************************************/

#ifndef DATATYPE__H
#define DATATYPE__H



#define IS_NULL -1

typedef /*signed*/ char     INT8;
typedef unsigned char       UINT8;
typedef /*signed*/ short    INT16;
typedef unsigned short      UINT16;
typedef /*signed*/ int      INT32;
typedef unsigned int        UINT32;
typedef void*               VOIDPTR;

#if defined(__LP64__)
    typedef signed long int INT64;
    typedef unsigned long int UINT64;
#else
    typedef signed long long int INT64;
    typedef unsigned long long int UINT64;
#endif

#define BOOL INT32
#define TRUE 1
#define FALSE 0
#endif
/*防止头文件重复定义*/
