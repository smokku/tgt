#include <stdio.h>
#include "tgt.h"

int async_running=0;

#ifdef TGT_POSSIBLE_ASYNC
void tgt_async_waitkeys(struct tgt_object * obj)
{
    int pid;
    async_running=1;
    pthread_create(&pid,NULL,tgt_waitkeys,obj);
}
#endif
