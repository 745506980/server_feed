#ifndef CLIENT_WEIGHT_PTHREAD__H
#define CLIENT_WEIGHT_PTHREAD__H

#include "datatype.h"
/* 
    HX711 数据由串口进行收发
 */

#define SERIAL_PATH  "/dev/ttySAC1"   








void Send_Grain_Weight(void);

/* HX711传感器数据处理线程 */
INT32 client_weight_pthread(void);


#endif