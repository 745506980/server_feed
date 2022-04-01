#include "protocol.h"
#include "mcu_protocol.h"
#include <stdio.h>
#include "myhead.h"

/*
    HX711数据串口协议处理

*/






INT32 Serial_Protocol_check_head(UINT8 buf[], UINT32 inlength)
{
    if (buf == NULL)
    {
        return -1;
    }
	if (buf[MCU_HEADER_FIRST_BYTE] == MCU_PROTOCOL_HEAD_FIRST_FIELD && \
	    buf[MCU_HEADER_SECOND_BYTE] == MCU_PROTOCOL_HEAD_SECOND_FIELD && \
	    buf[MCU_HEADER_VERSION_BYTE] == MCU_PROTOCOL_DEV_FIELD_V1)
	{
        return 0;
    }
    return -1;
}

/* 解析数据获取方向 命令 与数据长度 */
INT32 Serial_Protocol_analysis(UINT8 buf[], UINT32 length, UINT8 * oDirection, UINT16 * oCmd, UINT32 * oLength)
{
    UINT8 Direction = 0;
    UINT16 Cmd = 0;
    UINT32 Length = 0;
    INT32 Index = 0; //DIRECTION索引
    if (length != (MCU_PROTOCOL_DIRECTION_SIZE + MCU_PROTOCOL_CMD_SIZE + MCU_PROTOCOL_LENGTH_SIZE))
    {
        ERROR("length is error %d\n", length);
        return -1;
    }
    if (oDirection == NULL || oCmd == NULL || oLength == NULL)
    {
        ERROR("Invalid argument!\n");
        return -1;
    }

    Direction = buf[Index++];
    *oDirection = Direction;

    Cmd |= buf[Index++];
    Cmd |= (buf[Index++] << 8);
    *oCmd = Cmd;

    Length |= buf[Index++];
    Length |= (buf[Index++] << 8);
    Length |= (buf[Index++] << 16);
    Length |= (buf[Index++] << 24);


    *oLength = Length;

    return 0;
}


INT32 Master_Serial_Recv(INT32 fd, UINT8 *data, UINT32 length)
{



    return 0;
}

INT32 MCU_Serial_Send(const UINT8 data[], const UINT32 length)
{
    if (data == NULL)
    {
        return -1;
    }
    
    return 0;
}

