#include <stdio.h>
#include <stdlib.h>
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
    int n,i,k;
    float m;
    struct tgt_int_progress *idata;
    char *title;
    char atb[20];
    tgt_cell attr;
    tgt_cell * buff;
    
    switch(type)
    {
	case TGT_OBJECT_GETSIZES:
            n=8;
            if( (i=snprintf(atb,19,"%d",tgt_getnumtag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_PROGRESS_ENDVALUE,0))) > n) n=i;
            n+=strlen(tgt_getptrtag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_PROGRESS_CAPTION,""));
            ((struct tgt_ac_objectinfo*) b)->xsize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_XS,n,((struct tgt_ac_objectinfo*) b)->term);
            ((struct tgt_ac_objectinfo*) b)->ysize=1;
            ((struct tgt_ac_objectinfo*) b)->sizeflags|=TGT_AC_SF_YFIXED;
            return(1);
	case TGT_OBJECT_CREATE:
	    title=(char*) tgt_getptrtag(b,TGTT_PROGRESS_CAPTION,NULL);
	    if(title==NULL) title="";
	    obj->objflags|=TGT_OBJFLAGS_NONSELECTABLE;
	    idata=(struct tgt_int_progress*) malloc(sizeof(struct tgt_int_progress));
	    idata->title=title;
	    idata->endvalue=tgt_getnumtag(b,TGTT_PROGRESS_ENDVALUE,255);
	    idata->value=tgt_getnumtag(b,TGTT_PROGRESS_VALUE,0);
	    if(idata->value > idata->endvalue) idata->value=idata->endvalue;
	    idata->type=tgt_getnumtag(b,TGTT_PROGRESS_TYPE,0);
	    idata->activebg=tgt_getnumtag(b,TGTT_PROGRESS_ACTIVEBG,6);
	    obj->class_data=idata;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    idata=obj->class_data;
	    attr=tgt_hasfocus(obj) ? 
		TGT_T_BUILDCELL(obj->fg,idata->activebg,1,0,0) :
		TGT_T_BUILDCELL(obj->fg,obj->bg,0,0,0);


            if(idata->type & TGT_PROGRESSF_SHOWVALUE)
                if(idata->type & TGT_PROGRESSF_PERCENT)
                    snprintf(atb,19,"]%3d%%", idata->value / idata->endvalue * 100);
                else
                    snprintf(atb,19,"] %d ", idata->value);
            else
                snprintf(atb,19,"]");

	    buff=obj->visual_buffer;
	    buff+=(n=tgt_printf(buff,obj->xs,attr,"%s[", idata->title));

	    k=obj->xs-n-strlen(atb);

	    m=k * idata->value / idata->endvalue;
	    
	    
	    for(i=0,n=(int)m;i<n;i++) *(buff++)=TGT_T_FCHAR(attr,'#');
	    for(i=0,n=k-(int)m;i<n;i++) *(buff++)=TGT_T_FCHAR(attr,'.');
	    tgt_printf(buff,obj->xs,attr,atb);
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
		case TGTT_PROGRESS_TYPE: idata->type=(int) b; return(1);
		case TGTT_PROGRESS_ACTIVEBG: idata->activebg=(int) b; return(1);
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
