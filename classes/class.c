/* ----------------------------------------------------------------------------
**	class.c  object management (for use by class functions)
** ----------------------------------------------------------------------------
**	TGT	                                  TextGUIToolkit Library
**	Copyright (C)	2000-2001	Mateusz Golicz and Tomasz Sterna
**
**	LGPL license
**
** ----------------------------------------------------------------------------
**	MAINTAINER	Mateusz Golicz
**
**	Log:
**	- cleanup
**	
*/


#include <stdio.h>
#include <unistd.h>
#include "tgt.h"

/*
 * tgt_rebuild_parent: Notify object's parent about a change in one of its
 * children buffers ; ie. rebuild its buffer by pasting its children buffers
 * in appropiate places
 */

void tgt_rebuild_parent(struct tgt_object * obj)
{
    struct tgt_object * parent = obj->ln.parent; // Our hero
    struct tgt_object *child , *cc;
    
    if(parent)
    {
	/* The first child */
	child = parent->ln.child;
	/* Do for each child, starting from the last one */
	for(cc = child->ln.prev ; cc!=child ; cc = cc->ln.prev)
	    if( ! (cc->objflags & TGT_OBJFLAGS_INVISIBLE) ) 
	    {
		if( (cc->x + cc->xs) > parent->xs || 
		    (cc->y + cc->ys) > parent->ys )
			tgt_fatal_error(parent->term,"tgt_rebuild_parent(): Object outside its parent area, terminal too small?");
		tgt_blit(parent->visual_buffer,parent->xs,
			 cc->visual_buffer,cc->xs,cc->ys,cc->x,cc->y);
	    }

	if( ! (cc->objflags & TGT_OBJFLAGS_INVISIBLE) ) 
	{
	    if( (cc->x + cc->xs) > parent->xs || 
	    (cc->y + cc->ys) > parent->ys )
	    	tgt_fatal_error(parent->term,"tgt_rebuild_parent(): Object outside its parent area, terminal too small?");
	    tgt_blit(parent->visual_buffer,parent->xs,
		 cc->visual_buffer,cc->xs,cc->ys,cc->x,cc->y);
	}

	/* Recursively notify the parent of our parent about the change -
	 *    - after all we modified one of its children buffer
	 */
	tgt_rebuild_parent(parent);    
    }
    else
	tgt_update_screen(obj->term);
    /* This object does not have a parent - which means it's the top object
     * in TGT object hierachy - it's virtual parent is the terminal object -
     * - so we need to refresh terminal
     */
}

/* 
 * tgt_haschild: Find an object with given objid in children of given object
 */

struct tgt_object * tgt_haschild(struct tgt_object *obj,int id)
{
    struct tgt_object * children;
    struct tgt_object * cc = obj->ln.child; /* First child */

    if(!cc) return(NULL); /* If the object does not have any children - quit */
    for(children = cc->ln.next ; children!=cc ; children = children->ln.next)
    /* For each children ... */
	if(children->id == id) return(children);
	/* If that child has objid equal to the given one - return a pointer to it*/

    return(NULL); /* We didn't find what we were looking for */
}

/* 
 * tgt_findchild: Recursively look for an object with specified objid in object
 * tree
 */

struct tgt_object * tgt_findchild(struct tgt_object *obj,int id)
{
    struct tgt_object * children;
    struct tgt_object * cc = obj->ln.child; /* First child */
    struct tgt_object * match;

    if(!cc) return(NULL); /* If the object does not have any children - quit */
    for(children = cc->ln.next ; children!=cc ; children = children->ln.next)
    {
	/* For each children ... */
	if(children->id == id) return(children);
	/* If that child has objid equal to the given one - return a pointer to it*/
	if(match = tgt_findchild(children,id)) return(match);
	/* Look for object inside that child children list */
    }

    return(NULL); /* We didn't find what we were looking for */
}

/*
 * tgt_shalliswitch: helper function for keyboard handling - look for a key
 * in our parent's switching keys tables.
 */

