
/*
 * Copyright 2008-2011 Hikvision Digital Technology Co.,Ltd.
 * FileName:      util_debug.c
 * Description:   general module for log & debug. 
 * Modification History
 *  <version>    <time>      <author >   <desc>
 *    v2.0     May 20, 2010   huangtf    create
 *    v2.0     Oct 12, 2010   huangjs   modify debug cfg by "util_dbg_open".     
 *    v2.0     Oct 12, 2010   fenggx    support windows env.
 */
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "debug.h"
#include "datatype.h"
#include "mylog.h"
#include "fcntl.h"
#if defined (_WIN32)
#include <Windows.h>
#elif defined (__linux__)
#include <unistd.h>
#endif

static LogLevel s_eLogBehavior = LOG_LEVEL_PRINT_SAVE;
static int logfd;
static int __dbgflag = 0;
static int check_if_set_adbg(void)
{
    int bSetADBG = 0;
    const char *path = "/proc/cmdline";
    FILE *file = NULL;
    INT8 cmdline[256] = {0};
    
    file = fopen(path, "r");
    if (NULL == file)
    {
        printf("fopen %s error!\n", path);
        return -1;
    }

    if (NULL == fgets(cmdline, 256, file))
    {
        printf("fgets file:%s error!\n", path);
        bSetADBG = -1;
    }
    if (NULL != strstr(cmdline, "adbg=y"))
    {
        bSetADBG = 1;
    }

    fclose(file);
    return bSetADBG;
}

static int check_if_start_psh(void)
{
    int bOpenPSH = 0;
    const char *path = "/etc/profile";
    FILE *file = NULL;
    char data[256] = {0};
    char *start  = NULL;
    char *string = NULL;

    file = fopen(path, "r");
    if (NULL == file)
    {
        printf("fopen %s error!\n", path);
        return -1;
    }

    do
    {
        memset(data, 0, sizeof(data));
        string = fgets(data, sizeof(data), file);
        start = strstr(data, "/bin/psh");
        /* /bin/psh必须定格写（这里不考虑前面有无效空格的场景）, #/bin/psh为注释 */
        if ((NULL != start) && (start == data))
        {
            bOpenPSH = 1;
            break;
        }
    } while(string);

    fclose(file);
    return bOpenPSH;
}

void log_behavior_init(void)
{
    if ((1 == check_if_set_adbg()) || (1 != check_if_start_psh()))
    {
        printf("log behavior init to \"PRINT_SAVE\" mode\n");
        s_eLogBehavior = LOG_LEVEL_PRINT_SAVE;
    }
    else
    {
        printf("log behavior init to \"SAVE_ONLY\" mode\n");
        s_eLogBehavior = LOG_LEVEL_SAVE_ONLY;
    }
}

LogLevel log_behavior_get(void)
{
    return s_eLogBehavior;
}

void log_behavior_set(LogLevel eLogBehavior)
{
    const char *astrLogBehavior[] = {"PRINT_ONLY", "PRINT_SAVE", "SAVE_ONLY"};

    if ((eLogBehavior != LOG_LEVEL_PRINT_ONLY)
            && (eLogBehavior != LOG_LEVEL_PRINT_SAVE)
            && (eLogBehavior != LOG_LEVEL_SAVE_ONLY))
    {
        printf("Invalid log behavior : %d\n", eLogBehavior);
        return;
    }

    if (s_eLogBehavior == eLogBehavior)
    {
        return;
    }

    printf("change log behavior to %s mode!\n", astrLogBehavior[eLogBehavior]);
    s_eLogBehavior = eLogBehavior;
}

/************************ Default DEBUG CFG *******************************
 **************** all of debug cfg options list here! *********************
 **                                                                      **
 ** 1.DBG_INFO_LEN_MAX: bytes "_util_debug" can print at most. ************
 ** 2.DEFAULT_DBG_LEVEL:  cfg of debug level control. *********************
 ** 3.DEFAULT_DBG_FORMAT: cfg of debug format control. ********************
 ** 4.DEFAULT_MODULE_DBG: cfg of debug module control. ********************
 **   folders in APPS: dataApplication,gui,net,storage,event,system etc. ** 
 ** 5.DEFAULT_MODULE_MASK: cfg of debug module mask control. **************
 **   child folders of every modules: alarmCtrl,vca,... in event etc. *****
 **                                                                      **
 ** huangjs restruct end.**************************************************/

/*all options:
    ((1<<DBG_CRIT)|(1<<DBG_ERR)|(1<<DBG_WARN)|(1<<DBG_INFO)|(1<<DBG_HINT))
    #define DEFAULT_DBG_LEVEL   \
   ((1<<DBG_CRIT)|(1<<DBG_ERR)|(1<<DBG_WARN))
   */
