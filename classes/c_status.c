#include <stdio.h>
#include <stdlib.h>
#include "tgt.h"

struct tgt_int_status
{
    char *text;
    int type,yshift;
};


int tgt_builtin_status(struct tgt_object *obj,int type,int a,void *b)
{
    struct tgt_int_status *idata;
    tgt_cell attr;
    char *text;
    char buffer[20];
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    idata=(struct tgt_int_status*) malloc(sizeof(struct tgt_int_status));
	    text=(char*) tgt_getptrtag(b,TGTT_STATUS_TEXT,NULL);
	    if(text==NULL) text="";
	    idata->text=text;
	    idata->type=tgt_getnumtag(b,TGTT_STATUS_TYPE,0);
	    idata->yshift=obj->y;
	    obj->class_data=idata;
	    obj->objflags|=TGT_OBJFLAGS_NONSELECTABLE;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    idata=obj->class_data;
	    if(idata->type & TGT_STATUSF_ALIGNED)
		{obj->x=1; obj->y=obj->ln.parent->ys+idata->yshift-2; obj->xs=obj->ln.parent->xs-2; tgt_realloc_buffer(obj);}
	    else
		{obj->x=0; obj->y=obj->ln.parent->ys+idata->yshift-1; obj->xs=obj->ln.parent->xs; tgt_realloc_buffer(obj);}
	    attr=TGT_T_BUILDCELL(obj->fg,obj->bg,0,0,0);
	    if(idata->type & TGT_STATUSF_ALRIGHT)
		snprintf(buffer,19,"%%%d.%ds",obj->xs,obj->xs);
	    else
		snprintf(buffer,19,"%%-%d.%ds",obj->xs,obj->xs);
	    tgt_printf(obj->visual_buffer,obj->xs,attr,buffer,idata->text);
	    return(1);
	case TGT_OBJECT_SETTAG:
	    idata=obj->class_data;
	    switch(a)
	    {
		case TGTT_STATUS_TEXT: idata->text=b; return(1); 
		case TGTT_STATUS_TYPE: idata->type=(int) b; return(1);
	    }
	    return(0);
	case TGT_OBJECT_GETTAG:
	    idata=obj->class_data;
	    if(a==TGTT_STATUS_TEXT) { *(char **) b=idata->text; return(1); }
	    return(0);
	default: return(0);
    }
}
