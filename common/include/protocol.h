/*
    协议定义
    2021/07/14
*/

#ifndef PROTOCOL__H
#define PROTOCOL__H
#include "datatype.h"


#define V1		0x1
/*
    固定头
    0xFF 0xAA 0xBB 0xCC
*/
/*
    版本号
    0x1
*/
/*
    数据方向
    0x1 app->喂食器
    0x2 喂食器->app
    0x3 喂食器->服务器
*/
/*
    命令字
    0x1     手动喂食
    0x2     喂食计划
    0x3     控制小夜灯
    0x4     温度上报
    0x5     湿度上报
    0x6     喂食结果上报
    0x7     获取服务器时间
*/
/*
    数据长度
*/


typedef struct{
    UINT8 direction;    //方向
    UINT16 command;     //命令字
    UINT32 length;      //数据长度
}dcl;    //12字节


/* 喂食计划 */
typedef struct 
{
	UINT8 uWeekDay; /* 使用低7位，每一位表示一周中的一天,例如周一、周二 周日 为第0位  0000 0110 */
	UINT8 uHour;	/* 小时 0-23 */
	UINT8 uMin;		/* 分钟 0-59 */
	UINT8 uFeedNum;	/* 喂食份数 1 - 10 */
	UINT8 uSwitch;	/* 开关 1开 0关 */
}PROTOCOL_FEED_PLAN_ST;

#define FEED_PLAN_SIZE		(sizeof(PROTOCOL_FEED_PLAN_ST))

#if 0

typedef enum {
	PROTOCOL_SUNDAY = 0x0,	/* 周日 */
	PROTOCOL_MONDAY,		/* 周一 */
	PROTOCOL_TUESDAY,		/* 周二 */
	PROTOCOL_WEDNESDAY,		/* 周三 */
	PROTOCOL_THURSDAY,		/* 周四 */
	PROTOCOL_FRIDAY,		/* 周五 */
	PROTOCOL_SATURDAY,		/* 周六 */
	
}PROTOCOL_WEEKDAY_E;

#endif
/* 喂食计划中是在一周中的那一天 */

#define PROTOCOL_SUNDAY_SWITCH					(0x1)	/* 周日 */
#define PROTOCOL_MONDAY_SWITCH					(0x2) 	/* 周一 */
#define	PROTOCOL_TUESDAY_SWITCH					(0x4)	/* 周二 */
#define PROTOCOL_WEDNESDAY_SWCITCH				(0x8)	/* 周三 */
#define PROTOCOL_THURSDAY_SWITCH				(0x10)	/* 周四 */
#define PROTOCOL_FRIDAY_SWITCH					(0x20)	/* 周五 */
#define	PROTOCOL_SATURDAY_SWITCH				(0x40)	/* 周六 */

#define PROTOCOL_ALLDAY_SWITCH					(0xEF)	/* 一周全天 */

#define PROTOCOL_WEEKDAY_MAX 					(0x7)

/* 喂食计划最多10个 */
#define PROTOCOL_FEED_PLAN_MAX			(10)

/* 喂食记录 */
typedef struct 
{
	UINT8 uFeedNum;	/* 喂食份数 */
	UINT8 uYears;	/* 年份 自2021年起 */
	UINT8 uMon;		/* 月份 0-11 */
	UINT8 uDay;		/* 天 1 - 31 */
	UINT8 uHour;	/* 小时 0 - 23 */
	UINT8 uMin;		/* 分钟 */
	UINT8 uSec;		/* 秒 */
}PROTOCOL_FEED_RESULT_ST;



/*****************************各个字段长度****START*******************************/

//固定头长度
#define FIXED_HEADER_SIZE               5

//后半部长度
#define FRONT_HEADER_SIZE               7

//协议头长度 12字节
#define PROTOCOL_HEADER_SIZE            12

#define DATA_BUF_SIZE					1021 /* 数据长度为1021字节 */
/* 校验和 */
#define DATA_CRC_SIZE					1
//消息大小
#define MAX_DATA_BUF_SIZE                   DATA_BUF_SIZE + DATA_CRC_SIZE

/*****************************各个字段长度****END*******************************/

#define CRC_NUMBER 						128