#define DEFAULT_DBG_LEVEL   \
       ((1<<DBG_CRIT) | (1<<DBG_ERR) | (1<<DBG_WARN) | (1<<DBG_HINT))
/*all options:
    (DBG_FORMAT_TIME | DBG_FORMAT_FILELINE | DBG_FORMAT_FUNCTION)**********/
#define DEFAULT_DBG_FORMAT  \
    (DBG_FORMAT_FILELINE | DBG_FORMAT_FUNCTION | DBG_FORMAT_THREAD)

/*all options:(~0x0), MODULE_DBG_FILE etc, details see util_module.h*/
#define DEFAULT_MODULE_DBG  \
    (~0x0)

/*all options:(~0x0), MASK_DBG_FILE etc, details see util_module.h*/
#define DEFAULT_MODULE_MASK \
    (~0x0)
/*ZDS_TEST_DEBUG*/
//#define DBG_VERSION_BY_NFS

/**************************************************************************/



/* log infomation title */
static const char *levelStr[DBG_LEV_NUM] =
    {"[CRIT]", "[ERROR]", "[WARNING]", "[HINT]", "[INFO]", "[PRIT]"};

static unsigned int g_iDbgLevel = DEFAULT_DBG_LEVEL;
static unsigned int g_iDbgFormat = DEFAULT_DBG_FORMAT;
static unsigned int g_iDbgModule = DEFAULT_MODULE_DBG;
static unsigned int g_iDbgMask = DEFAULT_MODULE_MASK;


/* g_ALL_OP存储每个厂家的不同操作的接口 ,
按厂家的值作为数组下标*/
static struct Util_Debug_OP g_stuArrOPDbg = {0};

static char* util_dbg_base_name(char* p)
{
    char* pRet = p;
    char* pTmp = NULL;
    
    while(p && *p && (pTmp = strchr(p, '/')) )
    {
        p = ++pTmp; 
        pRet = p;
    }

    return pRet;
}

void set_util_debug_arr(struct Util_Debug_OP* pUtilDbg)
{
    g_stuArrOPDbg.GetThreadName = pUtilDbg->GetThreadName;
    
    return ;
}
/******************************************************
  Function:     util_dbg_get
  Description:  get deg ctrl param.
  Input:        dbgLevel:   打印级别
                modIdx:     打印一级模块
                modMask     打印二级子模块
  Output:           N/A
  Return:           N/A
*******************************************************/
void util_dbg_get(char* pBuf, unsigned iBufLen)
{
    if (!pBuf || !iBufLen)
    {
        return;
    }
    
    snprintf(pBuf + strlen(pBuf), iBufLen - strlen(pBuf),
        "g_iDbgLevel,g_iDbgFormat,g_iDbgModule,g_iDbgMask:  0x%08x 0x%08x 0x%08x 0x%08x\n",
        g_iDbgLevel, g_iDbgFormat, g_iDbgModule, g_iDbgMask);
    
    return;
}

/******************************************************
  Function:     util_dbg_set
  Description:  set deg ctrl param.
  Input:        dbgLevel:   打印级别
                modIdx:     打印一级模块
                modMask     打印二级子模块
  Output:           N/A
  Return:           N/A
*******************************************************/
void util_dbg_set \
    (unsigned iDbgLevel, unsigned iDbgFormat, unsigned iDbgModule, unsigned iDbgMask)
{
    if (!iDbgLevel && !iDbgFormat && !iDbgModule && !iDbgMask)
    {
        g_iDbgLevel = DEFAULT_DBG_LEVEL;
        g_iDbgFormat = DEFAULT_DBG_FORMAT;
        g_iDbgModule = DEFAULT_MODULE_DBG;
        g_iDbgMask = DEFAULT_MODULE_MASK;        
    }
    else
    {
        g_iDbgLevel = iDbgLevel;
        g_iDbgFormat = iDbgFormat;
        g_iDbgModule = iDbgModule;
        g_iDbgMask = iDbgMask;
    }
    return;
}
void get_util_dbg(UINT32* pLevel, UINT32* pFormat
    , UINT32* pModule, UINT32* pSubModule)
{
    if (!pLevel || !pFormat || !pModule || !pSubModule)
    {
        return;
    }
 
  *pLevel = g_iDbgLevel;
 
  *pFormat = g_iDbgFormat;
 
  *pModule = g_iDbgModule;
 
  *pSubModule = g_iDbgMask; 
}


