#include "tgt.h"
#include <stdio.h>

int tgt_set(struct tgt_object * obj,long tag,long value)
{
    switch(tag)
    {
	case TGTT_X: obj->x=(int) value; break;
	case TGTT_Y: obj->y=(int) value;  break;
	case TGTT_XS: obj->xs=(int) value; break;
	case TGTT_YS: obj->ys=(int) value; break;
	case TGTT_ID: obj->id=(int) value; break;
	default: return(obj->classf(obj,TGT_OBJECT_SETTAG,tag,(void*) value));
    }
    return(1);
}

int tgt_get(struct tgt_object * obj,long tag,int *value)
{
    switch(tag)
    {
	case TGTT_X: *value=(long) obj->x; break;
	case TGTT_Y: *value=(long) obj->y; break;
	case TGTT_ID: *value=(long) obj->id; break;
	default: return(obj->classf(obj,TGT_OBJECT_GETTAG,tag,value));
    }
    return(1);
}
