#include <stdio.h>
#include "tgt.h"

int tgt_builtin_desktop(struct tgt_object *obj,int type,int a,void* b)
{
    long c;
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    tgt_initconswitch();
	    return(1);
	case TGT_OBJECT_DESTROY:
	    tgt_chattr(obj->term,TGT_TA_NORMAL,0,0);
	    tgt_chattr(obj->term,TGT_TA_CLEAR,0,0);
	    fflush(stdout);
	    tgt_normalcon();
	    return(1);
	case TGT_OBJECT_REFRESH:
	    c=obj->bg;
	    tgt_chattr(obj->term,TGT_TA_BGCOLOR,c,0);
	    tgt_chattr(obj->term,TGT_TA_CLEAR,0,0);
	    printf(obj->term->c_clear);
	    return(1);
	case TGT_OBJECT_HANDLE:
	    if(a=='`') { tgt_activatenext_child(obj); return(1); }
	    return(0);
//	    if(a==-1) printf("back\n");
	default: return(0);
	
    }
}
