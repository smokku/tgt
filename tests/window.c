#include "tgt.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
struct tgt_object *desktop;

void destroy()
{
    tgt_destroyobject(desktop);
    _exit(0);
}

int main()
{
    struct tgt_terminal *term;
    struct tgt_object *window;
    int x,y,xs,ys,i;

    desktop=tgt_start(NULL,&term); /* Get root object */

    srand(time(NULL));
    
    for(i=0;i<10;i++)
    {
	xs=(rand() % (term->x_size-10))+10;
	ys=(rand() % (term->y_size-5))+5;
	x=rand() % (term->x_size-xs);
	y=rand() % (term->y_size-ys);
	window=tgt_createandlink(desktop,TGT_CLASS_WINDOW,
	(tgtt[]) {TGTT_WINDOW_TITLE, (tgtt) "Sample",
		 TGTT_CALLBACK,(tgtt) destroy,
		 TGTT_END,0}); /* Create a window object */
	tgt_set(window,TGTT_X,x);
	tgt_set(window,TGTT_Y,y);
	tgt_set(window,TGTT_XS,xs);
	tgt_set(window,TGTT_YS,ys);
    }
    tgt_refresh(desktop);    
    tgt_waitkeys(desktop);
    return(1);
}