void util_set_dbg(int dbgflag)
{
    __dbgflag = dbgflag;
}
/******************************************************
  Function:     util_dbg_open
  Description:  判断是否需调用_util_debug进行打印
  Input:        dbgLevel:   打印级别
                modIdx:     打印一级模块
                modMask     打印二级子模块
  Output:           N/A
  Return:           N/A
*******************************************************/
int util_dbg_open \
    (unsigned int dbgLevel, unsigned int modIdx, unsigned int modMask)
{
    #if 0
    if ((dbgLevel > 0)
        && (dbgLevel <= DBG_LEV_NUM)
        && ((1 << dbgLevel) & g_iDbgLevel)
        && (modIdx & g_iDbgModule)
        && (modMask & g_iDbgMask))
    {
        return 1;
    }
    #endif
    if ((dbgLevel > 0) && (__dbgflag & modMask))
    {
        return 1;
    }
    return 0;
}

int log_init(const char * path,int flags)
{
    DEBUG_SET_FLAGS(flags);
    logfd = open(path, O_CREAT | O_RDWR);
    if (logfd == -1)
    {
        ERROR("log init is error!!\n");
        return -1;
    }
//    printf("LOG init logfd:%d\n", logfd);
    return 0;
}
/*************************************************
  Function:     _util_debug
  Description:  调试信息打印接口
  Input:        FLFL_DEF:   包含__FILE__, __LINE__, FUNCTION,
                level:  打印信息级别
                    (DBG_CRIT | DBG_ERR | DBG_WARN | DBG_INFO | DBG_HINT)
                const char *fmt, ... 打印信息
  Output:           N/A
  Return:           N/A
*************************************************/
void _util_debug \
    (const char *file, int line, const char *function, int level, const char *fmt, ...)
{
    int         sizeCnt = 0;
    int         fmtCnt = 0;
    char        str[DBG_INFO_LEN_MAX+4]   = {0};
    char        timeStr[32] = {0};
    time_t      now = 0;
    va_list ap = {0};
    
    struct tm   *pLocalTime = NULL;
    unsigned    logFmt = 0xFF;

    if ((!file) || (!fmt))
    {
        return;
    }
    /*----------------optional format------------------*/
    if(logFmt & DBG_FORMAT_TIME)
    {
        now = time(NULL);
        pLocalTime = localtime(&now);
        strftime(timeStr, sizeof(timeStr), "%F %T", pLocalTime);
        sizeCnt += sprintf(str + sizeCnt, "[%s]", timeStr);
    }
    if(logFmt != DBG_FORMAT_NONE)
    {
        /*param level has check by util_dbg_open()*/
        sizeCnt += sprintf(str + sizeCnt, "%s", levelStr[level-1]);
    }    
    if(logFmt & DBG_FORMAT_FILELINE)
    {
        sizeCnt += sprintf(str + sizeCnt, "[%s #%d]", util_dbg_base_name((char *)file), line);
    }
    if(logFmt & DBG_FORMAT_FUNCTION)
    {
        if(NULL != function)
        {
            sizeCnt += sprintf(str + sizeCnt, "[@%s]", function);
        }
    }
    if(logFmt != DBG_FORMAT_NONE)
    {
        sizeCnt += sprintf(str + sizeCnt, ": ");
    }

    va_start(ap, fmt);
    fmtCnt = vsnprintf(str + sizeCnt, DBG_INFO_LEN_MAX + 3 - sizeCnt, fmt, ap);
    va_end(ap);
    
    /*_util_debug can print DBG_INFO_LEN_MAX bytes at most.
     * we assert and hint it following.
     */
    if (str[DBG_INFO_LEN_MAX-1])
    {
        str[DBG_INFO_LEN_MAX-1] = 0x00; 
        str[DBG_INFO_LEN_MAX-2] = '\n';
        sizeCnt = DBG_INFO_LEN_MAX;
    #if 0  /* 超过1k的按照1k打印,不再屏蔽打印信息 */
        sizeCnt += sprintf(str + sizeCnt, 
            "!!!print string too large, _util_debug error!!!");
        sizeCnt += sprintf(str + sizeCnt, "!!!!!!!!!!!!!!!\n");
    #endif
    
    }
    else
    {
        sizeCnt += fmtCnt;
        if('\n' != str[sizeCnt-1])
        {
            str[sizeCnt] = '\n';
            str[sizeCnt+1] = 0x00;
            sizeCnt += 1;
        }
    }

#if defined(__linux__)
#ifdef  CONFIG_TARGET_DEVICE_TYPE_XSIM
    if (strstr(str, "%"))
    {
        log_msg_write("app", log_behavior_get(), "%s", str);
    }
    else
    {
        log_msg_write("app", log_behavior_get(), str);
    }

#else
	write(logfd, str, sizeCnt);
#endif
#elif defined (_WIN32)
	OutputDebugString((LPCSTR)str);
#else
#error	 /* TODO:*/
#endif
    return;
}
#if 0
/* @fn      util_dbg_version
 * @brief   assert be dbg or release version?.
 * @brief   Author/Date huangjs/2011-04-28.
 * @param   [in] N/A.
 * @param   [out] N/A.
 * @return  HPR_TRUE/HPR_FALSE.
 */
