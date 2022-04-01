
#include <fcntl.h>
#include <stdio.h>
#include "serial.h"
#include <unistd.h>
#include <termios.h>
#include <string.h>
/*对串口进行配置
 * */
int Set_Serial(int fd, int nSpeed, int nBits, int nStop, char nCheck)
{
	struct termios ter;
	
	if (tcgetattr(fd, &ter) == -1)
	{
		perror("tcgetattr");
		return -1;
	}                                                                                                                                                                                                       

	//清空 ter 修改模式
	bzero(&ter, sizeof(ter));
	ter.c_cflag |= CLOCAL; // 修改控制模式，保证程序不会占用串口
	ter.c_cflag |= CREAD;	// (使用接收器)　能够从串口读取数据
	ter.c_cflag &= ~CSIZE;	//
	//设置波特率
	switch (nSpeed)
	{
		case 9600:
			cfsetispeed(&ter, B9600);
			cfsetospeed(&ter, B9600);
			break;
		case 115200:
			cfsetispeed(&ter, B115200);
			cfsetospeed(&ter, B115200);
			break;
		default:
			fprintf(stderr, "Speed 9600 or 115200\n");
			return -1;
	}
	//设置数据位
	switch (nBits)
	{
		case 5:
			ter.c_cflag |= CS5;
			break;
		case 6:
			ter.c_cflag |= CS6;
			break;
		case 7:
			ter.c_cflag |= CS7;
			break;
		case 8:
			ter.c_cflag |= CS8;
			break;
		default:
			fprintf(stderr, "Bits is Invalid parameter\n");
			return -1;
	}
	//　设置停止位
	switch (nStop)
	{
		case 1:
			ter.c_cflag &= ~CSTOPB;  //不设置即为一个停止位
			break;
		case 2:
			ter.c_cflag |= CSTOPB;  //　两个停止位;
			break;
		default:
			fprintf(stderr, "nStop Invalid parameter\n");
			return -1;		
	}
	//　设置校验位
	switch (nCheck)
	{
		case 'n':
		case 'N':
			ter.c_cflag &= ~PARENB;  //不启用奇偶校验　、
			ter.c_iflag &= ~INPCK;
			break;
		case 'e':
		case 'E':
			ter.c_cflag |= INPCK; // 启用校验
			ter.c_cflag |= PARENB;  //启用偶校验 
			ter.c_cflag &= ~PARODD;
			break;
		case 'o':
		case 'O':
			ter.c_cflag |= INPCK;
			ter.c_cflag |= PARENB;
			ter.c_cflag |= PARODD;
			break;
	}
	
	//设置等待时间和　最小接收字符
	ter.c_cc[VTIME] = 0;
	ter.c_cc[VMIN] = 1;  // 接收最小字符为1 相当于是阻塞
	
	tcflush(fd, TCIFLUSH); //如果发生数据溢出，接收数据，但是不读取

	//激活配置
	if (tcsetattr(fd, TCSANOW, &ter) == -1)
	{
		perror("tcsetattr");
		return -1;		
	}
	printf("seial set down!\n");
	return 0;
}




int Open_Serial(const char * path)
{
	int fd = open(path, O_RDWR | O_NOCTTY);
	if (fd == -1)
	{
		perror("open");
		return -1;
	}
#if 1    
	//
	//设置串口
	if (-1 == Set_Serial(fd, 9600, 8, 1, 'n'))
	{
		printf("set_uart is error!\n");
		return -1;
	}
	else 
	{

		printf("set_done!\n");
		return fd;
	}
#endif

    return fd;
}
