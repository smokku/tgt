#include <stdio.h>
#include <string.h>
#include "tgt.h"
#include "tgt_menu.h"

struct tgt_int_menu
{
    void * mainmenu;
    void * currentmenu;
    int selcolor;
    int framecolor;
    int open;
};
int tgt_builtin_menu(struct tgt_object *obj,int type,int a,void *b)
{
    struct tgt_int_menu *iw;
    int act,id;
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    iw=(struct tgt_int_menu*) malloc(sizeof(struct tgt_int_menu));
	    obj->objectf=(int(*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    obj->objflags=TGT_OBJFLAGS_NONSELECTABLE;
	    obj->class_data=(void*) iw;
	    iw->framecolor=tgt_getnumtag(b,TGTT_MENU_FRAMECOLOR,obj->fg);
	    iw->selcolor=tgt_getnumtag(b,TGTT_MENU_SELCOLOR,6);
	    iw->open=0;
	    iw->mainmenu=tgt_getptrtag(b,TGTT_MENU_ITEMS,NULL);
	    iw->currentmenu=iw->mainmenu;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    iw=obj->class_data;
	    free(iw);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    iw=obj->class_data;
	    act=tgt_hasfocus(obj);
	    if(act) tgt_menu_draw(obj->term,iw->currentmenu,obj->fg,obj->bg,iw->framecolor,iw->selcolor,a+obj->x,(int) b+obj->y,obj->xs);
	    return(1);
	case TGT_OBJECT_HANDLE:
	    iw=obj->class_data;
	    if(tgt_menu_ishorizontal(iw->currentmenu))
	    {
		switch(a)
		{
		    case TGT_KEY_DOWN: a=TGT_KEY_RIGHT; break;
		    case TGT_KEY_UP: a=TGT_KEY_LEFT; break;
		    case TGT_KEY_LEFT: a=TGT_KEY_UP; break;
		    case TGT_KEY_RIGHT: a=TGT_KEY_DOWN; break;
		}
	    }
	    switch(a)
		{
		    case TGT_KEY_DOWN:
			tgt_menu_chp(iw->currentmenu,1);
			tgt_refresh(obj);
			break;
		    case TGT_KEY_UP:
			tgt_menu_chp(iw->currentmenu,-1);
			tgt_refresh(obj);
			break;
		    case 13: case 10: case TGT_KEY_RIGHT:
			id=tgt_menu_enter(&iw->currentmenu);
			if(id==-1)
			{
			    tgt_refresh(obj);
			    return(1);
			}
			else
			{
			    if(a!=TGT_KEY_RIGHT)
			    {
				iw->currentmenu=iw->mainmenu;
				tgt_activatenext(obj); 
				tgt_refresh(tgt_findmparent(obj));
				if(obj->objectf) obj->objectf(obj,id);
			    }
			    return(1);
			}
		    case TGT_KEY_DELETE:
			iw->currentmenu=iw->mainmenu;
			tgt_activatenext(obj); 
			tgt_refresh(tgt_findmparent(obj));
			return(1);
		    case TGT_KEY_LEFT:
			iw->currentmenu=tgt_menu_parent(iw->currentmenu);
			tgt_refresh(tgt_findmparent(obj));
			return(1);			
		}
	    return(1);
	case TGT_OBJECT_GETTAG:
	    iw=obj->class_data;
	    return(0);
	case TGT_OBJECT_SETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_MENU_ITEMS: 
		    iw->mainmenu = b;
		    iw->currentmenu=iw->mainmenu;
		    break;
		case TGTT_MENU_FRAMECOLOR: iw->framecolor=(int) b; return(1);
		case TGTT_MENU_SELCOLOR: iw->selcolor=(int) b; return(1);
	    }
	    return(0);
	    default: return(0);
    }
}
