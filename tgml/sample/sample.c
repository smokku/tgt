
#include "tgt.h"
#include <stdio.h>

struct tgt_object * create_samplewindow();

/* Prototypes of functions, that'll be provided by TGML */

struct tgt_object * desk;

int command(struct tgt_object * obj, int p)
{
    if(obj->id==100)
    {
	tgt_destroyobject(desk);
	exit(0);
    }
}

int main(int argc, char ** argv)
{
    struct tgt_object * window;
    
    desk = tgt_start(NULL); /* Enable TGT */
    window = create_samplewindow(); /* Create form */
    tgt_link(window, desk); /* Place window on the desktop */
    tgt_refresh(desk); /* Update desktop  */
    tgt_waitkeys(desk); /* Wait for input */
}
