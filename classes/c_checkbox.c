#include <stdio.h>
#include "tgt.h"

struct tgt_int_checkbox
{
    int activebg;
    char *title;
    int active;
};

int tgt_builtin_checkbox(struct tgt_object *obj,int type,int a,void *b)
{
    int n;
    struct tgt_int_checkbox *idata;
    char *title;
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    title=(char*) tgt_getptrtag(b,TGTT_CHECKBOX_CAPTION,NULL);
	    idata=(struct tgt_int_checkbox*) malloc(sizeof(struct tgt_int_checkbox));
	    if(title==NULL) title="";
	    idata->title=(char *) title;
	    idata->active=tgt_getnumtag(b,TGTT_CHECKBOX_ACTIVE,0);
	    idata->activebg=tgt_getnumtag(b,TGTT_CHECKBOX_ACTIVEBG,6);
	    obj->objectf=(int (*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    obj->class_data=idata;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    idata=obj->class_data;
	    tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a,obj->y+(int) b);
	    if(tgt_hasfocus(obj))
	    {	
		tgt_chattr(obj->term,TGT_TA_BOLD,0,0);
		tgt_chattr(obj->term,TGT_TA_BGCOLOR,idata->activebg);
	    }
	    else
		tgt_chattr(obj->term,TGT_TA_BGCOLOR,obj->bg);

	    tgt_chattr(obj->term,TGT_TA_FGCOLOR,obj->fg);
	    printf("[%s] %s", idata->active?"x":" ", idata->title);
	    tgt_chattr(obj->term,TGT_TA_NORMAL,0,0);
	    fflush(stdout);
	    return(1);
	case TGT_OBJECT_HANDLE:
	    idata=obj->class_data;
	    if(a==13 || a==10 || a==32) 
	    {
		idata->active = ! idata->active;
		tgt_refresh(obj);
		if(obj->objectf!=NULL) obj->objectf(obj);
		return(1);
	    }
	    n=tgt_shalliswitch(obj,a,0);
	    if(n<0) { tgt_activateprev(obj); return(1); }
	    if(n>0) { tgt_activatenext(obj); return(1); }
	    return(0);
	case TGT_OBJECT_GETTAG:
	    idata=obj->class_data;
	    switch(a)
	    {
		case TGTT_CHECKBOX_ACTIVE: *((int*) b)=idata->active; return(1); 
	    }
	    return(0);
	case TGT_OBJECT_SETTAG:
	    idata=obj->class_data;
	    switch(a)
	    {
		case TGTT_CHECKBOX_ACTIVE: idata->active=(int) b; return(1);
		case TGTT_CHECKBOX_ACTIVEBG: idata->activebg=(int) b; return(1); 
		case TGTT_CHECKBOX_CAPTION: idata->title=(char*) b; return(1);
	    }
	    return(0);
	default: return(0);
    }
}
