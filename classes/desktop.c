#include <stdio.h>
#include "tgt.h"
int switches_next[]={'`',TGT_KEY_DOWN,TGT_KEY_RIGHT,0};
int switches_prev[]={TGT_KEY_NONE,TGT_KEY_UP,TGT_KEY_LEFT,0};

extern struct tgt_terminal * g_terminal;

int tgt_builtin_desktop(struct tgt_object *obj,int type,int a,void* b)
{
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    tgt_initconswitch();
	    obj->xs=g_terminal->x_size;
	    obj->ys=g_terminal->y_size;
	    obj->objectf=(int (*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    if(tgt_getnumtag(b,TGTT_DESKTOP_HIDECURSOR,0)) tgt_chattr(TGT_TA_INVISIBLECURSOR,0,0);
	    
	    obj->objflags|=TGT_OBJFLAGS_REFRESHBASE;
	    if(!(obj->next_keys)) obj->next_keys=switches_next;
	    if(!(obj->prev_keys)) obj->prev_keys=switches_prev;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    if(obj->objflags & TGT_OBJFLAGS_NETLINKED) return(1);
	    tgt_chattr(TGT_TA_NORMAL,0,0);
	    tgt_chattr(TGT_TA_TXT,0,0);
	    tgt_chattr(TGT_TA_CLEAR,0,0);
	    tgt_chattr(TGT_TA_VISIBLECURSOR,0,0);
	    fflush(stdout);
	    tgt_rawcon(0);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    tgt_fill_buffer(obj->visual_buffer,obj->xs,obj->ys,TGT_T_BUILDCELL(obj->fg,obj->bg,0,0,' '));
	    return(1);
	case TGT_OBJECT_HANDLE:
	    if(a==0x0c)
	    {
		tgt_fill_buffer(g_terminal->contents_buffer,obj->xs,obj->ys,0);
		tgt_refresh(obj);
	    }
	    if(obj->objectf) return(obj->objectf(obj,a));
	    return(0);
	case TGT_OBJECT_SETTAG:
	    if(a==TGTT_DESKTOP_HIDECURSOR) { if(b) 
		tgt_chattr(TGT_TA_INVISIBLECURSOR,0,0);
	    else
		tgt_chattr(TGT_TA_VISIBLECURSOR,0,0);
	    return(1); }
	default: return(0);
	
    }
}
