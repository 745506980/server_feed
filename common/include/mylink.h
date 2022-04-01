#ifndef LINK__H
#define LINK__H
#include "datatype.h"
#if 0
#define ID_SIZE 16

typedef struct node{
    INT32 fd;                 		    //文件描述符
    UINT8 id[ID_SIZE];          //设备ID
    UINT8 target_id[ID_SIZE];   //目标设备ID
    UINT32 id_length;		 	    //设备ID长度
    UINT8 num;				    //计数器，服务器每次发送心跳包num会+1， 如果num >= 3 则认为客户端无响应此时应释放该socket的资源
    struct node *next;
}Link_node;

/*
    头节点初始化
    分配内存空间
*/
BOOL link_init(Link_node **head, Link_node **end);

/*
    在尾部添加设备节点
    传入的节点指针，为尾节点指针
    插入数据为fd type 和 设备id
*/

BOOL link_append(Link_node *head, Link_node *end, const INT32 fd, const UINT8 id[], const UINT32 length);

/*
    根据fd删除一个设备节点
*/

BOOL link_delete(Link_node *head, const INT32 fd);

/*
    根据fd查找结点
*/
Link_node * get_fd_node(Link_node *head, const INT32 fd);

/*
    根据设备id查找结点
*/
Link_node * get_id_node(Link_node *head, const UINT8 id[]);
/*
	targetid APP需要连接的feed的id
	length targetid长度

*/
BOOL update_targetid_node(Link_node *head, const INT32 fd, const UINT8 targetid[], const UINT32 length);

/*
    输出所有节点数据
*/

void pri_all_node(Link_node *head);

Link_node * get_targetid_node(Link_node *head, const unsigned char targetid[]);

#endif
#endif

