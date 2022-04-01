/*************************************************************************
    > File Name: network.h
    > 作者:YJK 
    > Mail: 745506980@qq.com 
    > Created Time: 2021年10月14日 星期四 22时07分30秒
 ************************************************************************/

#ifndef NETWORK__H
#define NETWORK__H

#define MAXSLEEP 60
#define SERVER_PORT 5050
#define SERVER_IP1   "118.31.71.133"
#define SERVER_IP	 "172.17.182.232"


/** @fn    Function   : gui_imageset_create_ctrlwin
 *  @brief Description: create window's control
 *  @brief Auther/Time: zhangxinwang / 2019/10/10
 *  @param [in]iVoChan: 输出口[0-3]
 *  @param [in]hWnd: window's handle
 *  @param [in]wParam: message parameter
 *  @param [in]lParam: message parameter
 *  @return none
 */
BOOL recvmessage(int fd, unsigned char buf[], const unsigned int length);

/** @fn    Function   : gui_imageset_create_ctrlwin
 *  @brief Description: create window's control
 *  @brief Auther/Time: zhangxinwang / 2019/10/10
 *  @param [in]iVoChan: 输出口[0-3]
 *  @param [in]hWnd: window's handle
 *  @param [in]wParam: message parameter
 *  @param [in]lParam: message parameter
 *  @return none
 */
BOOL sendmessage(INT32 fd, const UINT8 direction,const UINT16 command, const UINT32 length, const UINT8 data[]);

int recvmessage_header(int fd, unsigned char buf[], const unsigned int length);

BOOL server_reply(int fd, const unsigned char direction, const unsigned short command, const unsigned char data);

BOOL client_reply(int fd, const unsigned char direction, const unsigned short command, const unsigned char data);

void close_socketfd(int sockfd);

INT32 Client_Network_Init(void);

#endif
/*防止头文件重复定义*/
