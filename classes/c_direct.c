#include <stdio.h>
#include <string.h>
#include "tgt.h"

int tgt_builtin_direct(struct tgt_object *obj,int type,int a,void *b)
{
    int l,fg,bg,bold,old_fg,old_bg,old_bold,x,y,cd;
    unsigned char *surface;
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    obj->class_data=(void*) surface=(unsigned char *) malloc(obj->xs*obj->ys*2);
	    y=obj->ys; y*=obj->xs;
	    cd=(obj->bg << 4) | obj->fg;
	    for(x=0;x<y;x++) { *(surface++)=cd; *(surface++)=' '; }
	    obj->objflags|=TGT_OBJFLAGS_NONSELECTABLE;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    surface=(unsigned char *) obj->class_data;
	    for(y=0;y<obj->ys;y++)
	    {
		old_fg=old_bg=old_bold=-1;
		tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a,obj->y+(int) b+y);
		for(x=0;x<obj->xs;x++)
		{
		    cd=*(surface++);
		    if((bold=TGTD_GETBOLD(cd))!=old_bold)
		    {
			if(old_bold=bold)
			    tgt_chattr(obj->term,TGT_TA_BOLD,0,0);
			else
			{
			    old_fg=old_bg=-1;
			    tgt_chattr(obj->term,TGT_TA_NORMAL,0,0);
			}
		    }
		    if((fg=TGTD_GETFG(cd))!=old_fg) tgt_chattr(obj->term,TGT_TA_FGCOLOR,old_fg=fg,0);
		    if((bg=TGTD_GETBG(cd))!=old_bg) tgt_chattr(obj->term,TGT_TA_BGCOLOR,old_bg=bg,0);
		    putchar(*(surface++));
		}
	    }
	    fflush(stdout);
	    return(1);
	case TGT_OBJECT_GETTAG:
	    if(a==TGTT_DIRECTSURFACE_SCREENMEM) { *(unsigned char **) b=obj->class_data; return(1); }
	    return(0);
	default: return(0);
    }
}
