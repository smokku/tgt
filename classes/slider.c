#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tgt.h"

struct tgt_int_slider
{
    int activebg;
    char *title;
    int value, minvalue, maxvalue;
    int type;
    int kl;
};


static int d_actbg=6;

int tgt_builtin_slider(struct tgt_object *obj,int type,int a,void *b)
{
    int n,i,k;
    struct tgt_int_slider *idata;
    float m;
    char *title;
    tgt_cell attr;
    tgt_cell *buff;
    char atb[20];    

    switch(type)
    {
	case TGT_OBJECT_GETSIZES:
	    n=8;
	    if( (i=snprintf(atb,19,"%d",(int) tgt_getnumtag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_SLIDER_MINVALUE,0))) > n) n=i;
	    if( (i=snprintf(atb,19,"%d",(int) tgt_getnumtag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_SLIDER_MAXVALUE,0))) > n) n=i;
	    n+=strlen(tgt_getptrtag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_SLIDER_CAPTION,""));
            ((struct tgt_ac_objectinfo*) b)->xsize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_XS,n);
	    ((struct tgt_ac_objectinfo*) b)->ysize=1;
	    ((struct tgt_ac_objectinfo*) b)->sizeflags|=TGT_AC_SF_YFIXED;
	    return(1);
	case TGT_OBJECT_CREATE:
	    title=(char*) tgt_getptrtag(b,TGTT_SLIDER_CAPTION,NULL);
	    if(title==NULL) title="";
	    idata=(struct tgt_int_slider*) malloc(sizeof(struct tgt_int_slider));
	    idata->title=title;
	    idata->minvalue=tgt_getnumtag(b,TGTT_SLIDER_MINVALUE,0);
	    idata->maxvalue=tgt_getnumtag(b,TGTT_SLIDER_MAXVALUE,100);
	    idata->value=tgt_getnumtag(b,TGTT_SLIDER_VALUE,0);
	    if(idata->value > idata->maxvalue) idata->value=idata->maxvalue;
	    if(idata->value < idata->minvalue) idata->value=idata->minvalue;
	    idata->type=tgt_getnumtag(b,TGTT_SLIDER_TYPE,0);
	    idata->activebg=tgt_getnumtag(b,TGTT_SLIDER_ACTIVEBG,d_actbg);
	    obj->objectf=(int (*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    obj->class_data=idata;
	    idata->kl=1;
	    return(1);
	case TGT_OBJECT_VISUALINIT:
	    obj->ys = 1;
	    tgt_realloc_buffer(obj);
	    return(1);
	case TGT_OBJECT_DESTROY:
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    idata=obj->class_data;
	    attr=tgt_hasfocus(obj) ?
		    TGT_T_BUILDCELL(obj->fg,idata->activebg,1,0,0) :
		    TGT_T_BUILDCELL(obj->fg,obj->bg,0,0,0);

	    buff=obj->visual_buffer;

	    if(idata->type & TGT_SLIDERF_SHOWVALUE)
		if(idata->type & TGT_SLIDERF_PERCENT)
		    snprintf(atb,19,"]%3d%%", 100 * (idata->value - idata->minvalue) / (idata->maxvalue - idata->minvalue));
		else
		    snprintf(atb,19,"] %d ", idata->value);
	    else
		snprintf(atb,19,"]");

	    n=tgt_printf(buff,obj->xs-strlen(atb),attr,"%s[", idata->title);
	    buff+=n;
	    n=obj->xs-n-strlen(atb);
	    m=(n-1) * (idata->value - idata->minvalue /*-1*/) / (idata->maxvalue - idata->minvalue);
	    for(i=0;i<n;i++) *(buff++)=TGT_T_FCHAR(attr,'-');
	    idata->kl=n;
	    tgt_printf(buff,obj->xs,attr,"%s",atb);
	    
	    obj->visual_buffer[strlen(idata->title)+1+((int) m)]=TGT_T_FCHAR(attr,'V');
	    return(1);
	case TGT_OBJECT_HANDLE:
	    idata=obj->class_data;
	    switch(a)
	    {
		case TGT_KEY_MOUSEDOWN:
		    k=tgt_mouse_get_x() - strlen(idata->title);
		    if(k>idata->kl) k=idata->kl;
		    if(k<0) k=0;
		    idata->value = ((idata->maxvalue - idata->minvalue) * k) / idata->kl + idata->minvalue;
		    tgt_refresh(obj);
		    if(obj->objectf) obj->objectf(obj, idata->value);
		    return(1);
		case TGT_KEY_RIGHT:
		    idata->value++;
		    if(idata->value > idata->maxvalue) idata->value=idata->maxvalue;
		    tgt_refresh(obj);
		    if(obj->objectf) obj->objectf(obj, idata->value);
		    return(1);
		case TGT_KEY_LEFT:
		    idata->value--;
		    if(idata->value < idata->minvalue) idata->value=idata->minvalue;
		    tgt_refresh(obj);
		    if(obj->objectf) obj->objectf(obj, idata->value);
		    return(1);
		case TGT_KEY_HOME:
		    idata->value=idata->minvalue;
		    tgt_refresh(obj);
		    if(obj->objectf) obj->objectf(obj, idata->value);
		    return(1);
		case TGT_KEY_END: 
		    idata->value=idata->maxvalue;
		    tgt_refresh(obj);
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
		case TGTT_SLIDER_CAPTION:
		    idata->title=(char*) b;
		    return(1);
                case TGTT_SLIDER_VALUE:
		    idata->value=(int) b;
		    if(idata->value > idata->maxvalue) idata->value=idata->maxvalue;
		    if(idata->value < idata->minvalue) idata->value=idata->minvalue;
                    return(1);
		case TGTT_SLIDER_MINVALUE:
		    idata->minvalue=(int) b;
		    if(idata->minvalue > idata->maxvalue-1) idata->minvalue=idata->maxvalue-1;
		    return(1);
		case TGTT_SLIDER_MAXVALUE:
		    idata->maxvalue=(int) b;
		    if(idata->maxvalue < idata->minvalue+1) idata->maxvalue=idata->minvalue+1;
		    return(1);
		case TGTT_SLIDER_TYPE: idata->type=(int) b; return(1);
		case TGTT_SLIDER_ACTIVEBG: idata->activebg=(int) b; return(1);

            }
	    return(0);
	case TGT_OBJECT_GETTAG:
	    idata=obj->class_data;
            switch(a)
            {
                case TGTT_SLIDER_CAPTION:
		    *((tgtt*) b)=(tgtt) idata->title;
                    return(1);
                case TGTT_SLIDER_VALUE:
		    *((tgtt*) b)=(tgtt) idata->value;
                    return(1);
		case TGTT_SLIDER_MINVALUE:
		    *((tgtt*) b)=(tgtt) idata->minvalue;
		    return(1);
		case TGTT_SLIDER_MAXVALUE:
		    *((tgtt*) b)=(tgtt) idata->maxvalue;
		    return(1);
		case TGTT_SLIDER_TYPE:
		    *((tgtt*) b)=(tgtt) idata->type;
		    return(1);
		case TGTT_SLIDER_ACTIVEBG:
		    *((tgtt*) b)=(tgtt) idata->activebg;
		    return(1);
            }
	    return(0);
	case TGT_OBJECT_SETDEFAULTS:
	    d_actbg=atoi(tgt_getprefs(b,"slider","activebg","6"));
	    return(1);				
	case TGT_OBJECT_MOUSEDRAG:
	    if(a<0) tgt_builtin_slider(obj,TGT_OBJECT_HANDLE,TGT_KEY_LEFT,NULL);
	    if(a>0) tgt_builtin_slider(obj,TGT_OBJECT_HANDLE,TGT_KEY_RIGHT,NULL);
	    return(1);
	default: return(0);
    }
}
