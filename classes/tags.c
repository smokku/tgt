/* ----------------------------------------------------------------------------
**      tags.c  Taglist lookup routines for usage by class and management 
**              functions ; SETTAG/GETTAG methods frontends
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
#include <stdio.h>
#include <stdarg.h>

/*
 *   tgt_set : A frontend for setting a tag in an object
 */

int tgt_set(struct tgt_object * obj,
			   tgtt tag,   /* Tag key */
		           tgtt value) /* Tag value */
{
    switch(tag)   /* Handle most common tags */
    {
	case TGTT_X: obj->x = (int) value; break;
	case TGTT_Y: obj->y = (int) value; break;
	case TGTT_XS: 
	    obj->xs = (int) value; 
	    tgt_realloc_buffer(obj); /* The buffer size has changed */
	    return(obj->classf(obj,TGT_OBJECT_VISUALINIT,tag,(void*) value));
	case TGTT_YS: 
	    obj->ys = (int) value; 
	    tgt_realloc_buffer(obj); /* The buffer size has changed */
	    return(obj->classf(obj,TGT_OBJECT_VISUALINIT,tag,(void*) value));
	    break;
	case TGTT_ID: obj->id= (int) value; break;
	case TGTT_FG: obj->fg= (int) value; break;
	case TGTT_BG: obj->bg= (int) value; break;
	case TGTT_CALLBACK:
	    obj->objectf = (int(*)()) value;
	default: return(obj->classf(obj,TGT_OBJECT_SETTAG,tag,(void*) value));
    }
    return(1);
}

/*
 *   tgt_set : A frontend for getting a tag from an object
 */

int tgt_get(struct tgt_object * obj,tgtt tag,tgtt *value)
{
    switch(tag) /* Handle most common tags */
    {
	case TGTT_X: *value=(tgtt) obj->x; break;
	case TGTT_Y: *value=(tgtt) obj->y; break;
	case TGTT_XS: *value=(tgtt) obj->xs; break;
	case TGTT_YS: *value=(tgtt) obj->ys; break;
	case TGTT_ID: *value=(tgtt) obj->id; break;
	default: return(obj->classf(obj,TGT_OBJECT_GETTAG,tag,value));
    }
    return(1);
}

/*
 * tgt_gettag : lookup the value of a tag of given key inside taglist 
 *
 */

tgtt tgt_gettag(tgtt *taglist,
		tgtt stag,		/* The tag key we are looking for */
		tgtt defaultvalue)      /* What to return when we dont find it?*/
{
    tgtt tag;
    tgtt value;
	
    if(! taglist) return(defaultvalue);
    for(;;)
    {
        tag   = taglist[0]; 
	value = taglist[1]; 
	taglist += 2;
        if(tag == TGTT_END) return(defaultvalue); /* The end of taglist */
        if(stag == tag) return(value); /* That's the tag we are looking for ! */
    }
}

/*
 * tgt_getsizetag : A frontend for tgt_gettag designed for getting tags specyfing
 *                  sizes - when integer specified as a tag value is negative -
 *                  (0 - that_integer) % of terminal size will be taken.
 */

tgtt tgt_getsizetag(tgtt * taglist,tgtt stag,tgtt defaultvalue,struct tgt_terminal * term)
{
    tgtt v = tgt_gettag(taglist,stag,defaultvalue);
    if(v < 0)
    {
	if(stag == TGTT_XS) v=((0 - v) * term->x_size) / 100;
	if(stag == TGTT_YS) v=((0 - v) * term->y_size) / 100;
    }
    return(v);
}

