#include <stdio.h>
#include "tgt.h"

struct tgt_int_button
{
    int activebg;
    char *title;
};
static int d_actbg=6;

int tgt_builtin_button(struct tgt_object *obj,int type,int a,void *b)
{
    int i,act,n;
    struct tgt_int_button *iw;
    char *title;
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    title=(char*) tgt_getptrtag(b,TGTT_BUTTON_CAPTION,NULL);
	    iw=(struct tgt_int_button*) malloc(sizeof(struct tgt_int_button));
	    obj->class_data=iw;
	    iw->title=title;
	    iw->activebg=tgt_getnumtag(b,TGTT_BUTTON_ACTIVEBG,d_actbg);
	    obj->objectf=(int (*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    return(1);
	case TGT_OBJECT_DESTROY:
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    iw=obj->class_data;
	    act=tgt_hasfocus(obj);
	    tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a,obj->y+(int) b);
	    if(act==1) 
	    {	
		tgt_chattr(obj->term,TGT_TA_BOLD,0,0);
		tgt_chattr(obj->term,TGT_TA_BGCOLOR,iw->activebg);
	    }
	    else
		tgt_chattr(obj->term,TGT_TA_BGCOLOR,obj->bg);

	    tgt_chattr(obj->term,TGT_TA_FGCOLOR,obj->fg);
	    if(iw->title) printf("%s",iw->title);
	    tgt_chattr(obj->term,TGT_TA_NORMAL,0,0);
	    fflush(stdout);
	    return(1);
	case TGT_OBJECT_HANDLE:
	    if(a==13 || a==10 || a==32) 
	    {
		if(obj->objectf!=NULL) obj->objectf(obj);
		return(1);
	    }
	    n=tgt_shalliswitch(obj,a,0);
	    if(n<0) { tgt_activateprev(obj); return(1); }
	    if(n>0) { tgt_activatenext(obj); return(1); }
	    return(0);
	case TGT_OBJECT_SETDEFAULTS:
	    d_actbg=atoi(tgt_getprefs(b,"button","activebg","6"));
	    return(1);
	case TGT_OBJECT_SETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_BUTTON_CAPTION: iw->title=(char*) b; return(1);
		case TGTT_BUTTON_ACTIVEBG: iw->activebg=(int) b; return(1);
	    }
	    return(0);
	default: return(0);
    }
}
