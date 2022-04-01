#ifndef QUEUE__H
#define QUEUE__H
#include <semaphore.h>
#define MSG_SIZE 1024

/*
	消息发送线程采用消息队列，主线程将消息投递到消息队列中，
	发送线程将消息队列中的消息发送至客户端
*/
typedef struct _MSG{
	int fd;
	unsigned char msg[MSG_SIZE];
	unsigned int length;
}MSG;

typedef MSG * PMSG;

typedef struct _QMSG{
	MSG  	Msg;
	struct _QMSG * next;
}QMSG;

typedef QMSG * PQMSG;
	
typedef struct _MSGQUEUE{
	sem_t wait;
	PQMSG  pFirstMsg;
	PQMSG  pLastMsg;
}MSGQUEUE;

typedef MSGQUEUE* PMSGQUEUE;

#define WAIT_MSGQ(pMsgQueue) \
do {	\
	sem_wait(&(pMsgQueue)->wait);\
}while(0)
#if 0
#define POST_MSGQ(pMsgQueue) \
do { \
  int sem_value; \
  /* Signal that the msg queue contains one more element for reading */ \
  sem_getvalue (&(pMsgQueue)->wait, &sem_value); \
  if (sem_value <= 0) {\
	  sem_post(&(pMsgQueue)->wait); \
  } \
} while(0)
#endif

#define POST_MSGQ(pMsgQueue)    \
do {                        \
    sem_post(&(pMsgQueue)->wait);   \
}while(0)

PMSGQUEUE GetMsgQueue(void);
BOOL MsgQueue_init(void);

int SendQueue(int fd, unsigned char msg[], unsigned int length);
int GetMessage(PMSGQUEUE MsgQueue, PMSG pMsg);


#endif





