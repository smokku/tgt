/* ----------------------------------------------------------------------------
**      management.c  object management functions
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


#include <stdio.h>
#include <stdlib.h>
#include "tgt.h"

#define MAXIMAL_CLASS_COUNT 100

#ifdef TGT_DLCLASSES
#include <dlfcn.h>
#endif

    static int int_initialized=0;  /* Have we already initialized our internal classes ? */
    void * internal_classes[MAXIMAL_CLASS_COUNT]; 
		/* Class functions' table */
    extern void * g_prefs;  /* TGT configuration file handle
                              (obtained from functions from app/config.c */

    extern int g_error_handling;

void tgt_init_intclasses(void);

/*
 * tgt_start - A front-end for tgt initalization functions
 *             Initialize library, fetch terminal description and create
 *             a desktop object (a root object)
*/

struct tgt_object * tgt_start(char *tname, /* Terminal name (or NULL) */
			      struct tgt_terminal **ptr) /* Variable to store
			             a pointer to the terminal in */
{
    struct tgt_object * desktop;
    struct tgt_terminal * terminal;
    
    tgt_init();            /* Initialize configuration, read rcfiles */
    tgt_init_intclasses(); /* Initialize class table */
    if( ! (terminal = tgt_setscreen(NULL) ) )  /* Fetch terminal description */
    {
	if(g_error_handling) 
	    tgt_fatal_error(NULL,"tgt_start(): Unable to initalize terminal");
	return(NULL);
    }
    if( ! (desktop = tgt_getdesktop(terminal)) )
    {
	tgt_destroyterminal(terminal);
	if(g_error_handling) 
	    tgt_fatal_error(NULL,"tgt_start(): Unable to initalize root object");
	return(NULL);
    }
    if(ptr) *ptr=terminal;
    return(desktop);
}

/*
 * tgt_addclass - Add a class handling function to internal classes table
 *
 */

void tgt_addclass(int id, /* Class identifier */
		  int (*classf)(struct tgt_object *,int,int,void*)) /*Function*/
{
    if(g_error_handling)
	if(id < 0 || id > MAXIMAL_CLASS_COUNT) /* Check the class-id */
	    tgt_fatal_error(NULL,"tgt_addclass(): Class ID outside range");

    if(g_prefs)
	classf(NULL,TGT_OBJECT_SETDEFAULTS,0,g_prefs); /* Initialize the class */
    internal_classes[id]=(void*) classf; /* ... and add it to our table */
}
#ifdef TGT_DLCLASSES

/*
 * tgt_loadclass - Load a class from a shared library file and add it
 *                 to class table
 */

void * tgt_loadclass(int id,char *name)
{
    void * dlh;
    int (*classf)(struct tgt_object *,int,int,void*);
    
    if( (dlh=dlopen(name,RTLD_LAZY)) ) /* Open so file */
    {
	classf = dlsym(dlh,"classmain"); /* Look the class routine symbol up */
	if(classf)
	{
	    tgt_addclass(id,classf); /* Add class to our table */
	    return(dlh);
	}
	else /* We didn't find 'classmain' symbol */
	{
	    dlclose(dlh);
	    if(g_error_handling)
		tgt_fatal_error(NULL,"tgt_loadclass(): classmain not found in requested file");
	    return(NULL);
	}
    }
    else /* We were unable to open the file */
	if(g_error_handling)
	    tgt_fatal_error(NULL,"tgt_loadclass(): Unable to open requested dynamic class file");
	else
	    return(NULL);
}

/*
 *  tgt_unloadclass - free all memory allocated during loading a class from
 *                    shared library file
 */

void tgt_unloadclass(void *dlh) /* Value returned from tgt_loadclass() */
{
    dlclose(dlh);
}
#endif

/* 
 * tgt_init_intclasses - initialize built-in class functions
 *
 */
 
void tgt_init_intclasses(void)
{

    tgt_addclass(TGT_CLASS_DESKTOP,tgt_builtin_desktop);
    tgt_addclass(TGT_CLASS_WINDOW,tgt_builtin_window);
    tgt_addclass(TGT_CLASS_BUTTON,tgt_builtin_button);
    tgt_addclass(TGT_CLASS_LABEL,tgt_builtin_label);
    tgt_addclass(TGT_CLASS_STRING,tgt_builtin_string);
    tgt_addclass(TGT_CLASS_LIST,tgt_builtin_list);
    tgt_addclass(TGT_CLASS_MENU,tgt_builtin_menu);
    tgt_addclass(TGT_CLASS_SELECTBOX,tgt_builtin_selectbox);
    tgt_addclass(TGT_CLASS_CHECKBOX,tgt_builtin_checkbox);
    tgt_addclass(TGT_CLASS_SLIDER,tgt_builtin_slider);
    tgt_addclass(TGT_CLASS_PROGRESS,tgt_builtin_progress);
    tgt_addclass(TGT_CLASS_STATUS,tgt_builtin_status);
    tgt_addclass(TGT_CLASS_TEXTBUFFER,tgt_builtin_buffer);
    tgt_addclass(TGT_CLASS_DIRECTSURFACE,tgt_builtin_direct);
    tgt_addclass(TGT_CLASS_TERMEMU,tgt_builtin_termemu);
    int_initialized=1;
}

