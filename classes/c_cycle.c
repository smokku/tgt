#include <stdio.h>
#include <string.h>
#include "tgt.h"
#include "tgt_menu.h"

struct tgt_int_cycle
{
    void * menuobj;
    char **items;
    int current;
    int selcolor;
    int framecolor;
    int open;
    int mx;
    char *ds;
    char *de;
};
    char *defs="[<";
    char *defe=">]";
    
int tgt_builtin_cycle(struct tgt_object *obj,int type,int a,void *b)
{
    struct tgt_int_cycle *iw;
    char **items;
    char *text;
    int act,id,i,n,l;
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    items=(char**) tgt_gettag(b,TGTT_CYCLE_ITEMS,(long) NULL);
	    if(items==NULL) return(0);
	    iw=(struct tgt_int_cycle*) malloc(sizeof(struct tgt_int_cycle));
	    obj->objectf=(int(*)()) tgt_gettag(b,TGTT_CALLBACK,(long) NULL);
	    obj->class_data=(void*) iw;
	    iw->framecolor=tgt_gettag(b,TGTT_CYCLE_POPFRAMECOLOR,obj->fg);
	    iw->selcolor=tgt_gettag(b,TGTT_CYCLE_POPSELCOLOR,6);
	    iw->ds=(char*) tgt_gettag(b,TGTT_CYCLE_MARK1,(long) defs);
	    iw->de=(char*) tgt_gettag(b,TGTT_CYCLE_MARK2,(long) defe);
	    
	    iw->open=0; iw->items=items;
	    iw->menuobj=tgt_menu_context(0);
	    iw->current=0;
	    for(i=0;items[i]!=NULL;i++) tgt_menu_add(iw->menuobj,items[i],i,0,NULL);
	    iw->mx=tgt_menu_getmaxsize(iw->menuobj);
	    return(1);
	case TGT_OBJECT_DESTROY:
	    iw=obj->class_data;
	    tgt_menu_destroycontext(iw->menuobj);
	    free(iw);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    iw=obj->class_data;
	    act=tgt_hasfocus(obj);
	    if(iw->open)
		tgt_menu_draw(obj->term,iw->menuobj,obj->fg,obj->bg,iw->framecolor,iw->selcolor,a+obj->x,(int) b+obj->y,0);
	    else
	    {
		tgt_chattr(obj->term,TGT_TA_COLORS,obj->fg,obj->bg);
	        if(act)
		    tgt_chattr(obj->term,TGT_TA_BOLD,0,0);
		tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a,obj->y+(int) b);
		printf(iw->ds);
		
		text=iw->items[iw->current];
		n=iw->mx; for(i=0;text[i]!=0;i++) putchar(text[i]);
		for(;i<n;i++) putchar(' ');
		
		printf(iw->de);
		tgt_chattr(obj->term,TGT_TA_NORMAL,0,0);
		fflush(stdout);
	    }
	    return(1);
	case TGT_OBJECT_HANDLE:
	    iw=obj->class_data;
	    if(iw->open)
	    {
		switch(a)
		{
		    case TGT_KEY_DOWN:
			tgt_menu_chp(iw->menuobj,1);
			tgt_refresh(obj);
			break;
		    case TGT_KEY_UP:
			tgt_menu_chp(iw->menuobj,-1);
			tgt_refresh(obj);
			break;
		    case 13: case 10: 
			id=tgt_menu_enter(&iw->menuobj);
			if(id==-1)
			{
			    tgt_refresh(obj);
			    return(1);
			}
			else
			{
			    iw->current=id;
			    iw->open=0;
			    tgt_refresh(tgt_findmparent(obj));
			    if(obj->objectf) obj->objectf(obj,id);
			    return(1);
			}
		    case TGT_KEY_DELETE:
			iw->open=0;
			tgt_refresh(tgt_findmparent(obj));
			return(1);
		}
	    }
	    else
	    {
		if(a==13 || a==32) { iw->open=1; tgt_refresh(obj); return(1); }
	        n=tgt_shalliswitch(obj,a,0);
		if(n<0) { tgt_activateprev(obj); return(1); }
		if(n>0) { tgt_activatenext(obj); return(1); }				
	    }
	    return(0);
	case TGT_OBJECT_GETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_CYCLE_SELECTED: *((int*) b)=iw->current; return(1);
	    }
	    return(0);
	case TGT_OBJECT_SETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_CYCLE_SELECTED:
		    iw->current=(int) b;
		    tgt_menu_setactive(iw->menuobj,(int) b);
		    return(1);
		case TGTT_CYCLE_MARK1: iw->ds=b; break;
		case TGTT_CYCLE_MARK2: iw->de=b; break;	
	    }
	    return(0);
	case TGT_OBJECT_SETDEFAULTS:
	    defs=tgt_getprefs(b,"cycle","smark","[<");
	    defe=tgt_getprefs(b,"cycle","emark",">]");
	    return(1);
	default: return(0);

    }
}
