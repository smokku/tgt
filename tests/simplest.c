#include "tgt.h"
#include <stdio.h>

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
    

    desktop=tgt_start(NULL,&term); /* Get root object */

    
    window=tgt_createobject(term,TGT_CLASS_WINDOW,
    (tgtt[]) {TGTT_X, 20, TGTT_Y, 10,
		 TGTT_XS,40, TGTT_YS,6,
		 TGTT_WINDOW_TITLE, (tgtt) "Hello World",
		 TGTT_END,0}); /* Create a window object */

    tgt_link(window,desktop); /* Attach it to the root object */

    tgt_createandlink(window,TGT_CLASS_CHECKBOX,
    (tgtt[]) {TGTT_X, 3, TGTT_Y, 3,
		 TGTT_CHECKBOX_CAPTION, (tgtt) "Test",
		 TGTT_END,0}); /* Create a window object */
    tgt_createandlink(window,TGT_CLASS_SLIDER,
    (tgtt[]) {TGTT_X, 3, TGTT_Y, 4, TGTT_XS, 20,
		 TGTT_SLIDER_CAPTION, (tgtt) "Blah",
		 TGTT_SLIDER_TYPE,TGT_SLIDERF_SHOWVALUE,
		 TGTT_END,0}); /* Create a window object */


    window=tgt_createandlink(desktop,TGT_CLASS_WINDOW,
    (tgtt[]) {TGTT_X, 23, TGTT_Y, 13,
		 TGTT_XS,40, TGTT_YS,5,
		 TGTT_WINDOW_TITLE, (tgtt) "Hello World",
		 TGTT_END,0}); /* Create a window object */
    
    
    tgt_createandlink(window,TGT_CLASS_BUTTON, /* 2 in 1 ... Create and attach in one function */
	(tagitem[]){TGTT_X, 28, TGTT_Y, 3,
	         TGTT_BUTTON_CAPTION, (tgtt) "[Quit]",
		 TGTT_CALLBACK, (tgtt) destroy, /*This will be called when the button is pressed*/
		 TGTT_END,0});
    tgt_refresh(desktop);    
    tgt_waitkeys(desktop);
    return(1);
}