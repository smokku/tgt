#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tgt.h"

struct tgt_int_label
{
    char *text;
    int type;
};

void tgt_builtin_label_reorganize(struct tgt_object * obj)
{
    struct tgt_int_label * idata=obj->class_data;
    tgt_cell attr=TGT_T_BUILDCELL(obj->fg,obj->bg,0,0,0);
    char buffer[20];
    
    if(idata->type & TGT_LABELF_ALRIGHT)
	snprintf(buffer,19,"%%%d.%ds",obj->xs,obj->xs);
    else
	snprintf(buffer,19,"%%-%d.%ds",obj->xs,obj->xs);

    if(idata->text)
	tgt_flprintf(obj->visual_buffer,obj->xs,attr,buffer,idata->text);
}

int tgt_builtin_label(struct tgt_object *obj,int type,int a,void *b)
{
    struct tgt_int_label *idata;
    char *text;
    int l,i;
    
    switch(type)
    {
	case TGT_OBJECT_GETSIZES:
	    if(text=tgt_getptrtag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_LABEL_TEXT,NULL))
		l=strlen(text);
	    else
		l=1;
            ((struct tgt_ac_objectinfo*) b)->xsize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_XS,l,((struct tgt_ac_objectinfo*) b)->term);
	    ((struct tgt_ac_objectinfo*) b)->ysize=1;
	    ((struct tgt_ac_objectinfo*) b)->sizeflags=TGT_AC_SF_YFIXED|TGT_AC_SF_XFIXED;
	    return(1);				    	
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
	case TGT_OBJECT_VISUALINIT:
	    tgt_builtin_label_reorganize(obj);
	    return(1);
	case TGT_OBJECT_SETTAG:
	    idata=obj->class_data;
	    if(a==TGTT_LABEL_TEXT) {  idata->text=b; if(obj->xs<strlen(b)) { obj->xs=strlen(b); tgt_realloc_buffer(obj); } tgt_builtin_label_reorganize(obj);return(1); }
	    if(a==TGTT_LABEL_TYPE) { idata->type=(int) b; return(1); }
	    return(0);
	case TGT_OBJECT_GETTAG:
	    idata=obj->class_data;
	    if(a==TGTT_LABEL_TEXT) { *(char **) b=idata->text; return(1); }
	    return(0);
	default: return(0);
    }
}
