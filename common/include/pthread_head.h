

#ifndef PTHREAD_HEAD__H
#define PTHREAD_HEAD__H

#include <pthread.h>
#include "datatype.h"

#define MUTEX_LOCK(x)	do{					\
		pthread_mutex_lock(x);				\
	}while(0);								\
	
#define MUTEX_UNLOCK(x) do {				\
		pthread_mutex_unlock(x);			\
	}while(0);								\



#endif

