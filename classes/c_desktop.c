#include <stdio.h>
#include "tgt.h"
int switches_next[]={'`',TGT_KEY_DOWN,TGT_KEY_RIGHT,0};
int switches_prev[]={TGT_KEY_NONE,TGT_KEY_UP,TGT_KEY_LEFT,0};

int tgt_builtin_desktop(struct tgt_object *obj,int type,int a,void* b)
{
    long c;
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    tgt_initconswitch();
	    obj->objectf=(int (*)()) tgt_gettag(b,TGTT_CALLBACK,0);
	    if(!(obj->next_keys)) obj->next_keys=switches_next;
	    if(!(obj->prev_keys)) obj->prev_keys=switches_prev;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    tgt_chattr(obj->term,TGT_TA_NORMAL,0,0);
	    tgt_chattr(obj->term,TGT_TA_CLEAR,0,0);
	    fflush(stdout);
	    tgt_rawcon(0);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    c=obj->bg;
	    tgt_chattr(obj->term,TGT_TA_BGCOLOR,c,0);
	    tgt_chattr(obj->term,TGT_TA_CLEAR,0,0);
	    return(1);
	case TGT_OBJECT_HANDLE:
	    if(obj->objectf) return(obj->objectf(obj,a));
	    return(0);
	default: return(0);
	
    }
}
