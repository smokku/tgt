#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tgt.h"

struct tgt_int_selectbox
{
    struct tgt_object * menuobj;
    char **items;
    int current;
    int icount;
    char *ds;
    char *de;
    int activebg;
};
    char defs[8]="[<";
    char defe[8]=">]";
    static int d_actbg=6; /* default background color when the activation button is active*/

int tgt_builtin_selectbox_callback(struct tgt_object * obj,int pos)
{
    ((struct tgt_int_selectbox*) ((struct tgt_object*) obj->classlink_data)->class_data)->current=pos;
    tgt_setfocus((struct tgt_object*) obj->classlink_data);
    if(((struct tgt_object*) obj->classlink_data)->objectf) ((struct tgt_object*) obj->classlink_data)->objectf((struct tgt_object*) obj->classlink_data,pos);
    return(0);
}
int tgt_builtin_selectbox(struct tgt_object *obj,int type,int a,void *b)
{
    struct tgt_int_selectbox *iw;
    char **items = NULL;
    int act,i=0,n,ms;
    tgt_cell attr;
    char fstr[20];

    switch(type)
    {
	case TGT_OBJECT_GETSIZES:
	    ms=act=strlen(defs)+strlen(defe);
	    if((items=(char**) tgt_getptrtag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_SELECTBOX_ITEMS,NULL)))
	    {
		for(i=0;items[i];i++)
		    if((n=(strlen(items[i])+act)) > ms) ms = n;
	    }
	    
            ((struct tgt_ac_objectinfo*) b)->xsize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_XS,ms);
            ((struct tgt_ac_objectinfo*) b)->ysize=1;
            ((struct tgt_ac_objectinfo*) b)->sizeflags|=TGT_AC_SF_YFIXED;
	    return(1);				    
	case TGT_OBJECT_LINK:
	    iw=obj->class_data;
	    tgt_link(iw->menuobj,obj->ln.parent);
	    return(1);
	case TGT_OBJECT_UNLINK:
	    iw=obj->class_data;
	    tgt_unlink(iw->menuobj);
	    return(1);
	case TGT_OBJECT_CREATE:
	    items=(char**) tgt_getptrtag(b,TGTT_SELECTBOX_ITEMS,NULL);
	    iw=(struct tgt_int_selectbox*) malloc(sizeof(struct tgt_int_selectbox));
	    obj->objectf=(int(*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    obj->class_data=(void*) iw;
	    iw->activebg=tgt_getnumtag(b,TGTT_SELECTBOX_ACTIVEBG,d_actbg);
	    iw->ds=(char*) tgt_getnumtag(b,TGTT_SELECTBOX_MARK1,(long) defs);
	    iw->de=(char*) tgt_getnumtag(b,TGTT_SELECTBOX_MARK2,(long) defe);
	    iw->items=items;
	    iw->menuobj=tgt_menu_getcontext();
	    iw->current=0; 
	    i=0;
	    if(items) 
		for(i=0;items[i]!=NULL;i++) tgt_menu_add(iw->menuobj,items[i],0,NULL);
	    iw->menuobj->classlink_data=(void*) obj;
	    tgt_set(iw->menuobj,TGTT_CALLBACK,(tgtt) tgt_builtin_selectbox_callback);
	    iw->icount=i;

	    if((iw->current = tgt_getnumtag(b,TGTT_SELECTBOX_SELECTED, 0)))
	        tgt_set(iw->menuobj, TGTA_MENU_SELECT, iw->current);
	    return(1);
	case TGT_OBJECT_VISUALINIT:
	    obj->ys = 1;
	    tgt_realloc_buffer(obj);
	    return(1);
	case TGT_OBJECT_DESTROY:
	    iw=obj->class_data;
	    if(!(obj->objflags & TGT_OBJFLAGS_RECURSIVEKILL)) tgt_destroyobject(iw->menuobj);
	    free(iw);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    iw=obj->class_data;
	    if(!iw->items) return(0);

	    attr=tgt_hasfocus(obj)?
		TGT_T_BUILDCELL(obj->fg,iw->activebg,1,0,0) :
		TGT_T_BUILDCELL(obj->fg,obj->bg,0,0,0);
		
	    snprintf(fstr,19,"%%s%%-%ds%%s", obj->xs-strlen(iw->ds)-strlen(iw->de));

	    tgt_flprintf(obj->visual_buffer,obj->xs,attr,fstr,iw->ds,iw->items[iw->current],iw->de);
	    return(1);
	case TGT_OBJECT_HANDLE:
	    iw=obj->class_data;
	    switch(a)
	    {
		case 10:
		case TGT_KEY_MOUSEDOWN:
		    tgt_menu_enable(iw->menuobj,obj->x,obj->y+1);
		    return(1);
		case TGT_KEY_PGDN:
		case ' ':
		    if((++iw->current) >= iw->icount) iw->current=0;
		    tgt_refresh(obj);
		    if(obj->objectf) obj->objectf(obj, iw->current);
		    return(1);
		case TGT_KEY_PGUP:
		    if((--iw->current) < 0) iw->current=iw->icount-1;
		    tgt_refresh(obj);
		    if(obj->objectf) obj->objectf(obj, iw->current);
		    return(1);
	    }
	    n=tgt_shalliswitch(obj,a,0);
	    if(n<0) { tgt_activateprev(obj); return(1); }
	    if(n>0) { tgt_activatenext(obj); return(1); }				
	    return(0);
	case TGT_OBJECT_GETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_SELECTBOX_SELECTED: *((tgtt*) b)=(tgtt) iw->current; return(1);
		case TGTT_SELECTBOX_ITEMS: *((tgtt*) b)=(tgtt) iw->items; return(1);
		case TGTT_SELECTBOX_MARK1: *((tgtt*) b)=(tgtt) iw->ds; return(1);
		case TGTT_SELECTBOX_MARK2: *((tgtt*) b)=(tgtt) iw->de; return(1);
	    }
	    return(0);
	case TGT_OBJECT_SETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_SELECTBOX_SELECTED:
		    iw->current=(int) b;
		    tgt_set(iw->menuobj, TGTA_MENU_SELECT, iw->current);
		    return(1);
		case TGTT_SELECTBOX_MARK1: iw->ds=b; return(1);
		case TGTT_SELECTBOX_MARK2: iw->de=b; return(1);	
		case TGTT_SELECTBOX_ITEMS: 
		    if(iw->menuobj->ln.parent) tgt_unlink(iw->menuobj);
		    tgt_destroyobject(iw->menuobj);
		    iw->menuobj=tgt_menu_getcontext();
		    

		    iw->menuobj->classlink_data=(void*) obj;
		    tgt_set(iw->menuobj,TGTT_CALLBACK,(tgtt) tgt_builtin_selectbox_callback);
		    items=b; iw->items=items;
		    iw->current=0;

		    if(items) 
		    {
		        for(i=0;items[i]!=NULL;i++) tgt_menu_add(iw->menuobj,items[i],0,NULL);
			iw->icount=i;
		    }
		    if(obj->ln.parent) tgt_link(iw->menuobj,obj->ln.parent);
		    return(1);
	    }
	    return(0);
	case TGT_OBJECT_SETDEFAULTS:
	    memcpy(defs,tgt_getprefs(b,"selectbox","smark","[<"),7); defs[7]=0;
	    memcpy(defe,tgt_getprefs(b,"selectbox","emark",">]"),7); defe[7]=0;
	    return(1);
	default: return(0);

    }
}