/*命令字定义*/
/*****************************命令字****START**********************************/
#if 0
//手动喂食
#define ID_MANUAL_FEED                  0x1
//喂食计划
/*
第一个byte[1Byte周数]:使用前7位，每一位表示一周中的一天
例如周一、周二 1100 0000      十六进制:0xC0
第二、三个byte[1Byte小时][1Byte分钟]：例如6：30，表示为 0000 00110 0001 1110
十六进制表示为：0x061E
第四个byte[1byte喂食份数]:喂食份数0-12
第五个byte[1byte开关]：1开 0关
例如"每周周一周二6：30喂食5份"数据表示为:0xC0061E0500
*/
#define ID_FEED_PLAN                    0x2
//控制小夜灯 0关 1开
#define ID_CONTROL_NIGHT_LIGHT          0x3
//温度数据上报
#define ID_TEMPERATUER_DATA_REPORT      0x4
//湿度数据上报
#define ID_HUMIDITY_DATA_REPORT         0x5

//获取本地时间
#define ID_GET_LOCAL_TIME               0x6

//设备发送自身信息
#define ID_FACILITY_INFORMATION			0x7

//APP连接目标设备

#define ID_APP_CONN_FEED				0x8

//喂食结果上报
/*
[1Byte喂食份数]+[1Byte年份(自2020年起)]+[1Byte月份]+[1Byte周]+[1Byte小时]+[1Byte分钟]+[1Byte秒]
 例如2021年6月20日12:30:30喂食2份  数据表示为0x020106130C1E1E
*/
#define ID_FEED_RESAULT_REPORT          0x9
// 心跳数据
#define ID_HEATBEAT_REPORT				0xa
// 自动亮灯
#define ID_AUTO_LIGHT                   0xB
// 快速喂食开关
#define ID_QUICK_FEED					0xC
// 喂食器是否在线
#define ID_FEED_CONN_STATUS				0xD
#endif
typedef enum 
{
	ID_MANUAL_FEED = 0x1,  	//手动喂食
	ID_FEED_PLAN,			//喂食计划
	ID_CONTROL_NIGHT_LIGHT, //小夜灯控制
	ID_TEMPERATUER_DATA_REPORT, 
	ID_HUMIDITY_DATA_REPORT,
	ID_GET_LOCAL_TIME,
	ID_FACILITY_INFORMATION,
	ID_APP_CONN_FEED,
	ID_FEED_RESAULT_REPORT,
	ID_HEATBEAT_REPORT,
	ID_AUTO_LIGHT,
	ID_QUICK_FEED,
	ID_FEED_CONN_STATUS,
	ID_GRAIN_WEIGHT,	//余粮重量
}ID_CMD;	//命令


/*****************************命令字****END***********************************/

#define MAX_HEATBEAT_NUM				0x3

/*****************************固定头数据****START*******************************/

/*
    固定头字段
*/
//固定头第一个字段
#define HEADER_FIRST_FIELD              0xFF
//固定头第二个字段
#define HEADER_SECOND_FIELD             0xAA
//固定头第三个字段
#define HEADER_THIRD_FIELD              0xBB
//固定头第四个字段
#define HEADER_FOURTH_FIELD             0xCC

/*
    版本号字段
*/
#define HEADER_VERSION_FIELD            0x1


/*****************************固定头数据****END*******************************/


/*****************************数据方向****START*******************************/

//APP到喂食器
#define APP_TO_FEEDER                   0x1
//喂食器--->APP
#define FEEDER_TO_APP                   0x2

//APP<----->服务器
#define APP_TO_SERVER					0x3
//APP
//喂食器<--->服务器
#define FEEDER_TO_SERVER                0x4
/*****************************数据方向****START*******************************/

/*
    连接信息 APP or 喂食器
*/
//APP
#define APP_SIGN                         0x11
//设备
#define FEEDER_SIGN                      0x22

/*****************************固定头INDEX****START*******************************/

//固定头第一个字节
#define HEADER_FIRST_BYTE                0x0
//固定头第二个字节
#define HEADER_SECOND_BYTE               0x1
//固定头第三个字节
#define HEADER_THIRD_BYTE                0x2
//固定头第四个字节
#define HEADER_FOURTH_BYTE               0x3
//版本字段
#define HEADER_VERSION_BYTE              0x4
//方向字段
#define HEADER_DIRECTION_BYTE            0x5
//命令字段 2 字节
#define HEADER_COMMAND0_BYTE             0x6

#define HEADER_COMMAND1_BYTE             0x7
//数据长度字段 4Byte
#define HEADER_DATA_LENGTH0_BYTE         0x8

