
#include "myhead.h"
static PMSGQUEUE __pMsgQueue = NULL;

PMSGQUEUE GetMsgQueue(void)
{
	return __pMsgQueue;
}
BOOL MsgQueue_init(void)
{
	__pMsgQueue = (PMSGQUEUE)malloc(sizeof(MSGQUEUE));
	if (__pMsgQueue != NULL)
	{
		__pMsgQueue->pFirstMsg = NULL;
		__pMsgQueue->pLastMsg = NULL;
			
		sem_init (&__pMsgQueue->wait, 0, 0);
		return TRUE;
	}
	else
	{
		ERROR("malloc");
		return FALSE;
	}
}

INT32 SendQueue(INT32 fd, UINT8 msg[], UINT32 length)
{
	if (msg == NULL)
		return FALSE;
	
    PMSGQUEUE pMsgQueue = GetMsgQueue();
    PQMSG  pqmsg;
    pqmsg = (PQMSG)malloc(sizeof(QMSG));
	if (pqmsg == NULL)
	{
		ERROR("malloc\n");
		return FALSE;
	}
	
	memset(pqmsg->Msg.msg, 0 , MSG_SIZE);
	pqmsg->Msg.fd = fd;
	memcpy(pqmsg->Msg.msg, msg, length);
    pqmsg->Msg.length = length;
    pqmsg->next = NULL;
    if (pMsgQueue->pFirstMsg == NULL) {
		
        pMsgQueue->pFirstMsg = pMsgQueue->pLastMsg = pqmsg;
    }
    else {
		
        pMsgQueue->pLastMsg->next = pqmsg;
        pMsgQueue->pLastMsg = pqmsg;
    }

	POST_MSGQ(pMsgQueue);
    return TRUE;
}

INT32 GetMessage(PMSGQUEUE MsgQueue, PMSG pMsg)
{
	if (NULL == MsgQueue || pMsg == NULL)
	{
		return FALSE;
	}
    PMSGQUEUE pMsgQueue = MsgQueue;
	PQMSG phead = NULL;
	WAIT_MSGQ(pMsgQueue);
    if (pMsgQueue->pFirstMsg) {
        phead = pMsgQueue->pFirstMsg;
        *pMsg = phead->Msg;
        pMsgQueue->pFirstMsg = phead->next;
        free (phead);
		phead = NULL;
    }
	return TRUE;
}
