#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tgt.h"

int tgt_builtin_direct(struct tgt_object *obj,int type,int a,void *b)
{
    switch(type)
    {
	case TGT_OBJECT_GETSIZES:
            ((struct tgt_ac_objectinfo*) b)->xsize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_XS,1);
	    ((struct tgt_ac_objectinfo*) b)->ysize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_YS,1);
	    return(1);			    
	case TGT_OBJECT_CREATE:
	    obj->objflags|=TGT_OBJFLAGS_NONSELECTABLE;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    return(1);
	case TGT_OBJECT_VISUALINIT:
	    tgt_fill_buffer(obj->visual_buffer,obj->xs,obj->ys,TGT_T_BUILDCELL(obj->fg,obj->bg,0,0,' '));
	    return(1);
	case TGT_OBJECT_GETTAG:
	    if(a==TGTT_DIRECTSURFACE_SCREENMEM) { *((tgtt *) b)=(tgtt) obj->visual_buffer; return(1); }
	    return(0);
	default: return(0);
    }
}