/*
 * tgt_link - assign an object a parent / add an object to another
 *            object's children list
 *
 */

void tgt_link(struct tgt_object *obj,struct tgt_object *parent)
{
    /* Set our new parent */
    obj->ln.parent = parent;
    if(parent->ln.child == NULL)
    { /* We'll be the only child of our parent */
	obj->ln.next = obj;
	obj->ln.prev = obj;
	parent->ln.child = obj;
    }
    else
    {
	/* Insert ourselves in the middle of the list */
	obj->ln.prev = parent->ln.child->ln.prev;
	obj->ln.next = parent->ln.child;
	parent->ln.child->ln.prev->ln.next = obj;
	parent->ln.child->ln.prev = obj;
	/* If the first object is a nonselectable one, take its place */
	if(parent->ln.child->objflags & TGT_OBJFLAGS_NONSELECTABLE)
	    parent->ln.child=obj;

    }
    /* Notify the object about its new parent */
    if(obj->classf) obj->classf(obj,TGT_OBJECT_LINK,0,NULL);
#ifdef TGT_GPMMOUSE
    /* We changed the object tree ... */
    tgt_reset_mouse();
#endif
}

/*
 * tgt_unlink - remove an object from its parent children list
 *
 */

void tgt_unlink(struct tgt_object *obj)
{

    if(obj->ln.parent)
    {
	if(obj->ln.parent->ln.child == obj) tgt_activatenext(obj);
	/* If we are the active one - switch the focus to another object */

	if(obj->ln.parent->ln.child == obj) obj->ln.parent->ln.child=obj->ln.next;
	/* Still we? Then switch to ANY other child */
	
	if(obj->ln.parent->ln.child == obj)
	/* AGAAAIAN? Goddamn, that's impossible */
	{
	    obj->ln.parent->ln.child=NULL;
	    /* Ohwell ... This object has no more children after all */
	}
	else
	{
	    obj->ln.next->ln.prev = obj->ln.prev;
	    obj->ln.prev->ln.next = obj->ln.next;
	    /* Just unlink the object */
	}
	obj->ln.parent = NULL; /* We do not have a parent now :( */
	if(obj->classf) obj->classf(obj,TGT_OBJECT_UNLINK,0,NULL);
	/* Notify the object .. */
    }
#ifdef TGT_GPMMOUSE
    /* We changed the object tree ... */
    tgt_reset_mouse();
#endif
}

/*
 *  tgt_createobject_int - Internal object creation routine
 *
 */


