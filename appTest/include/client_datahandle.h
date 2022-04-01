

#ifndef CLIENT_HANDLE__H
#define CLIENT_HANDLE__H


/** @fn    Function   : client_handle_proc
 *  @brief Description: 接收服务器发送数据
 *  @brief Auther/Time: yuanjunke / 2021-10-24
 *  @param [in]fd: 服务器fd
 *  @return none
 */

void client_handle_proc(INT32 sockfd);


#endif