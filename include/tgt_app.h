#ifndef TGT_APP_INCLUDED
#define TGT_APP_INCLUDED

#ifndef TGT_NOPTHREAD
void tgt_async_waitkeys(struct tgt_object *obj);
void tgt_async_stop(void);
#endif

struct tgt_queue_msg
{
    int code;
    long data;
    void *pointer;
};

struct tgt_queue
{
    int recv_pipe;
    int send_pipe;
};
#endif