#define HEADER_DATA_LENGTH1_BYTE         0x9

#define HEADER_DATA_LENGTH2_BYTE         0xa

#define HEADER_DATA_LENGTH3_BYTE         0xb

/*****************************固定头****END***********************************/


#define RECV_BUF_SIZE 1024

#define SEND_BUF_SIZE 1024


/*****************************心跳包数据INDEX****START*****************************/

#define HEATBEAT_CHECK_BYTE				0x0


/*****************************心跳包数据INDEX****END*******************************/
/*****************************时间数据INDEX****START*******************************/
#if 0
//时间数据是否有效
#define TIME_STATUS						0x0
//年份字段
#define YEAR_BYTE						0x1
//月份字段
#define MONTH_BYTE						0x2
//日字段
#define DAY_BYTE						0x3
//小时字段
#define HOUR_BYTE						0x4
//分钟字段
#define MINUTE_BYTE						0x5
//秒数字段
#define SECOND_BYTE						0x6
#endif
typedef enum {
	TIME_STATUS = 0x0,
	YEAR_BYTE,
	MONTH_BYTE,
	DAY_BYTE,
	HOUR_BYTE,
	MINUTE_BYTE,
	SECOND_BYTE,
	TIME_BYTE_END,
}TIME_TYPE;

#define YEAR_START					(121)  /* time  NOW - 1900 - 121*/
/*****************************时间数据INDEX****END*******************************/


/*数据截断*/
//前半部截断
#define FRONT_HALF_TRUNCATION           0x1
//后半部截断
#define AFTER_HALF_TRUNCATION           0x2

#define SUCCESS 1;
#define FAILE -1;

#if 0

/*
    校验包头
    传入的参数为 header的字节数组
    length buf的长度
    obj 获取 方向 命令 和 数据长度
    成功返回 TRUE
    失败返回 FALSE
*/
BOOL header_handle(UINT8 buf[], const UINT32 length, dcl * obj);
/*
	读取data数据
	fd socket_fd
	buf 数据缓冲区
	length 数据的长度
	成功返回 TRUE
	失败返回 FALSE
*/

BOOL data_recv_handle(int fd, UINT8 buf[], const UINT32 length);
/*
	读取header数据
	fd socket_fd
	buf 数据缓冲区
	length 数据的长度
	成功返回 TRUE
	失败返回 FALSE
*/

int header_recv_handle(int fd, UINT8 buf[], const UINT32 length);
/*
	数据处理
	fd socket_fd
	buf 数据
	direction 方向
	command 命令
	length 数据长度

	成功返回 TRUE
	失败返回 FALSE
*/
BOOL data_handle(int fd, UINT8 buf[], const UINT8 direction, const UINT16 command, const UINT32 length);
/*
	封装一个send
	direction 方向
	command 命令
	length data 长度
	data 数据
*/
BOOL send_data(int fd, const UINT8 direction,const UINT16 command,const  UINT32 length, const UINT8 data[]);


//使用字节流

/*
	客户端信息处理
	主要是将设备的信息加入到链表中
	1、APP链表
	2、设备链表

*/
BOOL facilty_information_handle(int fd, UINT8 id[], const UINT16 command, const UINT8 direction, const UINT32 length);

/*
	APP与喂食器建立连接

	target_id 喂食器ID
	length 喂食器length

*/

BOOL app_conn_feed(int fd, const UINT8 direction, const UINT16 command, UINT8 target_id[],const UINT32 length);


/*
	处理客户端获取服务器时间请求
	//给客户端发送服务器的时间


*/
BOOL get_local_time(UINT8 time[], UINT32 length);


/*

	喂食结果处理
	feed 发送喂食结果，
	服务器将喂食结果转发给绑定feed的app

*/


BOOL feed_resault_handle(int fd, UINT8 feedresult[], const UINT8 direction, const UINT16 command, const UINT32 length);

/*
	发送
*/

#endif

BOOL protocol_header_check(UINT8 buf[], const UINT32 length, dcl * obj);

void debug_data(const UINT8 * data, int length);

void debug_protol(int fd, UINT16 command);

/* 校验数据 CRC校验*/
BOOL protocol_data_check(const UINT8 data[], const UINT32 length);
/* 生成校验和 */
void protocol_data_set_crc(const UINT8 data[], const UINT32 length, UINT8 * crc);

#endif

