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

/*
   this shows the EASIER way to create GUI's using TGT ... however notice that's
   not the simplest way ... you can ask then: why it's named '2' ? Well ...
   things shown in the third example are less complicated, however they require
   the programmer to type many characters...
   
   !!!!!!
   THIS IS A PROPAGANDA EXAMPLE, don't try to understand it if you're on the
   first stage of TGT education (ie. if you have just read 1.c) ... It's
   designed only to show you that TGT is a good solution for Very Lazy
   Programmers ;)
   !!!!!!
*/

#include "tgt.h"
#include <stdio.h>
#include <stdlib.h>

struct tgt_object * create_first();
struct tgt_object * create_second();
/* These are prototypes provided by TGMLP-generated source */

struct tgt_object * desk;
struct tgt_object * win[2];
/* these will hold our objects' pointers */

void command(struct tgt_object *, int);
/* this has been specified in 2.tgml as a callback for all controls */
void winstate(int, int);
/* see below */

int main(int argc, char ** argv)
{
    
    desk = tgt_start(NULL);
    tgt_set(desk, TGTT_ID, 5);
    tgt_set(desk, TGTT_CALLBACK, (tgtt) command);
    /* the desktop callback is called whenever user presses a key that is
       ununderstandable for TGT, so in this case, pressing such a key will
       do the same thing like pressing button with id=5 (which is
       "Open both windows" */
    
    win[0] = create_first();
    win[1] = create_second();
    /* ... call the code generated by TGMLP ... */
    
    tgt_link(win[0], desk); /* open the first window */
    
    tgt_refresh(desk);
    tgt_waitkeys(desk); /* wait for action */
    return(0);
}

/* this is called whenever user hits a control in our application, because
   it was specified as callback= parameter inside 2.tgml */

void command(struct tgt_object * obj, int n)
{
    struct tgt_object * o;

    switch(obj->id)
    {
	case 1: winstate(501, n); break; /* open/close the second window */
	case 2: winstate(500, n); break; /* open/close the first window */
	case 3:
	    /* close all windows */
	    if((o = tgt_findchild(desk, 500))) tgt_unlink(o);
	    /* close window with ID = 500 (the first one, see 2.tgml) */
	    if((o = tgt_findchild(desk, 501))) tgt_unlink(o);
	    /* close window with ID = 501 (the second one, see 2.tgml) */
	    
	    tgt_set(tgt_findchild(win[0], 1), TGTT_CHECKBOX_ACTIVE, 0);
	    tgt_set(tgt_findchild(win[1], 2), TGTT_CHECKBOX_ACTIVE, 0);
	    /* move all checkboxes to unselected state [ ] */
	    
	    tgt_refresh(desk);
	    break;
	case 4:
	    /* quit application */
	    tgt_destroyobject(desk);
	    exit(0);
	    break;
	case 5: 
	    /* reopen all */

	    winstate(500, 1); /* open 1st window */
	    winstate(501, 1); /* open 2nd window */

	    tgt_set(tgt_findchild(win[0], 1), TGTT_CHECKBOX_ACTIVE, 1);
	    tgt_set(tgt_findchild(win[1], 2), TGTT_CHECKBOX_ACTIVE, 1);
	    /* move checkboxes to appropiate states */
	    tgt_refresh(tgt_findchild(win[0], 1));
	    tgt_refresh(tgt_findchild(win[1], 2));
	    /* refresh checkboxes */
	    break;
    }
    return;
}

/* this function:
    if n is true:
       opens window no. id , if it's not already opened
    if n is false:
       closes window no. id, if it's opened
*/

void winstate(int id, int n)
{
    struct tgt_object * obj;

    if(n)
    {
	/* open the window */
	if(! tgt_findchild(desk, id))  /* is it currently opened ? */
	{
	    /* no? then link appropiate object to the desktop */
	    tgt_link(win[id - 500], desk);
	    tgt_refresh(win[id - 500]);
	}
    }
    else
    {
	/* close the window */
	if((obj = tgt_findchild(desk, id))) /* is it actually opened ? */
	{
	    tgt_unlink(obj); /* if so, close it */
	    tgt_refresh(desk);
	}
    }
    return;
}