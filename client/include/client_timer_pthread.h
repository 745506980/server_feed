#ifndef CLIENT_TIMER_PTHREAD__H
#define CLIENT_TIMER_PTHREAD__H


#define TIMER_INVAKID           (-1)
#define TIMER_ONEWEEK           (604800)

/* 
    定时器线程
    
 */

#define FEEDPLAN_FILEPATH   ("/home/config/feedplan")

INT32 client_timer_pthread(void);

/* 更新喂食计划 */
void client_update_feedplan(UINT8 * feedplan, UINT32 length);


/* 找到距离当前时间最小的喂食计划并设置 闹钟*/

void client_update_timer(void);

#endif