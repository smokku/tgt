#include "tgt.h"
#include <stdio.h>

    struct tgt_object *desktop;
    struct tgt_terminal *term;

void destroy()
{
    tgt_destroyobject(desktop);
    _exit(0);
}

main()
{
    struct tgt_object *window;
    struct tgt_object * direct;
    unsigned char *dsurface;
    
    desktop=tgt_start(NULL,&term); /* Get root object */

    tgt_set(desktop,TGTT_DESKTOP_HIDECURSOR,1);
    
    window=tgt_createobject(term,TGT_CLASS_WINDOW, /* Create a window object */
	(tagitem[]){TGTT_X, 20, TGTT_Y, 5,
		 TGTT_XS,40, TGTT_YS,15,
		 TGTT_WINDOW_TITLE,"Direct surface example",
		 TGTT_CALLBACK,destroy,
		 TGTT_END,0});
    
    
    tgt_link(window,desktop); /* Attach it to the root object */
    direct=tgt_createandlink(window,term,TGT_CLASS_DIRECTSURFACE,
	(tagitem[]) { TGTT_X,2,TGTT_Y,2,
			TGTT_XS,36,TGTT_YS,11,
			TGTT_BG,TGT_COLOR_BLACK,TGTT_FG,TGT_COLOR_YELLOW,
			TGTT_END,0});

    tgt_refresh(desktop);    
    tgt_async_waitkeys(desktop);
    for(;;)
    {
	int i;
	tgt_get(direct,TGTT_DIRECTSURFACE_SCREENMEM,&dsurface);
	for(i=0;i<64;i++) { *(dsurface++)=(i & 7) << 4| (i>>3) + rand()%2*8; *(dsurface++)='a'+i%24; }
	tgt_refresh(direct);
	usleep(10);
    }
}