struct tgt_object * tgt_createobject_int(struct tgt_terminal *term, /* Terminal */
				 int (*classf)(struct tgt_object*,int,int,void*),
				 /* Class function */
				 tagitem *taglist,
				 int stx,int sty,int ptx,int pty)
				 /* Object sizes and position - if sizes are
				    non zero - it wont respect the object
				    taglist - This is designed for autolayouting
				    engines which have to override object
				    settings */
{
    struct tgt_object * ret;

    /* Allocate the object structure */
    ret=(struct tgt_object*) malloc(sizeof(struct tgt_object));

    ret->term = term;
    ret->classf = classf;
    /* Fill basic fields */
    
    /* Set sizes & fetch common tags */
    if(stx) 
    {
	ret->xs = stx;
	ret->x  = ptx;
    }
    else
    {
	ret->xs = tgt_getsizetag(taglist,TGTT_XS,1,term);
	ret->x  = tgt_getsizetag(taglist,TGTT_X,0,term);
    }
    	
    if(sty)
    {
	ret->ys = sty;
	ret->y  = pty;
    }
    else
    {
	ret->ys = tgt_getsizetag(taglist,TGTT_YS,1,term);
	ret->y  = tgt_getsizetag(taglist,TGTT_Y,0,term);
    }     

	
    ret->fg = tgt_getnumtag(taglist,TGTT_FG,term->color_fg);
    ret->bg = tgt_getnumtag(taglist,TGTT_BG,term->color_bg);
    ret->id = tgt_getnumtag(taglist,TGTT_ID,0); 
    ret->next_keys = (int*) tgt_getptrtag(taglist,TGTT_NEXT_KEYS,0);
    ret->prev_keys = (int*) tgt_getptrtag(taglist,TGTT_PREV_KEYS,0);

    ret->ln.parent = NULL; /* Reset object list node */
    ret->ln.child  = NULL;
    ret->ln.next   = NULL;
    ret->ln.prev   = NULL;
    
    ret->objflags  = 0;

    if(classf)
    {
	/* Initialize the object in YourWay (TM) */
	if(classf(ret,TGT_OBJECT_CREATE,0,taglist)==0) 
	{ 
	    free(ret);
	    if(g_error_handling) tgt_fatal_error(term,"tgt_createobject(): Custom object initializations failed");
	    return(NULL); 
	}
    }
    else  /* if we do not have a class function */
	if(g_error_handling)
	    tgt_fatal_error(term,"tgt_createobject_int(): No such class\n");
	else
	{
	    free(ret);
	    return(NULL);
	}

    if(ret->objflags & TGT_OBJFLAGS_REFRESHBASE) /* Is this a desktop object? */
    {
	ret->xs = term->x_size;
	ret->ys = term->y_size;
	ret->visual_buffer = term->terminal_buffer;
	 /* Then - its buffer and sizes are same as its terminal parameters */
    }
    else    
	/* Otherwise, allocate a buffer containing the object image */
	ret->visual_buffer = (tgt_cell*) malloc(sizeof(tgt_cell) * ret->xs * ret->ys);	
	
    classf(ret,TGT_OBJECT_VISUALINIT,0,NULL);
    /* Let the object draw itself, if it is not going to change its image */
#ifdef TGT_GPMMOUSE
    tgt_reset_mouse();
#endif
    return(ret);
}

/*
 * tgt_getdesktop - allocate a root object for given terminal.
 *
 */

struct tgt_object * tgt_getdesktop(struct tgt_terminal * term)
{
    struct tgt_object * desk;

    desk = tgt_createobject_int(term,
			      tgt_builtin_desktop,
			      (tagitem[]) { TGTT_X,0, TGTT_Y,0, TGTT_END,0},
			      0,0,0,0);
    return(desk);
}

/*
 * tgt_destroy - Destroy an object and free all associated with it memory
 *               Calls itself recursively in order to remove object's children
 */

void tgt_destroyobject(struct tgt_object *obj)
{
    struct tgt_object * firstch = obj->ln.child;
    struct tgt_object * nextch;
    struct tgt_object * tmpch;
    
    if( (nextch=obj->ln.child)!=NULL ) /* If we have any children ... */
	do
	{
	    tmpch = nextch->ln.next; /* Go trough them ... */
	    nextch->objflags |= TGT_OBJFLAGS_RECURSIVEKILL;
				/* Tell the object that it's a mass-kill */
	    tgt_destroyobject(nextch); /* ... and destroy each one */
	    nextch = tmpch;
	}
	while(nextch!=firstch);

#ifdef TGT_GPMMOUSE
    tgt_reset_mouse();
    /* We changed the object tree ... */
#endif
    /* Ask the class function to free all memory it allocated during object
       creation */
    obj->classf(obj,TGT_OBJECT_DESTROY,0,0);
    free(obj); /* Free the object structure */
    return;
}

/*
 * tgt_int_getclass - Find a class function for given classid
 *
 */

int (*tgt_int_getclass(int classid))(struct tgt_object*,int,int,void*)
{
    return(internal_classes[classid]);
}

/*
 * tgt_createobject - Create an object using one of builtin classes
 *
 */


struct tgt_object * tgt_createobject(struct tgt_terminal *term,
				     int classid,
				     tagitem *taglist)
{
    if(int_initialized == 0) tgt_init_intclasses();

    return(tgt_createobject_int(term,tgt_int_getclass(classid),taglist,0,0,0,0));
}

/*
 * tgt_createandlink - Create an object and perform a tgt_link() on it.
 *
 */

struct tgt_object * tgt_createandlink(struct tgt_object *parent, 
				 int classid,
				 tagitem *taglist)
{
    struct tgt_object * ret;
    if(int_initialized == 0) tgt_init_intclasses();
    ret = tgt_createobject_int(parent->term,internal_classes[classid],taglist,0,0,0,0);
    if(! ret) return(NULL);
    tgt_link(ret,parent);
    return(ret);
}
