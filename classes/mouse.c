/* ----------------------------------------------------------------------------
**      mouse.c Mouse handling functions (using GPM)
** ----------------------------------------------------------------------------
**      TGT                                       TextGUIToolkit Library
**      Copyright (C)   2000-2001       Mateusz Golicz and Tomasz Sterna
**
**      LGPL license
**
** ----------------------------------------------------------------------------
**      MAINTAINER      Mateusz Golicz
**
**      Log:
**      - cleanup
**
*/

#include "tgt.h"
#ifdef TGT_GPMMOUSE
#include <stdio.h>
#include <unistd.h>
#include "gpm.h"

/*
 * tgt_init_mouse : Initialize connection with GPM
 *
 */

void tgt_init_mouse(void)
{
    unsigned char * tty;
    Gpm_Connect conn;
    
    conn.eventMask = GPM_DRAG | GPM_UP | GPM_DOWN; /* Events we want to receive */
    conn.defaultMask = GPM_MOVE; /* Events we want to keep their default
                                    behaviour */
    conn.maxMod = 0;
    conn.minMod = 0; /* We don't want to receive events with any modifier
                        keys pressed */
	    
    Gpm_Open(&conn,0);
}

static struct tgt_object * tgt_mouse_recv = NULL;
                       /* The object that will receive mouse events
		          (ie. the one that was under the pointer when user
			   pushed mouse button) */
static int tgt_mouse_x = 0, tgt_mouse_y = 0;
		       /* Mouse position, relative to object, at the
		          moment user clicked tgt_mouse_recv object */
static int tgt_mouse_buttons = 0;
			/* Buttons that were pressed at the moment of event */
static struct tgt_object * tgt_mouse_lock=NULL;
                       /* If not-NULL - do not allow to click with mouse
		          object other than this one */

/*
 *  tgt_mouse_click : Check whether one of object's children is under
 *                    the pointer.
 */


struct tgt_object * tgt_mouse_click(struct tgt_object * obj,
				    int x, /* Pointer position , relative to */
				    int y) /* obj */
{
    struct tgt_object * child = obj->ln.child;
    struct tgt_object * first_child;

    tgt_mouse_x = x; tgt_mouse_y = y; /* If case we won't find anything ... */
    
    if(child) /* Do we actually have children ? */
    {
	for(first_child = child->ln.prev ; ; child = child->ln.next)
		/* Do for every child .. */
	{
	    if(! (child->objflags & TGT_OBJFLAGS_INVISIBLE))
	    /* If that children is visible .... */
	    {
		if(x >= child->x && y >= child->y)
		    if(x < (child->x + child->xs) && y< (child->y + child->ys))
		    {
			/* Lookup inside that child */
			return(tgt_mouse_click(child,x - child->x,y - child->y));
		    }
	    }
	    /* We reached the last children */
	    if(child == first_child) break;
	}
	return(obj); /* There are no children linked at that position - 
	                - so we are the object that was clicked actually .. */
    }
    else
	return(obj); /* We do not have any children ! */
}

/*
 * tgt_reset_mouse : Reset mouse routines
 *
 */

void tgt_reset_mouse(void) /* Called whenever object tree structure changes */
{
    tgt_mouse_recv = NULL; /* We need that - what if eg. someone deletes
                              an object during a mouse drag ? */
    tgt_mouse_lock = NULL;
}

/*
 * tgt_lock_mouse : Disallow delivering any mouse event to object other than 
 *                  the supplied one.
 *
 */

void tgt_lock_mouse(struct tgt_object *obj)
{
    tgt_mouse_lock=obj;
    if(tgt_mouse_recv != obj) tgt_mouse_recv=NULL;
}

/*
 * tgt_unlock_mouse : Reallow delivering mouse events to all objects
 *
 */

void tgt_unlock_mouse(void)
{
    tgt_mouse_lock=NULL;
}

/* tgt_mouse_get_x
 * tgt_mouse_get_y : Return mouse position during last downclick, relative
 *                   to the object that was clicked
 */

int tgt_mouse_get_x(void)
{
    return(tgt_mouse_x);
}
int tgt_mouse_get_y(void)
{
    return(tgt_mouse_y);
}

int tgt_mouse_get_button(void)
{
    return(tgt_mouse_buttons);
}


/*
 * tgt_handle_mouse : Receive a mouse event from gpm
 *
 */

void tgt_handle_mouse(struct tgt_object * obj)
{
    Gpm_Event evt;
    
    if(Gpm_GetEvent(&evt) > 0)
    {
	tgt_mouse_buttons = ((evt.buttons & GPM_B_LEFT) ? TGT_MOUSE_LMB : 0) |
			    ((evt.buttons & GPM_B_RIGHT)? TGT_MOUSE_RMB : 0);

	if(evt.type & GPM_DOWN) /* A downclick ! */
	{
	    /* Find the base object - we could only have hope that it's
	       the desktop ... */
	    for( ; obj->ln.parent ; obj = obj->ln.parent);
	    /* Find the object that was actually clicked */
	    obj = tgt_mouse_click(obj,evt.x-1,evt.y-1);
	    
	    if(tgt_mouse_lock) if(tgt_mouse_lock != obj) return;
	    /* The mouse was locked by one of the objects and
	       the object that was clicked is not that one */

	    tgt_mouse_recv=obj;
	    /* This object will receive further mouse events,
	       such as _UP or _DRAG */
	    
	    if(! (obj->objflags & TGT_OBJFLAGS_NONSELECTABLE)) 
	        tgt_setfocus(tgt_mouse_recv);
	    /* Make the clicked object the active one */

	    if(tgt_mouse_recv->classf) 
		tgt_mouse_recv->classf(tgt_mouse_recv,TGT_OBJECT_HANDLE,TGT_KEY_MOUSEDOWN,NULL);
	    /* Notify the object about that click */
	}
	if(evt.type & GPM_UP)
	{
	    if(tgt_mouse_recv)
	    {
		if(tgt_mouse_recv->classf)
		    tgt_mouse_recv->classf(tgt_mouse_recv,TGT_OBJECT_HANDLE,TGT_KEY_MOUSEUP,NULL);
		/* Notify the object that is currently receiving mouse events about
		    that click */
		tgt_mouse_recv=NULL;
	    }
	}
	if(evt.type & GPM_DRAG)
	    if(tgt_mouse_recv) 
		if(tgt_mouse_recv->classf) 
		    tgt_mouse_recv->classf(tgt_mouse_recv,TGT_OBJECT_MOUSEDRAG,evt.dx,(void*) (tgtt) evt.dy);
		    /* Notify the object currently receiving mouse events */
    }
}

/*
 * tgt_shutdown_mouse - Disconnect from GPM
 *
 */

void tgt_shutdown_mouse(void)
{
    Gpm_Close();
}

#else
int tgt_mouse_get_x(void)
{
    return(0);
}
int tgt_mouse_get_y(void)
{
    return(0);
}
int tgt_mouse_get_button(void)
{
    return(0);
}
void tgt_lock_mouse(struct tgt_object *obj)
{
}
void tgt_unlock_mouse(void)
{
}
#endif