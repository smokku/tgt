#include "tgt.h"
#include <stdio.h>

struct tgt_object *desktop;

void destroy()
{
    tgt_destroyobject(desktop);
    _exit(0);
}

main()
{
    struct tgt_terminal *term;
    struct tgt_object *window;
    desktop=tgt_start(NULL,&term); /* Get root object */
    
    window=tgt_createobject(term,TGT_CLASS_WINDOW, /* Create a window object */
	(long[]){TGTT_X, 20, TGTT_Y, 10,
		 TGTT_XS,40, TGTT_YS,5,
		 TGTT_WINDOW_TITLE,"Hello World",
		 TGTT_END,0});
    
    tgt_link(window,desktop); /* Attach it to the root object */
    
    tgt_createandlink(window,term,TGT_CLASS_BUTTON, /* 2 in 1 ... Create and attach in one function */
	(long[]){TGTT_X, 28, TGTT_Y, 3,
	         TGTT_BUTTON_CAPTION, "[Quit]",
		 TGTT_CALLBACK,(long) destroy, /*This will be called when the button is pressed*/
		 TGTT_END,0});
    tgt_refresh(desktop);    
    tgt_waitkeys(desktop);
}