#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tgt.h"

int tgt_builtin_menu(struct tgt_object *obj,int type,int a,void *b)
{
    struct tgt_int_menu *iw;
    struct tgt_object *child;
    int act,k,i;
    char * iname;
    
    switch(type)
    {
	case TGT_OBJECT_LINK:
	    iw=(struct tgt_int_menu *) obj->class_data;
	    iw->fdpos=0;
    	    if(obj->ln.parent) if(obj->ln.parent->xs<obj->xs) obj->xs=obj->ln.parent->xs;
	    if(obj->ln.parent) if(obj->ln.parent->ys<obj->ys) obj->ys=obj->ln.parent->ys;
	    tgt_realloc_buffer(obj);
	    tgt_menu_rerender(obj->visual_buffer,obj->xs,obj->ys,iw->items,obj->fg,obj->bg,iw->framecolor);
	    if(iw->nitems)
		tgt_menu_chcolors(obj->visual_buffer,(iw->active=0)-iw->fdpos,obj->xs,obj->ys,obj->fg,iw->selcolor);
	    return(1);	
	case TGT_OBJECT_CREATE:
	    iw=(struct tgt_int_menu*) malloc(sizeof(struct tgt_int_menu));
	    obj->objectf=(int(*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    obj->objflags=TGT_OBJFLAGS_NONSELECTABLE|TGT_OBJFLAGS_INVISIBLE;
	    obj->class_data=(void*) iw;
	    iw->framecolor=tgt_getnumtag(b,TGTT_MENU_FRAMECOLOR,obj->fg);
	    iw->selcolor=tgt_getnumtag(b,TGTT_MENU_SELCOLOR,6);
	    iw->parent=NULL; iw->items=NULL; iw->nitems=0; iw->active=-1;
	    iw->fdpos=0;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    iw=obj->class_data;
	    if(iw->items) free(iw->items);
	    free(iw);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    return(1);
	case TGT_OBJECT_HANDLE:
	    iw=obj->class_data;
	    switch(a)
		{
		    case TGT_KEY_DOWN:
		    case TGT_KEY_MOUSEDOWN:
			tgt_menu_chcolors(obj->visual_buffer,iw->active-iw->fdpos,obj->xs,obj->ys,obj->fg,obj->bg);
			act=iw->active;
			if(a==TGT_KEY_MOUSEDOWN)
			{
			    k=tgt_mouse_get_y();
			    if(k<1) k=1;
			    if(k>=(obj->ys-1)) k=obj->ys-2;
			    iw->active=iw->fdpos+k-2;
			}
			do
			{
			    iw->active+=1;
			    if(iw->active>=iw->nitems) { iw->active=0; if(act==-1) { act=iw->active; } }
			}
			while( (!iw->items[iw->active].selectable) && iw->active!=act);
			if(iw->active==act)
			{
			    iw->active=-1;
			}
			else
			{
			    if((iw->active-iw->fdpos)>=(obj->ys-2))
			    {
				iw->fdpos=iw->active-(obj->ys-2)+1;
				tgt_menu_rerender(obj->visual_buffer,obj->xs,obj->ys,iw->items+iw->fdpos,obj->fg,obj->bg,iw->framecolor);
			    }
			    if((iw->active-iw->fdpos)<0)
			    {
				iw->fdpos=iw->active;
				tgt_menu_rerender(obj->visual_buffer,obj->xs,obj->ys,iw->items+iw->fdpos,obj->fg,obj->bg,iw->framecolor);
			    }
			
			    tgt_menu_chcolors(obj->visual_buffer,iw->active-iw->fdpos,obj->xs,obj->ys,obj->fg,iw->selcolor);
			    tgt_rebuild_parent(obj);
			}
			return(1);
		    case TGT_KEY_UP:
			tgt_menu_chcolors(obj->visual_buffer,iw->active-iw->fdpos,obj->xs,obj->ys,obj->fg,obj->bg);
			act=iw->active;
			do
			{
			    iw->active-=1;
			    if(iw->active<0) { iw->active=iw->nitems-1; if(act==-1) { act=iw->active; } }
			}
			while( (!iw->items[iw->active].selectable) && iw->active!=act);
			if(iw->active==act)
			{
			    iw->active=-1;
			}
			else
			{
			    if((iw->active-iw->fdpos)>=(obj->ys-2))
			    {
				iw->fdpos=iw->active-(obj->ys-2)+1;
				tgt_menu_rerender(obj->visual_buffer,obj->xs,obj->ys,iw->items+iw->fdpos,obj->fg,obj->bg,iw->framecolor);
			    }
			    if((iw->active-iw->fdpos)<0)
			    {
				iw->fdpos=iw->active;
				tgt_menu_rerender(obj->visual_buffer,obj->xs,obj->ys,iw->items+iw->fdpos,obj->fg,obj->bg,iw->framecolor);
			    }
			    tgt_menu_chcolors(obj->visual_buffer,iw->active-iw->fdpos,obj->xs,obj->ys,obj->fg,iw->selcolor);
			    tgt_rebuild_parent(obj);
			}
			return(1);

			return(1);
		    case 13: case 10: case TGT_KEY_RIGHT: case 32: case TGT_KEY_MOUSEUP:
			if(iw->active>=0)
			{
			    if((child=iw->items[iw->active].child))
			    {
				if(!child->ln.parent) if(obj->ln.parent) tgt_link(child,obj->ln.parent);
				tgt_menu_enable(iw->items[iw->active].child,obj->xs+obj->x-2,iw->active-iw->fdpos+obj->y);
			    }
			    else
			    {
				struct tgt_object *parent;
				for(parent=obj;parent;parent=((struct tgt_int_menu*)parent->class_data)->parent)
				    parent->objflags|=TGT_OBJFLAGS_INVISIBLE;
				if(obj->ln.parent) tgt_refresh(obj->ln.parent);
				tgt_unlock_mouse();
				tgt_activatenext(obj); 
				if(obj->objectf) obj->objectf(obj,iw->active);
			    }
			}
			return(1);
		    case TGT_KEY_DELETE:
		    case TGT_KEY_BKSPC:
		    case TGT_KEY_ESC:
		    case TGT_KEY_LEFT:
			obj->objflags|=TGT_OBJFLAGS_INVISIBLE;
			if(iw->parent)
			    tgt_setfocus(iw->parent);
			else
			{
			    if(obj->ln.parent) tgt_refresh(obj->ln.parent);
			    tgt_unlock_mouse();
			    tgt_activatenext(obj); 
			}
			return(1);
		}
	    return(0);
	case TGT_OBJECT_GETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_MENU_FRAMECOLOR:
		    *((tgtt*) b) = (tgtt) iw->framecolor;
		    return(1);
		case TGTT_MENU_SELCOLOR:
		    *((tgtt*) b) = (tgtt) iw->selcolor;
		    return(1);
	    }
	    return(0);
	case TGT_OBJECT_SETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTA_MENU_ADDITEM: 
		    iw->nitems++;
		    iw->items=(struct tgt_int_menuitem*) realloc(iw->items,sizeof(struct tgt_int_menuitem)*iw->nitems);
		    obj->ys=iw->nitems+2;
		    if(b)
		    {
			if((strlen((char*) b)+2)>obj->xs) obj->xs=strlen((char*) b)+2;
		        if(obj->ln.parent) if(obj->ln.parent->ys<obj->ys) obj->ys=obj->ln.parent->ys;
		    }
	    	    tgt_realloc_buffer(obj);
		    iw->items[iw->nitems-1].name=(char*) b;
		    iw->items[iw->nitems-1].child=NULL;
		    iw->items[iw->nitems-1].selectable=1;
		    iw->fdpos=0;
		    tgt_menu_rerender(obj->visual_buffer,obj->xs,obj->ys,iw->items,obj->fg,obj->bg,iw->framecolor);
		    tgt_menu_chcolors(obj->visual_buffer,(iw->active=0)-iw->fdpos,obj->xs,obj->ys,obj->fg,iw->selcolor);
		    return(1);
		case TGTA_MENU_INSERTITEM:
		    k=(++(iw->nitems));

		    if(((struct tgt_int_menu_exchange*) b)->n>=iw->nitems || ((struct tgt_int_menu_exchange*) b)->n<0) tgt_fatal_error("TGT Menu Class: MENU_INSERTITEM: No such item");

		    iw->items=(struct tgt_int_menuitem*) realloc(iw->items,sizeof(struct tgt_int_menuitem)*k);
		    for(i = k-1, k = ((struct tgt_int_menu_exchange*) b)->n; i>k ; i--) iw->items[i] = iw->items[i-1];

		    obj->ys = iw->nitems+2;
		    iname = ((struct tgt_int_menu_exchange*) b)->p;
		    
		    if(iname)
		    {
			if((strlen((char*) iname)+2)>obj->xs) obj->xs=strlen((char*) iname)+2;
		        if(obj->ln.parent) if(obj->ln.parent->ys<obj->ys) obj->ys=obj->ln.parent->ys;
		    }
	    	    tgt_realloc_buffer(obj);
		    iw->items[k].name = iname;
		    iw->items[k].child = NULL;
		    iw->items[k].selectable = 1;
		    iw->fdpos = 0;
		    tgt_menu_rerender(obj->visual_buffer,obj->xs,obj->ys,iw->items,obj->fg,obj->bg,iw->framecolor);
		    tgt_menu_chcolors(obj->visual_buffer,(iw->active=0)-iw->fdpos,obj->xs,obj->ys,obj->fg,iw->selcolor);
		    return(1);
		case TGTA_MENU_CHANGEITEM:
		    k = ((struct tgt_int_menu_exchange*) b)->n;
		    iname = ((struct tgt_int_menu_exchange*) b)->p;
		    
		    if(k>=iw->nitems || k<0) tgt_fatal_error("TGT Menu Class: MENU_CHANGEITEM: No such item");

		    if(iname)
		    {
			if((strlen((char*) iname)+2)>obj->xs) obj->xs=strlen((char*) iname)+2;
		        if(obj->ln.parent) if(obj->ln.parent->ys<obj->ys) obj->ys=obj->ln.parent->ys;
		    }
	    	    tgt_realloc_buffer(obj);
		    iw->items[k].name = iname;
		    iw->fdpos = 0;
		    tgt_menu_rerender(obj->visual_buffer,obj->xs,obj->ys,iw->items,obj->fg,obj->bg,iw->framecolor);
		    tgt_menu_chcolors(obj->visual_buffer,(iw->active=0)-iw->fdpos,obj->xs,obj->ys,obj->fg,iw->selcolor);
		    return(1);
		case TGTA_MENU_DELETEITEM:
		    i = (int) b;
		    if(i>=iw->nitems || i<0) tgt_fatal_error("TGT Menu Class: MENU_DELETEITEM: No such item");
		    iw->nitems--;
		    for(k=iw->nitems; i<k ; i++) iw->items[i] = iw->items[i+1];
		    if(iw->nitems)
		    {
			iw->items=(struct tgt_int_menuitem*) realloc(iw->items,sizeof(struct tgt_int_menuitem)*iw->nitems);
		    }
		    else
		    {
			free(iw->items);
			iw->items = NULL;
		    }
		    obj->ys = iw->nitems+2;
	    	    tgt_realloc_buffer(obj);
		    iw->fdpos = 0;
		    tgt_menu_rerender(obj->visual_buffer,obj->xs,obj->ys,iw->items,obj->fg,obj->bg,iw->framecolor);
		    if(iw->nitems)
			tgt_menu_chcolors(obj->visual_buffer,(iw->active=0)-iw->fdpos,obj->xs,obj->ys,obj->fg,iw->selcolor);
		    return(1);
		case TGTA_MENU_ASSIGNSUBMENU:
		    if(iw->nitems) 
		    {
			iw->items[iw->nitems-1].child=(struct tgt_object *) b;
			((struct tgt_int_menu*) (((struct tgt_object *) b)->class_data))->parent=obj;
		    }
		    return(1);
		case TGTA_MENU_CHSUBMENU:
		    k = ((struct tgt_int_menu_exchange*) b)->n;

		    if(k>=iw->nitems || k<0) tgt_fatal_error("TGT Menu Class: MENU_CHSUBMENU: No such item");
		    iw->items[k].child=(struct tgt_object *) ((struct tgt_int_menu_exchange*) b)->p;
		    ((struct tgt_int_menu*) 
			((struct tgt_object*)
			    ((struct tgt_int_menu_exchange*) b)->p)->class_data)->parent=obj;
		    return(1);
		case TGTA_MENU_SWITCHSELECTABILITY:
		    k = ((struct tgt_int_menu_exchange*) b)->n;
		    if(k>=iw->nitems || k<0) tgt_fatal_error("TGT Menu Class: MENU_SWITCHSELECTABILITY: No such item");
		    iw->items[k].selectable=((struct tgt_int_menu_exchange*) b)->q;
		    return(1);
		case TGTA_MENU_SWITCHSELECTABILITYLAST:
		    iw->items[iw->nitems-1].selectable=1-iw->items[iw->nitems-1].selectable;
		    return(1);
		case TGTA_MENU_SELECT:
		    tgt_menu_chcolors(obj->visual_buffer,iw->active-iw->fdpos,obj->xs,obj->ys,obj->fg,obj->bg);
		    iw->active = (int) b;
		    if(iw->active >= iw->nitems) iw->active = iw->nitems-1;
		    if(iw->active < 0) iw->active = 0;
		    k = obj->ys - 2;
		    if(iw->active < iw->fdpos || iw->active > (iw->fdpos + k))
		    {
			iw->fdpos = iw->active - (k/2);
			if(iw->fdpos < 0) iw->fdpos = 0;
			if(iw->fdpos > (iw->nitems - k)) iw->fdpos = iw->nitems - k;
			tgt_menu_rerender(obj->visual_buffer,obj->xs,obj->ys,iw->items+iw->fdpos,obj->fg,obj->bg,iw->framecolor);
		    }
		    tgt_menu_chcolors(obj->visual_buffer,iw->active-iw->fdpos,obj->xs,obj->ys,obj->fg,iw->selcolor);
		    return(1);	
		case TGTT_MENU_FRAMECOLOR:
		    iw->framecolor=(int) b; 
		    tgt_menu_rerender(obj->visual_buffer,obj->xs,obj->ys,iw->items+iw->fdpos,obj->fg,obj->bg,iw->framecolor);
		    if(iw->active)
			tgt_menu_chcolors(obj->visual_buffer,iw->active-iw->fdpos,obj->xs,obj->ys,obj->fg,iw->selcolor);
		    return(1);
		case TGTT_MENU_SELCOLOR: 
		    iw->selcolor=(int) b;
		    if(iw->active)
			tgt_menu_chcolors(obj->visual_buffer,iw->active-iw->fdpos,obj->xs,obj->ys,obj->fg,iw->selcolor);
		    return(1);
		case TGTT_FG:
		case TGTT_BG:
		    tgt_menu_rerender(obj->visual_buffer,obj->xs,obj->ys,iw->items+iw->fdpos,obj->fg,obj->bg,iw->framecolor);
		    if(iw->active)
			tgt_menu_chcolors(obj->visual_buffer,iw->active-iw->fdpos,obj->xs,obj->ys,obj->fg,iw->selcolor);
		    return(1);		
	    }
	    return(0);
	case TGT_OBJECT_MOUSEDRAG:
	    if((int) b > 0) tgt_builtin_menu(obj,TGT_OBJECT_HANDLE,TGT_KEY_DOWN,NULL);
	    if((int) b < 0) tgt_builtin_menu(obj,TGT_OBJECT_HANDLE,TGT_KEY_UP,NULL);
	    return(1);
	default: return(0);
    }
}
