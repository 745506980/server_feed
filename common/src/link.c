
#include "mylink.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "debug.h"
#if 0
/*
    头节点初始化
    分配内存空间
*/

BOOL link_init(Link_node **head, Link_node **end)
{
    *head = malloc(sizeof(Link_node));
    if (NULL == *head)
        return FALSE;
    *end = *head;
    (*end)->next = NULL;
    return TRUE;
}
/*
    在尾部添加设备节点
    插入数据为fd type 和 设备id
*/
BOOL link_append(Link_node *head, Link_node *end, const INT32 fd, const UINT8 id[], const UINT32 length)
{
    Link_node * node = malloc(sizeof(Link_node));
    Link_node * node_exit = NULL;
    if (NULL == node)
        return FALSE;
    node_exit = get_id_node(head, id);
	if (node_exit == NULL) { //当前链表无该ID
	    node->fd = fd;
	    memcpy(node->id, id, length);
		memset(node->target_id, 0, ID_SIZE);
		node->id_length = length;
		node->num = 0;
	    end->next = node;
	    end = node;
	    end->next = NULL;
		return TRUE;
	} else { /* ID 已存在更新节点 */
        if (node_exit->num != 0)
        {
            node_exit->fd = fd;
            memcpy(node_exit->id, id, length);
            node_exit->id_length = length;
            node_exit->num = 0;
        }
		return TRUE;
	}
    return FALSE;
}
/*
    根据fd删除一个设备节点
*/
BOOL link_delete(Link_node *head, const INT32 fd)
{
    if (NULL == head)
        return IS_NULL;
    Link_node * pre = head;        //前驱节点
    Link_node * rear = pre->next;  //后继节点
    while(rear)
    {
        if (rear->fd == fd)
        {
            pre->next = rear->next;
            free(rear);
            rear=NULL;
            return TRUE;
        }
        pre = pre->next;
        rear = pre->next;
    }
    return FALSE;  //未找到
}
/*
    根据fd查找结点
*/
Link_node * get_fd_node(Link_node *head, const INT32 fd)
{
    if (NULL == head)
        return NULL;
    Link_node * node = head->next;
    while (node)
    {
        if (node->fd == fd)
            return node;
        node = node->next;
    }
    return NULL;    //未找到
}
/*
    根据设备id查找结点
*/
Link_node * get_id_node(Link_node *head, const UINT8 id[])
{
    if (NULL == head)
        return NULL;
    Link_node * node = head->next;
    while (node)
    {
        if ((strncmp((INT8 *)node->id, (INT8 *)id, ID_SIZE)) == 0)
            return node;
        node = node->next;
    }
    return NULL;
}

Link_node * get_targetid_node(Link_node *head, const unsigned char targetid[])
{
    if (NULL == head)
        return NULL;
    Link_node * node = head->next;
    while (node)
    {
        if ((strncmp((INT8 *)node->target_id, (INT8 *)targetid, ID_SIZE)) == 0)
            return node;
        node = node->next;
    }
    return NULL;

}

/*
	更新目标节点的num， 用于心跳反馈
*/
int update_fd_of_num_increase (Link_node *head, const int fd)
{


    return 0;
}


void pri_all_node(Link_node *head)
{
    Link_node * node = head->next;
    while (node)
    {
        printf("fd=%d, id=%s, target_id=%s, id_length%d\n", node->fd, node->id, node->target_id, node->id_length);
        node = node->next;
    }
}


#endif
