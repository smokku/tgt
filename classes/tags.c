#include "tgt.h"
#include <stdio.h>

int tgt_set(struct tgt_object * obj,long tag,long value)
{
    switch(tag)
    {
	case TTGT_X: obj->x=(int) value; if(obj->ln.parent) tgt_refresh(obj->ln.parent); else tgt_refresh(obj); break;
	case TTGT_Y: obj->y=(int) value; if(obj->ln.parent) tgt_refresh(obj->ln.parent); else tgt_refresh(obj); break;
	case TTGT_ID: obj->id=(int) value; break;
	default: return(obj->classf(obj,TGT_OBJECT_SETTAG,tag,(void*) value));
    }
    return(1);
}

int tgt_get(struct tgt_object * obj,long tag,int *value)
{
    switch(tag)
    {
	case TTGT_X: *value=(long) obj->x; break;
	case TTGT_Y: *value=(long) obj->y; break;
	case TTGT_ID: *value=(long) obj->id; break;
	default: return(obj->classf(obj,TGT_OBJECT_GETTAG,tag,value));
    }
    return(1);
}
