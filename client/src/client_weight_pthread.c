#include "client_weight_pthread.h"
#include "mcu_protocol.h"
#include "myhead.h"
#include "client.h"
#include "serial.h"
static INT32 OldWeight = 0;
pthread_mutex_t datamutex = PTHREAD_MUTEX_INITIALIZER;

INT32 Serial_Get_Weight(void)
{
    INT32 iWeight = 0;
    pthread_mutex_lock(&datamutex);
    iWeight = OldWeight;
    pthread_mutex_unlock(&datamutex);

    return iWeight;
}
static INT32 Serial_Protocol_Proc(UINT8 Direction, UINT16 CMD, UINT32 Length, UINT8 data[])
{
    INT32 Index = 0;
    INT32 i = 0;
    INT32 Crc = 0;
    INT32 TmpWeight = 0;
    switch(CMD)
    {
        /* 获取余量重量 */
        case MCU_PROTOCOL_CMD_GET_GRAIN_WEIGHT:
        {
            Crc = 0;
            for (i = 0; i < Length - 1; i++)
            {
                Crc += data[i];
            }
            if (data[Length - 1] == (Crc % 128))
            {
                pthread_mutex_lock(&datamutex);
                
                TmpWeight = 0;
                TmpWeight |= (INT8)data[Index++];
                TmpWeight |= (INT8)(data[Index++] << 8);
                TmpWeight |= (INT8)(data[Index++] << 16);
                TmpWeight |= (INT8)(data[Index++] << 24);
                if (TmpWeight != OldWeight)
                {
					sendmessage(sockfd, FEEDER_TO_APP, ID_GRAIN_WEIGHT, sizeof(TmpWeight), (UINT8 *)&TmpWeight);
                    OldWeight = TmpWeight;
                }
                
                //DEBUG("Weight %d\n", OldWeight);
                
                pthread_mutex_unlock(&datamutex);
            }
            else
            {
                ERROR("Crc is error data crc [%d] Crc [%d] \n", Crc % 128, data[Length - 1]);
            }
        }
    }
    return 0;
}

void Send_Grain_Weight(void)
{    
    INT32 Weight = 0;
    Weight = Serial_Get_Weight();
    /* 发送余粮重量 */
#ifdef CLIENT_TEST	
	Weight = 150;
#endif 	
    sendmessage(sockfd, FEEDER_TO_APP, ID_GRAIN_WEIGHT, sizeof(Weight), (UINT8 *)&Weight);
    DEBUG("Grain Weight :%d\n", Weight);
}

void * client_weight_handle_proc(void * arg)
{
    int fd = -1;
	UINT8 recv[1024] = {0};                                                                                   	
    fd = Open_Serial(SERIAL_PATH);
	int ret = 1;
	UINT8 Direction = 0;
	UINT16 Cmd = 0;
	UINT32 Length = 0;
	UINT8 header = 0;
	struct timeval tim;
	fd_set readfds;
    if (fd == -1)
    {
        printf("open serial is error!!!\n");
        return NULL;
    }
	for ( ;; )
	{
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		tim.tv_sec = 10;
		tim.tv_usec = 0;
		ret = select(fd + 1, &readfds, NULL, NULL, &tim);
		if (ret == -1)
		{
			ERROR("select is error!!!!\n");
		}
		/*1、首先读取头部数据 */
        ret = read(fd, &header, 1);					
        if (-1 == ret)  
        {
            ERROR("read header is error!\n");
		}
		if (MCU_PROTOCOL_HEAD_FIRST_FIELD == header)
		{
			ret = read(fd, &header, 1);					
			if (-1 == ret)  
			{
				ERROR("read header is error!\n");
			}
			if (MCU_PROTOCOL_HEAD_SECOND_FIELD == header)
			{
				ret = read(fd, &header, 1);					
				if (-1 == ret)  
				{
					ERROR("read header is error!\n");
				}
				if (MCU_PROTOCOL_DEV_FIELD_V1 == header)
				{
					ret = read(fd, recv, MCU_PROTOCOL_PARAMETER_SIZE);
					if (-1 == ret)  
					{
						ERROR("read parameter is error!\n");
					}
					if (-1 != Serial_Protocol_analysis(recv, ret, &Direction, &Cmd, &Length))
					{
						//DEBUG("DIRECTION [0x%x] CMD [0x%x] LENGTH [0x%x]\n", Direction, Cmd, Length);
						read(fd, recv, Length);
						if (-1 == ret)  
						{
							ERROR("read parameter is error!\n");
						}
						Serial_Protocol_Proc(Direction, Cmd, Length, recv);
					}
					else
					{
						ERROR("parameter is error!!\n");
					}
				}
				else
				{
					ERROR("Header 3 is error [0x%x]\n", header);
				}
			}
			else
			{
				ERROR("Header 2 is error [0x%x]\n", header);
			}
		}
		else
		{
			ERROR("HEAD IS ERROR 0x%x\n", header);
		}
	
#if 0
		/* 头部校验成功读取剩下的数据 */
		if (-1 != Serial_Protocol_check_head(recv, MCU_PROTOCOL_HEADER_SIZE))
		{
			read(fd, recv, MCU_PROTOCOL_PARAMETER_SIZE);
			if (-1 == ret)  
        	{
            	ERROR("read parameter is error!\n");
			}
			if (-1 != Serial_Protocol_analysis(recv, MCU_PROTOCOL_PARAMETER_SIZE, &Direction, &Cmd, &Length))
			{
				read(fd, recv, Length);
				if (-1 == ret)  
				{
					ERROR("read parameter is error!\n");
				}
				Serial_Protocol_Proc(Direction, Cmd, Length, recv);
			}
			else
			{
				ERROR("parameter is error!!\n");
			}
		}
		else
		{
			for (i = 0; i < MCU_PROTOCOL_HEADER_SIZE;i++)
			{
				printf("recv[%d]:%d\n", i, recv[i]);
			}
			ERROR("HEAD is error\n");
		}
	

#endif
	}
    return NULL;
}

/* HX711传感器数据处理线程 */
INT32 client_weight_pthread(void)
{
    INT32 err = 0;	
	pthread_t tid = 0;
	pthread_attr_t tattr;
	memset(&tattr, 0, sizeof(tattr));
	err = pthread_attr_init(&tattr);
	if (err != 0){
		ERROR_EXIT("pthread_attr_init\n");
	}
	err = pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
	if (err != 0){
		ERROR_EXIT("pthread_attr_setdetachstate\n");
	}
	err = pthread_create(&tid, &tattr, client_weight_handle_proc, NULL);
	if (err != 0) {
		ERROR_EXIT("pthread_create\n");
	}
	return err;
}