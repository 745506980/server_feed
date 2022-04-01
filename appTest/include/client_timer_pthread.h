#ifndef CLIENT_TIMER_PTHREAD__H
#define CLIENT_TIMER_PTHREAD__H


#define TIMER_INVAKID           (-1)
/* 
    定时器线程
    
 */


INT32 client_timer_pthread(void);

/* 更新喂食计划 */
void client_update_feedplan(UINT8 * feedplan, UINT32 length);


/* 找到距离当前时间最小的喂食计划并设置 闹钟*/

void client_update_timer(void);

#endif