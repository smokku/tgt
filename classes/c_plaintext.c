#include <stdio.h>
#include "tgt.h"

int tgt_builtin_label(struct tgt_object *obj,int type,int a,void *b)
{
    int i,act;
    char *title;
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    title=(char*) tgt_gettag(b,TTGT_LABEL_TEXT,0);
	    if(title==NULL) return(0);
	    obj->class_data=title;
	    obj->objflags|=TGT_OBJFLAGS_NONSELECTABLE;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    return(1);
	case TGT_OBJECT_REFRESH:
	    tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a,obj->y+(int) b);
	    tgt_chattr(obj->term,TGT_TA_BGCOLOR,obj->bg,0);
	    tgt_chattr(obj->term,TGT_TA_FGCOLOR,obj->fg,0);
	    printf("%s",obj->class_data);
	    return(1);
	default: return(0);
    }
}
