#include <stdio.h>
#include "tgt.h"

    static char defaulttitle[]="Window";
    int wswitches_next[]={'\t',TGT_KEY_DOWN,TGT_KEY_RIGHT,0};
    int wswitches_prev[]={TGT_KEY_NONE,TGT_KEY_UP,TGT_KEY_LEFT,0};

struct tgt_int_window
{
    int borderfg;
    int titlefg;
    char *title;
};

int tgt_builtin_window(struct tgt_object *obj,int type,int a,void *b)
{
    int i,n;
    int act;
    struct tgt_int_window *iw;
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    iw=(struct tgt_int_window*) malloc(sizeof(struct tgt_int_window));
	    obj->class_data=iw;
	    iw->title=(char*) tgt_gettag(b,TGTT_WINDOW_TITLE,(long) defaulttitle);
	    iw->borderfg=tgt_gettag(b,TGTT_WINDOW_BORDERCOLOR,6);
	    iw->titlefg=tgt_gettag(b,TGTT_WINDOW_TITLECOLOR,7);
            if(!(obj->next_keys)) obj->next_keys=wswitches_next;
	    if(!(obj->prev_keys)) obj->prev_keys=wswitches_prev;		
	    return(1);
	case TGT_OBJECT_DESTROY:
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    act=tgt_isactive(obj);
	    if(act==1) tgt_chattr(obj->term,TGT_TA_BOLD,0,0);
	    iw=obj->class_data;
	    tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a,(void*) ((int) (obj->y+(int) b)));
	    tgt_drawtitleb(obj->term,obj->xs,iw->title,iw->borderfg,obj->bg,iw->titlefg);
	    
	    tgt_drawsideb(obj->term,obj->x+a,obj->y+1+(int) b,obj->ys-2,obj->xs);
	    tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a,obj->y+(int) b+obj->ys-1);
	    tgt_drawlowerb(obj->term,obj->xs);	    
	    if(act==1) tgt_chattr(obj->term,TGT_TA_NORMAL,0,0);
	    tgt_chattr(obj->term,TGT_TA_TXT,0,0);
	    fflush(stdout);
	    return(1);
	case TGT_OBJECT_HANDLE:
	    n=tgt_shalliswitch(obj,a,0);
	    if(n<0) { tgt_activateprev(obj); return(1); }
	    if(n>0) { tgt_activatenext(obj); return(1); }
// in future eventually window moving (for instance) functions by keys
// like alt + -> 
            if(obj->objectf) return(obj->objectf(a));
	    return(0);
	    break;
	default: return(0);
    }
}