int tgt_shalliswitch(struct tgt_object *obj,
		     int key, /* Keycode */
		     int pri) /* Priority, if not-null this function will
		                 return non-zero only in 'ermergency' cases */
{
    struct tgt_object * parent;
    int * nk , * pk , i;

    if( ! (parent = obj->ln.parent) ) return(0); /* Actually, we do not have a parent */
    if(pri) /* The object wants to collect as many keys as it's possible - 
               compare the keys only with first items in switching keys
	       tables */
    {
	if( (nk = parent->next_keys) ) /* If the parent actually *has* a
					    switchingkeys table ... */
	    if(nk[0] == key) return(1); /* Check for entry in table containing
					keys choosing the next child */
	if( (pk = parent->next_keys) )
	    if(pk[0] == key) return(-1); /* ... And same for keys choosing
	                                   the previous child  */
    }
    else
    {
	if( (nk = parent->next_keys) )
	    for( i=0 ; nk[i]!=0 ; i++) if(nk[i] == key) return(1);
	if( (pk = parent->prev_keys) )
	    for( i=0 ; pk[i]!=0 ; i++) if(pk[i] == key) return(-1);
	/* Like above, but search whole tables */
    }
    return(0); /* We didn't found that key in our entries */
}

/*
 * tgt_intrefresh: Internal refreshing function
 */

int tgt_intrefresh(struct tgt_object *obj, /* An object and ... */
		   int a, /* Its parent onscreen X coordinate */
		   int b, /* Its parent onscreen Y coordinate */
		   int rs)/* Refresh also the object children recursively ? */
{
    struct tgt_object * firstch;
    struct tgt_object * prevch;


    /* Call the class handler - it does the all dirty work */
    
    obj->classf(obj,TGT_OBJECT_REFRESH,a,(void*) b);

    if(obj->ln.child) /* Do we have any children ? */
    {    
	firstch = obj -> ln.child -> ln.prev; /* Last child, but it's the
	                             first one we want to refresh */
	a += obj->x; 
	b += obj->y; /* Update coordinates */
	if( (prevch = obj->ln.child->ln.prev) ) /* Last child again .. */
    	    do
	    {
		if( ! (prevch->objflags & TGT_OBJFLAGS_INVISIBLE) )
		{
		    /* Recursively refresh children if requested */
		    if(rs) tgt_intrefresh(prevch, a, b, 1);
		    
		    /* Blit child's buffer inside ours */
		    tgt_blit(obj->visual_buffer,obj->xs,
			     prevch->visual_buffer,prevch->xs,prevch->ys,
			     prevch->x,prevch->y);
		}
		prevch = prevch->ln.prev; /* Next object to refresh ... */
	    }
	    while(prevch != firstch); /* Done when we meet the first object
				       we were refreshing */
    }
    return(0);
}
#ifdef SEMAPHORED_REFRESH
    static int ref_semaphore=0;
#endif


#if 0
int tgt_voilatile_refresh(struct tgt_object *obj)
{
#ifdef SEMAPHORED_REFRESH
    ref_semaphore=0;
#endif
    tgt_refresh(obj);
    return(1);
}
#endif

/*
 * tgt_do_refresh: Refresh an object
 */

int tgt_do_refresh(struct tgt_object *obj,
		   int r) /* Refresh children recursively ? */
{
    int x , y;
    struct tgt_object * parent;
    struct tgt_object * ref;
    if(obj->objflags & TGT_OBJFLAGS_INVISIBLE) return(0); 
		/* If the object is invisible - return immediately */
#ifdef SEMAPHORED_REFRESH
    while(ref_semaphore) usleep(SEMAPHORED_REFRESH_DELAY); 
		/* Wait for semaphore to be released */
    ref_semaphore = 1; /* Obtain the semaphore */
#endif
    /* Find absolute coordinates of our parent */
    x=0; y=0; ref=obj;
    for( parent = obj->ln.parent ; parent!=NULL ; parent = parent->ln.parent)
	/* Go up the object tree */
    {
	x+=parent->x;
	y+=parent->y;
	ref=parent;
    }
    
    if(ref->objflags & TGT_OBJFLAGS_REFRESHBASE) /* Was the last object during
                 our survey a desktop object ? */
    {
	tgt_intrefresh(obj,x,y,r); /* Call internal refresh function */
	tgt_rebuild_parent(obj);   /* Notify our parent that we are changed now */
    }
#ifdef SEMAPHORED_REFRESH
    ref_semaphore=0; /* Release semaphore */
#endif
    return(1);
}

