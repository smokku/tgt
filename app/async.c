#include <stdio.h>
#include "tgt.h"

#ifdef TGT_POSSIBLE_ASYNC
void tgt_async_waitkeys(struct tgt_object * obj)
{
    int pid;
    pthread_create(&pid,NULL,tgt_waitkeys,obj);
}
#endif
