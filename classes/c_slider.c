#include <stdio.h>
#include "tgt.h"

struct tgt_int_slider
{
    int activebg;
    char *title;
    int value, minvalue, maxvalue;
    int type;
};

int tgt_builtin_slider(struct tgt_object *obj,int type,int a,void *b)
{
    int n,i;
    struct tgt_int_slider *idata;
    float m;
    char *title;
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    title=(char*) tgt_gettag(b,TGTT_SLIDER_CAPTION,0);
	    if(title==NULL) title="";
	    idata=(struct tgt_int_slider*) malloc(sizeof(struct tgt_int_slider));
	    idata->title=title;
	    idata->minvalue=tgt_gettag(b,TGTT_SLIDER_MINVALUE,0);
	    idata->maxvalue=tgt_gettag(b,TGTT_SLIDER_MAXVALUE,100);
	    idata->value=tgt_gettag(b,TGTT_SLIDER_VALUE,0);
	    if(idata->value > idata->maxvalue) idata->value=idata->maxvalue;
	    if(idata->value < idata->minvalue) idata->value=idata->minvalue;
	    idata->type=tgt_gettag(b,TGTT_SLIDER_TYPE,0);
	    idata->activebg=tgt_gettag(b,TGTT_SLIDER_ACTIVEBG,6);
	    obj->objectf=(int (*)()) tgt_gettag(b,TGTT_CALLBACK,0);
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

	    m=(obj->xs - 3) * (idata->value - idata->minvalue) / (idata->maxvalue - idata->minvalue);
	    tgt_chattr(obj->term,TGT_TA_FGCOLOR,obj->fg);
	    printf("%s[", idata->title);
	    for(i=0,n=(int)m-1;i<n;i++) putchar('-');
	    putchar('V');
	    for(i=0,n=obj->xs-(int)m-1;i<n;i++) putchar('-');
	    if(idata->type & TGT_SLIDERT_SHOWVALUE)
		if(idata->type & TGT_SLIDERT_PERCENT)
		    printf("] %d%%", idata->value / (idata->maxvalue - idata->minvalue) * 100);
		else
		    printf("] %d", idata->value);
	    else
		putchar(']');
	    tgt_chattr(obj->term,TGT_TA_NORMAL,0,0);
	    fflush(stdout);
	    return(1);
	case TGT_OBJECT_HANDLE:
	    idata=obj->class_data;
	    switch(a)
	    {
		case TGT_KEY_RIGHT:
		    idata->value++;
		    if(idata->value > idata->maxvalue) idata->value=idata->maxvalue;
		    tgt_refresh(obj);
		    return(1);
		case TGT_KEY_LEFT:
		    idata->value--;
		    if(idata->value < idata->minvalue) idata->value=idata->minvalue;
		    tgt_refresh(obj);
		    return(1);
		case TGT_KEY_HOME:
		    idata->value=idata->minvalue;
		    tgt_refresh(obj);
		    return(1);
		case TGT_KEY_END: 
		    idata->value=idata->maxvalue;
		    tgt_refresh(obj);
		    return(1);
		case 13: case 10:
		    if(obj->objectf) obj->objectf(obj, idata->value);
		    return(1);
		default:
		    n=tgt_shalliswitch(obj,a,0);
		    if(n<0) { tgt_activateprev(obj); return(1); }
		    if(n>0) { tgt_activatenext(obj); return(1); }
		    return(0);
	    }
	    return(0);
	case TGT_OBJECT_SETTAG:
	    idata=obj->class_data;
            switch(a)
            {
                case TGTT_SLIDER_VALUE:
		    idata->value=(int) b;
                    return(1);
		case TGTT_SLIDER_MINVALUE:
		    idata->minvalue=(int) b;
		    return(1);
		case TGTT_SLIDER_MAXVALUE:
		    idata->maxvalue=(int) b;
		    return(1);
            }
	    return(0);
	case TGT_OBJECT_GETTAG:
	    idata=obj->class_data;
            switch(a)
            {
                case TGTT_SLIDER_VALUE:
		    *((int*) b)=idata->value;
                    return(1);
		case TGTT_SLIDER_MINVALUE:
		    *((int*) b)=idata->minvalue;
		    return(1);
		case TGTT_SLIDER_MAXVALUE:
		    *((int*) b)=idata->maxvalue;
		    return(1);
            }
	    return(0);
	default: return(0);
    }
}
