#include <stdio.h>
#include "tgt.h"

struct tgt_int_progress
{
    int activebg;
    char *title;
    int value, endvalue;
    int type;
};

int tgt_builtin_progress(struct tgt_object *obj,int type,int a,void *b)
{
    int n,i;
    float m;
    struct tgt_int_progress *idata;
    char *title;
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    title=(char*) tgt_gettag(b,TGTT_PROGRESS_CAPTION,0);
	    if(title==NULL) title="";
	    obj->objflags|=TGT_OBJFLAGS_NONSELECTABLE;
	    idata=(struct tgt_int_progress*) malloc(sizeof(struct tgt_int_progress));
	    idata->title=title;
	    idata->endvalue=tgt_gettag(b,TGTT_PROGRESS_ENDVALUE,255);
	    idata->value=tgt_gettag(b,TGTT_PROGRESS_VALUE,0);
	    if(idata->value > idata->endvalue) idata->value=idata->endvalue;
	    idata->type=tgt_gettag(b,TGTT_PROGRESS_TYPE,0);
	    idata->activebg=tgt_gettag(b,TGTT_PROGRESS_ACTIVEBG,6);
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

	    m=(obj->xs - 2) * idata->value / idata->endvalue;
	    tgt_chattr(obj->term,TGT_TA_FGCOLOR,obj->fg);
	    printf("%s[", idata->title);
	    for(i=0,n=(int)m;i<n;i++) putchar('#');
	    for(i=0,n=obj->xs-(int)m-2;i<n;i++) putchar('.');
            if(idata->type & TGT_PROGRESSF_SHOWVALUE)
                if(idata->type & TGT_PROGRESSF_PERCENT)
                    printf("]%3d%%", idata->value / idata->endvalue * 100);
                else
                    printf("] %d ", idata->value);
            else
                putchar(']');
	    tgt_chattr(obj->term,TGT_TA_NORMAL,0,0);
	    fflush(stdout);
	    return(1);
	case TGT_OBJECT_SETTAG:
	    idata=obj->class_data;
	    switch(a)
	    {
		case TGTT_PROGRESS_VALUE:
		    idata->value=(int) b;
		    if(idata->value > idata->endvalue) idata->value=idata->endvalue;
		    if(idata->value < 0) idata->value=0;
		    return(1);
		case TGTT_PROGRESS_ENDVALUE:
		    idata->endvalue=(int) b;
		    if(idata->endvalue < 1) idata->endvalue=1;
		    return(1);
	    }
	    return(0);
	case TGT_OBJECT_GETTAG:
	    idata=obj->class_data;
	    switch(a)
	    {
		case TGTT_PROGRESS_VALUE:
		    *((int*) b)=idata->value;
		    return(1);
		case TGTT_PROGRESS_ENDVALUE:
		    *((int*) b)=idata->endvalue;
		    return(1);
	    }
	    return(0);
	default: return(0);
    }
}
