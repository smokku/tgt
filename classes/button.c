/* ----------------------------------------------------------------------------
**      c_button.c  The Only Commented Class File (TM)
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
#include <string.h>
#include <stdlib.h>
#include "tgt.h"

/* own class data structure */
struct tgt_int_button
{
    int activebg;
    char *title;
    int state;
};

static int d_actbg=6; /* default background color when the button is active*/

/* rebuild the object buffer */
void tgt_builtin_button_rebuild(struct tgt_object *obj)
{
    tgt_cell attr, *buff=obj->visual_buffer;
    int xsl = obj->xs,i;
    unsigned char *title;
    struct tgt_int_button * iw = obj->class_data;
    
    attr = TGT_T_BUILDCELL(obj->fg,obj->bg,0,0,0); /* build MSB part of object cells */
    title = iw->title;
    if(title)
    {
	if(strlen(title)<xsl) xsl=strlen(title); /* the string length, if it's shorter than the object */
	i = obj->xs-xsl; /* how many spaces we have to print out to fill the whole object ? */
	while(xsl--) *(buff++)=TGT_T_FCHAR(attr,*(title++)); /* print the title */
	while(i--) *(buff++)=TGT_T_FCHAR(attr,' '); /* print padding spaces */
	iw->state = 1; /* nonselected */
    }
}


int tgt_builtin_button(struct tgt_object *obj,int type,int a,void *b)
{
    int n;
    struct tgt_int_button *iw;
    char *title;
    switch(type)
    {
	case TGT_OBJECT_GETSIZES:
	    /* GetSizes method
	       Args: b - pointer to tgt_ac_objectinfo 
	       Return the [minimal] object size for autocreate routines */

	    /* Do we have a title set ? */
	    if((title=(char*) tgt_getptrtag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_BUTTON_CAPTION,NULL)))
	    /* If so - our minimal width is our title length */
		n=strlen(title);
	    else
	    /* If not - ohwell ... */
		n=1;

	    /* Now, if user defined TGT_XS, respect it before title length */	
	    ((struct tgt_ac_objectinfo*) b)->xsize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_XS,n);
	    ((struct tgt_ac_objectinfo*) b)->ysize=1;
	    ((struct tgt_ac_objectinfo*) b)->sizeflags|=TGT_AC_SF_XFIXED|TGT_AC_SF_YFIXED;
	    /* To be improved                           ^^^^^^^^^^^^^^^^^*/
	    return(1);
	case TGT_OBJECT_CREATE:
	    /* Create method
	       Args: b - pointer to object's taglist 
	       Initialize object.
	    */
	    title=(char*) tgt_getptrtag(b,TGTT_BUTTON_CAPTION,NULL);
	    iw=(struct tgt_int_button*) malloc(sizeof(struct tgt_int_button));
	    /* allocate own data structure, get object tags etc. */
	    obj->class_data=iw;
	    
	    iw->title = title;
	    iw->activebg=tgt_getnumtag(b,TGTT_BUTTON_ACTIVEBG,d_actbg);
	    
	    if(title)
		if(tgt_getsizetag(b,TGTT_XS,0)==0) obj->xs=strlen(title);

	    obj->objectf=(int (*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    return(1);
	case TGT_OBJECT_DESTROY:
	    /* Destroy method
	       Free allocated by class memory, deinitialize object */
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_VISUALINIT:
	    /* VisualInit method
	       Fill object's visual buffer, if neccessary */
	    obj->ys = 1;
	    tgt_realloc_buffer(obj);
	    tgt_builtin_button_rebuild(obj);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    /* Refresh method
	       Uhm ... Refreshes object on screen */
	    iw=obj->class_data; /* get internal class data of this object */
	    if(tgt_hasfocus(obj)) /* are we active ? */
	    {
	        if(iw->state == 1) /* 1 means inactive */
		{
		    /* so we are inactive and we have to be active ? */
		    /* then replace background color to the activebg one */
		    tgt_change_colors(obj->visual_buffer,obj->xs,obj->fg,iw->activebg,1);
		    iw->state = 2;
		}
	    }
	    else
	    {
		if(iw->state == 2) /* 2 means active */
		{
		    /* so we are active and we have to be inactive ? */
		    /* then replace background color to the bg one */
		    tgt_change_colors(obj->visual_buffer,obj->xs,obj->fg,obj->bg,0);
		    iw->state = 1;
		}
	    }
	    return(1);
	case TGT_OBJECT_HANDLE:
	    /* Handle[Input] method
	       Args: a - keycode
	       Parse keyboard and mouse events */
	    if(a==13 || a==10 || a==32 || a==TGT_KEY_MOUSEUP) 
	    {
		if(obj->objectf!=NULL) obj->objectf(obj); /* call object's callback */
		return(1);
	    }
	    n=tgt_shalliswitch(obj,a,0); /* is that key an object-switching key ? */
	    if(n<0) { tgt_activateprev(obj); return(1); } /* if so, switch to the */
	    if(n>0) { tgt_activatenext(obj); return(1); } /* next/previous object */
	    return(0);
	case TGT_OBJECT_SETDEFAULTS:
	    /* SetDefaults method 
	       Args: b - pointer to preferences structure
	       Initialize class, if neccessary */
	    /* get default values */
	    d_actbg=atoi(tgt_getprefs(b,"button","activebg","6"));
	    return(1);
	case TGT_OBJECT_SETTAG:
	    /* SetTag method
	       Args: a - tag code
	             b - tag value
	       Change an object capability */
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_BUTTON_CAPTION:
		    iw->title=(char*) b; 
		    if(obj->xs==1) obj->xs=strlen(b);
		    tgt_realloc_buffer(obj);
		    tgt_builtin_button_rebuild(obj);
		    return(1);
		case TGTT_BUTTON_ACTIVEBG: iw->activebg=(int) b;tgt_builtin_button_rebuild(obj); return(1);
		case TGTT_FG:
		case TGTT_BG:
		    tgt_builtin_button_rebuild(obj);
		    return(1);
	    }
	    return(0);
	case TGT_OBJECT_GETTAG:
	    /* GetTag method
	       Args: a - tag code
	             b - tag value pointer
	       Save an object capability to specifed variable*/
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_BUTTON_CAPTION: *((tgtt*)b) = (tgtt) iw->title; return(1);
		case TGTT_BUTTON_ACTIVEBG: *((tgtt*)b) = (tgtt) iw->activebg; return(1);
	    }
	    return(0);
	default: return(0);
    }
}
