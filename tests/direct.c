#include "tgt.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

    struct tgt_object *desktop;
    struct tgt_terminal *term;

void destroy()
{
    tgt_destroyobject(desktop);
    exit(0);
}

int main()
{
    struct tgt_object *window;
    struct tgt_object * direct;
    tgt_cell *dsurface;
    
    desktop=tgt_start(NULL,&term); /* Get root object */

    tgt_set(desktop,TGTT_DESKTOP_HIDECURSOR,1);
    
    window=tgt_createobject(term,TGT_CLASS_WINDOW, /* Create a window object */
	(tagitem[]){TGTT_X, 20, TGTT_Y, 5,
		 TGTT_XS,40, TGTT_YS,15,
		 TGTT_WINDOW_TITLE,(tgtt) "Direct surface example",
		 TGTT_CALLBACK,(tgtt) destroy,
		 TGTT_END,0});
    
    
    tgt_link(window,desktop); /* Attach it to the root object */
    direct=tgt_createandlink(window,TGT_CLASS_DIRECTSURFACE,
	(tagitem[]) { TGTT_X,2,TGTT_Y,2,
			TGTT_XS,36,TGTT_YS,11,
			TGTT_BG,TGT_COLOR_BLACK,TGTT_FG,TGT_COLOR_YELLOW,
			TGTT_END,0});

    tgt_refresh(desktop);    
    tgt_async_waitkeys(desktop);
    for(;;)
    {
	int i;
	tgt_get(direct,TGTT_DIRECTSURFACE_SCREENMEM,(tgtt*) &dsurface);
	for(i=0;i<396;i++) 
	{
	    *(dsurface++)=TGT_T_BUILDCELL(rand()%8,rand()%8,rand()%2,rand()%2,'a'+i%24);
	}
	tgt_refresh(direct);
	usleep(10);
    }
}