HPR_BOOL util_dbg_version(HPR_VOID)
{
    HPR_BOOL bNfsDbg = HPR_FALSE;
    
    #ifndef  CONFIG_SUPPORT_DBG_CTRL 
    /*release version, must set it HPR_FALSE.*/
    bNfsDbg = HPR_FALSE;
    #endif
	
    #ifdef DBG_VERSION_BY_NFS
    /*if need debug by NFS, set it HPR_TRUE.*/
    bNfsDbg = HPR_TRUE;
    #endif
    return bNfsDbg;
}


void util_debug_hex(unsigned char* pBuff, unsigned int bufLen)
{
	#ifdef  CONFIG_SUPPORT_DBG_CTRL
    unsigned int i = 0;
    fprintf(stderr, "\n-----------------------\n");
    for(i = 0; i<bufLen; i++)
    {
        if((0 == i%16))
        {
            fprintf(stderr,"\n %04d |", i+1);
        }
        fprintf(stderr, "%02x ", pBuff[i]&0xff);
    }
    fprintf(stderr, "\n-----------------------\n");
	#endif

    return;
}

/**   
 * \fn util_read_file_level
 * \brief 读取util debug的文件保存的level
 * \brief fenggx@2014-10-14
 * \param[in] pFile - 文件名
 * \param[out] pCtrl - 读取的内容
 * \return  n/a 成功 HPR_OK /失败 ErrNo
**/
HPR_INT32 util_read_file_level(HPR_INT8* pFile, UTIL_DBG_CTRL* pCtrl)
{
    HPR_INT32 iRet = HPR_ERROR;
#ifndef OS_WINDOWS_VERSION
    FILE *pDbgFile = NULL;
#ifndef PATH_MAX
    #define PATH_MAX 1024
#endif
    HPR_INT8 charbuffer[PATH_MAX];
    HPR_INT8 readbuffer[128];
    
    if(!pFile)
    {
        return HPR_ERROR;
    }

    pDbgFile = fopen(pFile, "r");
    if (NULL == pDbgFile) 
    {               
        //perror(pFile);
        getcwd(charbuffer, PATH_MAX);    
        //fprintf(stderr, "open write (%s)(%s)file ", charbuffer, pFile);
        return HPR_ERROR;
    }

    if(NULL == fgets(readbuffer, sizeof(readbuffer)-1, pDbgFile))
    {
        if(pDbgFile)
        {
            fclose(pDbgFile);
            pDbgFile = NULL;
        }
        return HPR_ERROR;
    }

    if(4 == sscanf(readbuffer, "0x%x 0x%x 0x%x 0x%x"
        , &pCtrl->iDbgLevel, &pCtrl->iDbgFormat
        , &pCtrl->iDbgModule , &pCtrl->iDbgMask))
    {
    /*
        fprintf(stdout, "read buffer [%s: 0x%x,0x%x,0x%x,0x%x]"
            , readbuffer, pCtrl->iDbgLevel, pCtrl->iDbgFormat
            , pCtrl->iDbgModule , pCtrl->iDbgMask); */
        iRet = HPR_OK;
    }
    else
    {/*
        fprintf(stderr, "sscanf (%s)Err ", readbuffer); */
        iRet = HPR_ERROR;
    }
    
    if(pDbgFile)
    {
        fclose(pDbgFile);
        pDbgFile = NULL;
    }
#endif
    return iRet;
}

void util_module_init(void)
{
	printf("####### util_module_init ##########\n");
	int logsize = 0;
	int sync_size = 0;
	int net_type = 0;
	char msg[128] = "module_init";
	
	logsize = 500 *1024*1024;
	//每次同步日志到flash的大小	
	sync_size  = LOG_MSG_SYNC_MINI_SIZE;

	sprintf(msg, LOG_MODULE_INIT_FORMAT_STRING, msg, logsize, sync_size, net_type);
	log_msg_write("app", LOG_LEVEL_CMD, msg);
}
#endif