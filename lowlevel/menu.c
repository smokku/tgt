#include "tgt.h"
#include <stdio.h>
#define TGT_MENU_PROTOS
#include "tgt_menu.h"
#include <stdlib.h>
#define max(a,b) ((a) > (b) ? (a) : (b))

struct tgt_int_menuitem 
{ 
    char *name;
    struct tgt_object *child; 
    int selectable;
};


void tgt_menu_rerender(tgt_cell * buffer,int xs,int ys,
		       struct tgt_int_menuitem * items,
		       int fg,int bg,int framecolor)
{
    tgt_cell attr=TGT_T_BUILDCELL(fg,bg,1,0,0);
    tgt_cell fcattr=TGT_T_FG(attr,framecolor);
    int j,i,k,n=ys-2;
    
    tgt_int_upperb(buffer,xs,TGT_T_FG(attr,framecolor));buffer+=xs;
    for(i=0;i<n;i++)
    {
	if(items[i].name)
	{
    	    *(buffer++)=TGT_T_FCHAR(fcattr,TGT_TC_VL);
    	    tgt_flprintf(buffer,xs-2,attr,"%s",items[i].name);
    	    buffer+=xs-2;
    	    *(buffer++)=TGT_T_FCHAR(fcattr,TGT_TC_VL);
	}
	else
	{
    	    *(buffer++)=TGT_T_FCHAR(fcattr,TGT_TC_TE);
    	    for(j=0,k=xs-2;j<k;j++) *(buffer++)=TGT_T_FCHAR(fcattr,TGT_TC_HL);
    	    *(buffer++)=TGT_T_FCHAR(fcattr,TGT_TC_TS);
	}
    }
    tgt_int_lowerb(buffer,xs,TGT_T_FG(attr,framecolor));buffer+=xs;
}

void tgt_menu_chcolors(tgt_cell * buffer,int n,int xs,int ys,int fg,int bg)
{
    int i,q=xs-2;
    if(n>=0 && n<=(ys-2))
    {
	buffer+=(n+1)*xs+1;
	for(i=0;i<q;i++) *(buffer++)=TGT_T_FG(TGT_T_BG(*buffer,bg),fg);
    }
}

void tgt_menu_enable(struct tgt_object *obj,int x,int y)
{    
    struct tgt_object *parent=obj->ln.parent;
    obj->x=x; obj->y=y;
    if(parent)
    {
	if((obj->x+obj->xs)>parent->xs) obj->x=parent->xs-obj->xs;
	if((obj->y+obj->ys)>parent->ys) obj->y=parent->ys-obj->ys;
    }
    obj->objflags&=~(TGT_OBJFLAGS_INVISIBLE);
    tgt_setfocus(obj);
    tgt_lock_mouse(obj);
}

struct tgt_object * tgt_menu_getcontext(struct tgt_terminal *term)
{
    return(tgt_createobject(term,TGT_CLASS_MENU,NULL));
}

void tgt_menu_toggleselectable(struct tgt_object * menu,int pos)
{
    menu->classf(menu,TGT_OBJECT_SETTAG,TGTA_MENU_SWITCHSELECTABILITY,(void*) pos);
}

void tgt_menu_add(struct tgt_object * obj,char * item,int nonselectable,struct tgt_object * submenu)
{
    obj->classf(obj,TGT_OBJECT_SETTAG,TGTA_MENU_ADDITEM,(void*) item);
    if(!item) nonselectable=1;
    if(nonselectable) obj->classf(obj,TGT_OBJECT_SETTAG,TGTA_MENU_SWITCHSELECTABILITYLAST,0);
    if(submenu) obj->classf(obj,TGT_OBJECT_SETTAG,TGTA_MENU_ASSIGNSUBMENU,(void*) submenu);
}
