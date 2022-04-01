#ifndef SERIAL__H
#define SERIAL__H


/*对串口进行配置
 * */
int Set_Serial(int fd, int nSpeed, int nBits, int nStop, char nCheck);


int Open_Serial(const char * path);


#endif
