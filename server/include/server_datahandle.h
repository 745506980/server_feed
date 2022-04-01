#ifndef SERVER_DATAHEANDLE__H
#define SERVER_DATAHEANDLE__H
#include "datatype.h"
/* 服务器数据转发处理  */


/* 数据处理 */
BOOL data_handle(INT32 fd, UINT8 buf[], const UINT8 direction, const UINT16 command, const UINT32 length);

#endif