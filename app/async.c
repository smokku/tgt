#include <stdio.h>
#include <pthread.h>
#include "tgt.h"

int async_running = 0;

#ifdef TGT_POSSIBLE_ASYNC
/*
 * tgt_async_waitkeys : tgt_waitkeys functionality in a thread
 *
 */

void tgt_async_waitkeys(struct tgt_object * obj)
{
    pthread_t pid;
    
    async_running = 1;
    pthread_create(&pid,NULL,(void*(*)(void*))tgt_waitkeys,obj);
}
#endif