/*
 * tgt_isactive: Check whether given object is the first one on its parent children
 *              list
 */

int tgt_isactive(struct tgt_object *obj)
{
    if( ! obj->ln.parent ) return(0);
    if(obj->ln.parent->ln.child == obj) return(1); else return(0);
}

/*
 * tgt_hasfocus: Recursively check whether object and all its predecessors are
 * active ones , relative to their parents
 */

int tgt_hasfocus(struct tgt_object *obj)
{
    struct tgt_object * parent;
    struct tgt_object * current;
    for(parent = obj->ln.parent, current = obj ; parent!=NULL ;)
    {
        if(parent->ln.child != current) return(0);
        current = parent;
        parent = parent->ln.parent; /* Go up the object tree */
    }
    return(1);
}
/*
 * tgt_unsetfocus: Make object the last object on its parent children list
 */

/*
 * tgt_setfocus: Make object, its parent, the parent of its parent, etc. active
 */

void tgt_setfocus(struct tgt_object *obj)
{
    struct tgt_object * parent;
    struct tgt_object * current;
    struct tgt_object * pred;
    struct tgt_object * this;

    for(parent = obj->ln.parent, current = obj ; parent!=NULL ;)
    {
	if(current != parent->ln.child)
	{
	    current->ln.prev->ln.next = current->ln.next;
	    current->ln.next->ln.prev = current->ln.prev;
	
	    pred=parent->ln.child->ln.prev;
	    this=parent->ln.child;
	
	    pred->ln.next=current;
	    current->ln.prev=pred;
	
	    this->ln.prev=current;
	    current->ln.next=this;
	}
	
	parent->ln.child = current;
	current = parent;
	parent = parent->ln.parent;
    }
    if(obj->ln.parent)
	tgt_refresh(obj->ln.parent);
    else
	tgt_refresh(obj);
}

/*
 * tgt_findnext_selectable: Find next object that can be marked active
 * on given object's parent children list
 */

struct tgt_object * tgt_findnext_selectable(struct tgt_object * obj)
{
    struct tgt_object * next;
    if(! obj) return(NULL); /* What're we talking about ? */
    next = obj;
    for(;;)
    {
	next = next->ln.next;
	if(next == obj) return(obj);
	if( ! (next->objflags & TGT_OBJFLAGS_NONSELECTABLE) ) return(next);
    }
}

/*
 * tgt_findprev_selectable: Find previous object that can be marked active
 * on given object's parent children list
 */

struct tgt_object * tgt_findprev_selectable(struct tgt_object * obj)
{
    struct tgt_object *next;
    if(! obj) return(NULL);
    next = obj;
    for(;;)
    {
	next = next->ln.prev;
	if(next == obj) return(obj);
	if( ! (next->objflags & TGT_OBJFLAGS_NONSELECTABLE) ) return(next);
    }
}

/*
 * tgt_activatenext: Find next/previous object that can be marked active on given 
 * tgt_activateprev: object's parent children list, and make that object the
 *                  active one.
 *
 */

void tgt_activatenext(struct tgt_object *obj)
{
    struct tgt_object * parent;
    if( ! (parent = obj->ln.parent) ) return;
    parent->ln.child = tgt_findnext_selectable(obj);
    tgt_refresh(obj);
    if(parent->ln.child != obj) tgt_refresh(parent->ln.child);
}
void tgt_activateprev(struct tgt_object *obj)
{
    struct tgt_object *parent;
    if( ! (parent = obj->ln.parent) ) return;
    parent->ln.child = tgt_findprev_selectable(obj);
    tgt_refresh(obj);
    if(parent->ln.child != obj) tgt_refresh(parent->ln.child);
}

/* 
 * tgt_deliver_msg: Deliver message to an object, specifically, find an object
 * that should be receiving messages and ask it whether it can undestand this
 * one. If not - ask its parent ... 
 */

int tgt_deliver_msg(struct tgt_object *obj,int type,int param,char* param2)
{
    struct tgt_object * active;

    for(active = obj ; active->ln.child != NULL ; active = active->ln.child);

    while(active->classf(active,type,param,param2) != 1)
    {
	if(active == obj) return(0);
	if( ! (active = active->ln.parent) ) return(0);
    }
    return(1);
}

