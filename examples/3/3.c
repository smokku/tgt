/* ----------------------------------------------------------------------------
**      2.c    TGT Usage Example
** ----------------------------------------------------------------------------
**      TGT                                       TextGUIToolkit Library
**      Copyright (C)   2000-2001       Tomasz Sterna and Mateusz Golicz
**
**      LGPL license
**
** ----------------------------------------------------------------------------
**      MAINTAINER      Mateusz Golicz
**
**      Log:
**      
**
*/

#include "tgt.h"
#include <stdio.h>
#include <stdlib.h>

int button1_fn(struct tgt_object *, int);
int button2_fn(struct tgt_object *, int);
int button3_fn(struct tgt_object *, int);

struct tgt_object * desktop; /* this'll store the root object */
struct tgt_object * window2;
int islinked = 0;

int main(int argc, char ** argv)
{
    struct tgt_object * window1;

    desktop = tgt_start(NULL); /* initialize TGT */

    window1 = tgt_createobject(TGT_CLASS_WINDOW, (tgtt[])
		{TGTT_WINDOW_TITLE, (tgtt) "Main window",
		 TGTT_XS, 30,
		 TGTT_YS, 7,
		 TGTT_END,0}); /* create first window */
   
    tgt_set(window1, TGTT_X, (tgt_getterminal()->x_size - 30) / 2);
    tgt_set(window1, TGTT_Y, (tgt_getterminal()->y_size - 7) / 2);
    /* center it on the screen */

    tgt_createandlink(window1, TGT_CLASS_BUTTON, (tgtt[])
		{TGTT_BUTTON_CAPTION, (tgtt) "[Open second window ]",
		 TGTT_X, 2, TGTT_Y, 3, /* position, relative to the window object */
		 TGTT_CALLBACK, (tgtt) button1_fn,
		 TGTT_END,0});

    tgt_createandlink(window1, TGT_CLASS_BUTTON, (tgtt[])
		{TGTT_BUTTON_CAPTION, (tgtt) "[Exit this stupid app]",
		 TGTT_X, 2, TGTT_Y, 4,
		 TGTT_CALLBACK, (tgtt) button2_fn,
		 TGTT_END,0});
		 
    /* tgt_createandlink works in the same way as tgt_createobject(), but
       additionally automatically attaches (using tgt_link) created object
       to object, which a pointer to was passed as the first argument,  */

    window2 = tgt_createobject(TGT_CLASS_WINDOW, (tgtt[])
		{TGTT_WINDOW_TITLE, (tgtt) "Second window",
		 TGTT_X, 0, TGTT_Y, 0,
		 TGTT_XS, 40, TGTT_YS, 4,
		 TGTT_END,0}); /* create second window */
   
    tgt_createandlink(window2, TGT_CLASS_BUTTON, (tgtt[])
               {TGTT_BUTTON_CAPTION, (tgtt) "[Close this window]",
	        TGTT_CALLBACK, (tgtt) button3_fn,
	        TGTT_X, 2, TGTT_Y, 1,
	        TGTT_END,0});
    tgt_createandlink(window2, TGT_CLASS_LABEL, (tgtt[])
		{TGTT_LABEL_TEXT, (tgtt) "Try to switch windows using `",
		 TGTT_X, 2, TGTT_Y, 2,
		 TGTT_END,0});

    tgt_link(window1, desktop); /* make the first window visible on the desktop */

    tgt_refresh(desktop);
    tgt_waitkeys(desktop); /* wait for action */
    return(0);
}

int button1_fn(struct tgt_object * obj, int q)
{
    if(!islinked) /* beware linking the second window two times */
    {
	tgt_link(window2, desktop);
	tgt_refresh(window2);
	islinked = 1;
    }
    return(1);
}
int button2_fn(struct tgt_object * obj, int q) /* our quit button callback */
{
    /* this'll be called, when that button is pressed */
    tgt_destroyobject(desktop); /* flush all objects */
    exit(0); /* quit */
    return(1);
}
int button3_fn(struct tgt_object * obj, int q)
{
    if(islinked) /* beware linking the second window two times */
    {
	tgt_unlink(window2);
	tgt_refresh(desktop);
	islinked = 0;
    }
    return(1);
}
