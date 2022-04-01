#ifndef DEBUG__H
#define DEBUG__H
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "datatype.h"

//[file]:[function]:[line]:error


#ifndef __UTIL_DEBUG_H__
#define __UTIL_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif


#define DBG_INFO_LEN_MAX    1024 /* 最大输出的字符个数 */

/* debug Level */
#define DBG_NONE                0x0  /* may be used set by cmd */

#define DBG_CRIT                0x1  /* critical info  */
#define DBG_ERR                 0x2  /* error info */
/* following macros are forbidden if hicore will be released */
#define DBG_WARN                0x3  /* waring info */
#define DBG_HINT                0x4  /* hint info*/
#define DBG_INFO                0x5  /* debug info*/
#define DBG_PRIT                0x6  /* print info*/

/* debug Level num*/
#define DBG_LEV_NUM             0x6

/*information format*/
#define DBG_FORMAT_NONE         0x0
#define DBG_FORMAT_TIME         0x1
#define DBG_FORMAT_FILELINE     0x2
#define DBG_FORMAT_FUNCTION     0x4
#define DBG_FORMAT_THREAD       0x8

#define  UTIL_FILE          __FILE__
#define  UTIL_LINE          __LINE__

/*__VA_ARGS__ can only appear in the expansion of a C99 variadic macro*/
//#define  UTIL_VA_ARGS       __VA_ARGS__

#if defined(_WIN32)
    #if _MSC_VER >= 1200
    #define UTIL_FUNCTION        __FUNCTION__
    #else
    #define UTIL_FUNCTION        (const char *)0
    #endif
#elif defined(__linux__)
    #if __GNUC__ >= 3
    #define UTIL_FUNCTION        __FUNCTION__
    #else
    #define UTIL_FUNCTION        (const char *)0
    #endif
#endif

typedef struct
{
    INT32 iDbgLevel; 
    INT32 iDbgFormat; 
    INT32 iDbgModule; 
    INT32 iDbgMask;
}UTIL_DBG_CTRL;

struct Util_Debug_OP
{
    const char* (*GetThreadName)();
};

int util_dbg_version(void);


int util_dbg_open \
    (unsigned int dbgLevel, unsigned int modIdx, unsigned int modMask);
void _util_debug \
    (const char *file, int line, const char *function, int level, const char *fmt, ...);

void set_util_debug_arr(struct Util_Debug_OP* utilDbg);

void util_set_dbg(int dbgflag);



/******************** debug switcher for whole prj *************************
 **                                                                       **
 ** 1.all of DEBUG CFG options see definition in top of util_debug.c *******
 ** 2.UTIL_DEBUG just be able to print BYTES small than DBG_INFO_LEN_MAX.***
 ** 3.never forget to close HIK_GLOBAL_DEBUG in release version!!!**********
 **                                                                       **
 ** huangjs restruct end.***************************************************/

#ifndef CONFIG_SUPPORT_DBG_CTRL
#define CONFIG_SUPPORT_DBG_CTRL
#endif

#ifdef  CONFIG_SUPPORT_DBG_CTRL 

#define UTIL_DEBUG(DBG_T, MODULE_IDX, MODULE_MASK, ...) {\
    if (util_dbg_open(DBG_T, MODULE_IDX, MODULE_MASK))\
    {\
        _util_debug(UTIL_FILE, UTIL_LINE, UTIL_FUNCTION, DBG_T, __VA_ARGS__);\
    }\
}

#else
    #define UTIL_DEBUG(DBG_T, MODULE_IDX, MODULE_MASK, ...)
#endif


/*it used to open some indicate or temporarily debug info.*/
#define USR_DBG(...) \
    UTIL_DEBUG(DBG_PRIT, MODULE_DBG_FILE, MASK_DBG_FILE, __VA_ARGS__)


/*it used to check user function argument whether be Illegal?
 * NULL Pointer, over flow Array offset, etc.*/
#define usr_assert(c, ret) {\
    if(!(c))\
    {\
        UTIL_DEBUG(DBG_ERR, MODULE_DBG_FILE, MASK_DBG_FILE,\
            "[%s] Assertion: %s fail!\n", UTIL_FUNCTION, #c);\
        return (ret);\
    }\
}

void util_debug_hex(unsigned char* pBuff, unsigned int bufLen);
void get_util_dbg(UINT32* pLevel, UINT32* pFormat
    , UINT32* pModule, UINT32* pSubModule);
void util_dbg_set
(
    unsigned iDbgLevel, 
    unsigned iDbgFormat, 
    unsigned iDbgModule, 
    unsigned iDbgMask
);//在头部增加声明;
INT32 util_read_file_level(INT8* pFile, UTIL_DBG_CTRL* pCtrl);

/******************************************************
  Function:     util_dbg_get
  Description:  get deg ctrl param.
  Input:        dbgLevel:   打印级别
                modIdx:     打印一级模块
                modMask     打印二级子模块
  Output:           N/A
  Return:           N/A
*******************************************************/
void util_dbg_get(char* pBuf, unsigned iBufLen);

int log_init(const char * path, int flags);
/******************************** end **************************************/

#ifdef __cplusplus
}

#endif
#define DEBUG_CTRL 1

#ifdef DEBUG_CTRL

#define DEBUG_ERROR         (1 << 1)
#define DEBUG_INFO          (1 << 0)

#define ERROR_EXIT(x)                        \
do{                                     \
    fprintf(stderr,                     \
    "[ERROR]:[FILE]:%s, [FUNCTION]:%s, [LINE]:%d, [ERRNO]:%d, [ERRMSG]:%s \n", \
    __FILE__, __FUNCTION__, __LINE__, errno, x);            \
    exit(EXIT_FAILURE);                           \
}while(0)

#define ERROR(...)                        \
do{                                     \
    UTIL_DEBUG(DBG_ERR, 1, DEBUG_ERROR, __VA_ARGS__); \
}while(0)


#define DEBUG(...)               \
do{                                     \
    UTIL_DEBUG(DBG_INFO, 1, DEBUG_INFO, __VA_ARGS__); \
}while(0)   

#define DEBUG_SET_FLAGS(x) \
do{                         \
    util_set_dbg(x);        \
}while(0)                   \

#endif

#else

#endif

#endif
