#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include "tgt.h"
#include "tgt_app.h"
#ifdef TGT_QUEUES

struct tgt_queue * tgt_app_createqueue(void)
{
    struct tgt_queue *ret;
    int pipes[2];
    ret=(struct tgt_queue*) malloc(sizeof(struct tgt_queue));
    pipe(pipes);
    ret->recv_pipe=pipes[0];
    ret->send_pipe=pipes[1];
    return(ret);
}

void tgt_app_queue(struct tgt_queue *q,int code,long d1,void *d2)
{
    struct tgt_queue_msg m;
    m.code=code;
    m.data=d1;
    m.pointer=d2;
    write(q->send_pipe,&m,sizeof(struct tgt_queue_msg));
}

int tgt_app_waitmsg(struct tgt_queue *q,struct tgt_queue_msg *m)
{
    read(q->recv_pipe,m,sizeof(struct tgt_queue_msg));
    return(1);
}

int tgt_app_checkmsg(struct tgt_queue *q,struct tgt_queue_msg *m,unsigned int s,unsigned int us)
{
    struct timeval tv;
    fd_set fds;
    int r;
    FD_ZERO(&fds); FD_SET(q->recv_pipe,&fds);
    tv.tv_sec=s; tv.tv_usec=us;
    r=select(q->recv_pipe+1,&fds,NULL,NULL,&tv);
    if(r==0) return(0);
    read(q->recv_pipe,m,sizeof(struct tgt_queue_msg));
    return(1);
}

#endif