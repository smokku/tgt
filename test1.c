#include "tgt.h"
#include "tgt_app.h"
#define MSG 1
#define QUIT 2

main()
{
    struct tgt_object *window;
    struct tgt_object *button;
    struct tgt_queue_msg msg;
    struct tgt_terminal *myterm;
    int i;

    myterm=tgt_setscreen((void*)0);
    tgt_rawcon();
    for(;;)
    {
	i=tgt_get_key(myterm);
	if(i=='q') break;
	printf("%d\n",i);
    }
    tgt_normalcon();
}