#include <stdio.h>
#include <string.h>
#include "tgt.h"

struct tgt_int_label
{
    char *text;
    int type;
};

int tgt_builtin_label(struct tgt_object *obj,int type,int a,void *b)
{
    struct tgt_int_label *idata;
    char *text;
    char buffer[20];
    int l;
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    idata=(struct tgt_int_label*) malloc(sizeof(struct tgt_int_label));
	    text=(char*) tgt_getptrtag(b,TGTT_LABEL_TEXT,NULL);
	    if(text==NULL) text="";
	    l=strlen(text);
	    if(obj->xs < l) obj->xs=l;
	    idata->text=text;
	    idata->type=tgt_getnumtag(b,TGTT_LABEL_TYPE,0);
	    obj->class_data=idata;
	    obj->objflags|=TGT_OBJFLAGS_NONSELECTABLE;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    idata=obj->class_data;
	    tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a,obj->y+(int) b);
	    tgt_chattr(obj->term,TGT_TA_BGCOLOR,obj->bg,0);
	    tgt_chattr(obj->term,TGT_TA_FGCOLOR,obj->fg,0);
	    if(idata->type & TGT_LABELF_ALRIGHT)
		sprintf(buffer,"%%%d.%ds",obj->xs,obj->xs);
	    else
		sprintf(buffer,"%%-%d.%ds",obj->xs,obj->xs);
	    printf(buffer,idata->text);
	    fflush(stdout);
	    return(1);
	case TGT_OBJECT_SETTAG:
	    idata=obj->class_data;
	    if(a==TGTT_LABEL_TEXT) { idata->text=b; return(1); }
	    if(a==TGTT_LABEL_TYPE) { idata->type=(int) b; return(1); }
	    return(0);
	case TGT_OBJECT_GETTAG:
	    idata=obj->class_data;
	    if(a==TGTT_LABEL_TEXT) { *(char **) b=idata->text; return(1); }
	    return(0);
	default: return(0);
    }
}
