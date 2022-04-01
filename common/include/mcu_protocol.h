#ifndef MCU_PROTOCOL__H
#define MCU_PROTOCOL__H
#include "datatype.h"

/* |HEAD|DEV|DIRECTION|CMD|LENGTH|DATA|CRC| */
/*
HEAD 2Byte 固定头

0xAA 0xBB

DEV 1Byte  版本号

0x01

DIRECTION  1Byte 方向

0x01  MCU --> 主控
0x02  主控 --> MCU

CMD     2Byte 命令

0x01    余粮重量
...     后续拓展

LENGTH  4Byte 长度

DATA   1024   数据最大值

CRC  数据之和%128

*/
/* 字节大小 */

#define MCU_PROTOCOL_HEAD_SIZE          2   //固定头长度  HEAD DEV 

#define MCU_PROTOCOL_DEV_SIZE           1   //版本号

#define MCU_PROTOCOL_HEADER_SIZE        (MCU_PROTOCOL_HEAD_SIZE + MCU_PROTOCOL_DEV_SIZE)

#define MCU_PROTOCOL_DIRECTION_SIZE     1   //方向  

#define MCU_PROTOCOL_CMD_SIZE           2   //命令

#define MCU_PROTOCOL_LENGTH_SIZE        4   //数据长度

#define MCU_PROTOCOL_PARAMETER_SIZE     (MCU_PROTOCOL_DIRECTION_SIZE + MCU_PROTOCOL_CMD_SIZE + MCU_PROTOCOL_LENGTH_SIZE)


#define MCU_PROTOCOL_DATA_SIZE          1024    //数据大小

#define MCU_PROTOCOL_CRC_SIZE           1   //CRC校验和


#define MCU_HEADER_FIRST_BYTE           0
#define MCU_HEADER_SECOND_BYTE          1
#define MCU_HEADER_VERSION_BYTE         2

#define MCU_DIRECTION_BYTE              3

/* 固定头 */
#define MCU_PROTOCOL_HEAD_FIRST_FIELD   0xAA

#define MCU_PROTOCOL_HEAD_SECOND_FIELD  0xBB

/* 版本号 */
#define MCU_PROTOCOL_DEV_FIELD_V1       0x1

/* 方向 */
//#define MCU_PROTOCOL_DIRECTION_MCU_TO_MASTER    0x1 //MCU to 主控

//#define MCU_PROTOCOL_DIRECTION_MASTER_TO_MCU    0x2

typedef enum
{
    MCU_PROTOCOL_DIRECTION_MCU_TO_MASTER = 0x1,
    MCU_PROTOCOL_DIRECTION_MASTER_TO_MCU,

}MCU_PROTOCOL_DIRECTION;


/* 命令 */
typedef enum
{
    MCU_PROTOCOL_CMD_GET_GRAIN_WEIGHT = 0x1, //获取余粮重量

}MCU_PROTOCOL_CMD;


typedef struct {
    UINT8 Direction;
    UINT16 Cmd;
    UINT32 Length;
}MCU_;


//INT32 MCU_Serial_Recv();

INT32 Master_Serial_Recv(INT32 fd, UINT8 *data, UINT32 length);

INT32 MCU_Serial_Send(const UINT8 data[], const UINT32 length);

INT32 Serial_Get_Weight(void);

/* 解析数据获取方向 命令 与数据长度 */
INT32 Serial_Protocol_analysis(UINT8 buf[], UINT32 length, UINT8 * oDirection, UINT16 * oCmd, UINT32 * oLength);

INT32 Serial_Protocol_check_head(UINT8 buf[], UINT32 inlength);

#endif