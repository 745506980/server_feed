#include "client_humiture_pthread.h"
#include "myhead.h"
#include "client.h"
#include "protocol.h"
#include "client_weight_pthread.h"
static int fd_dht11;
/*获取温湿度*/
int client_get_humiture(unsigned char *temperature, unsigned char * humidity)
{
    unsigned char buf[5] = {0};
    int ret = read(fd_dht11, buf, 5);
    if (ret == -1)
    {
    //    perror("read1");
    //    exit(-1);
    }
    /*校验和*/
    int crc = buf[0] + buf[1] + buf[2] + buf[3];
    int j = 0;
    while ((crc & 0xFF) != buf[4])
    {
        sleep(2);
        ret = read(fd_dht11, buf, 5);
        if (ret == -1)
        {
                perror("read2");
                return -1;
        }
        crc = buf[0] + buf[1] + buf[2] + buf[3];
        j++;
        if (j == 3) //连续3次出错，重新打开传感器
        {
                close(fd_dht11);
                fd_dht11 = open(HUMITURE_PATH, O_RDWR);
                printf("open dht11 %d\n", fd_dht11);
                j = 0;
        }
    }
    //数据正确返回温湿度数据
    memcpy(temperature, &buf[2], 1);
    memcpy(humidity, &buf[0], 1);
    return 0;
}

void * client_humiture_handle_proc(void * arg)
{
    unsigned char temperature = 0;
    unsigned char humidity = 0;
    fd_dht11 = open(HUMITURE_PATH, O_RDWR);
    if (-1 == fd_dht11)
    {
        ERROR("open humiture is error!\n");
    }
    DEBUG("############################humiture pthread!!!!!!!!!!!!!!!!\n");
    for( ; ; )
    {
        /* 获取温湿度数据 */
        if (0 != client_get_humiture(&temperature, &humidity))
        {
            ERROR("client_get_humiture\n");
        }
        /*数据上报*/
        DEBUG("#####################3humiture wendu:%d  shidu:%d\n", temperature, humidity);
        #ifdef CLIENT_TEST
        temperature = 15;
        humidity = 50;
        /* 温度数据上报 */
        sendmessage(sockfd, FEEDER_TO_APP, ID_TEMPERATUER_DATA_REPORT, sizeof(temperature), &temperature);
        /* 湿度数据上报 */
        sendmessage(sockfd, FEEDER_TO_APP, ID_HUMIDITY_DATA_REPORT, sizeof(humidity), &humidity);
        /* 余粮数据更新 */
        Send_Grain_Weight();
        #else
                /* 温度数据上报 */
        sendmessage(sockfd, FEEDER_TO_APP, ID_TEMPERATUER_DATA_REPORT, sizeof(temperature), &temperature);
        /* 湿度数据上报 */
        sendmessage(sockfd, FEEDER_TO_APP, ID_HUMIDITY_DATA_REPORT, sizeof(humidity), &humidity);
        /* 余粮数据更新 */
        Send_Grain_Weight();
        #endif

        sleep(10);
    }
    return NULL;
}

/* 温湿度传感器数据处理线程 */
int client_humiture_pthread(void)
{
    int err = 0;	
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

	err = pthread_create(&tid, &tattr, client_humiture_handle_proc, NULL);
	if (err != 0) {
		ERROR_EXIT("pthread_create\n");
	}
	
	return err;

}

