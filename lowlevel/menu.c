#include "tgt.h"
#include <stdio.h>
#define TGT_MENU_PROTOS
#include "tgt_menu.h"
#define max(a,b) ((a) > (b) ? (a) : (b))

struct tgt_menuitm
{
    struct tgt_menuitm *next;
    char *name;
    short id,flags;
    struct tgt_menuhdr *submenu;
};
struct tgt_menuhdr
{
    struct tgt_menuitm *next;
    struct tgt_menuhdr *parent;
    short xlm,count;
    int current;
    short flags;
};

struct tgt_object * tgt_findmparent(struct tgt_object *obj)
{
    for(;obj->ln.parent!=NULL;obj=obj->ln.parent);
    return(obj);
}

struct tgt_menuhdr * tgt_menu_context(int f)
{
    struct tgt_menuhdr *ret;
    ret=(struct tgt_menuhdr*) malloc(sizeof(struct tgt_menuhdr));
    ret->next=NULL;
    ret->count=0;ret->xlm=0;ret->current=0; ret->parent=NULL;
    ret->flags=f;
}

void tgt_menu_add(struct tgt_menuhdr * ptr,char *name,short id,short flags,struct tgt_menuhdr *submenu)
{
    struct tgt_menuitm *ret;
    ret=(struct tgt_menuitm*) malloc(sizeof(struct tgt_menuitm));
    ret->next=NULL; ret->id=id; ret->flags=flags; ret->name=name; ret->submenu=submenu;
    if(submenu) submenu->parent=ptr;

    ptr->count++;
    ptr->xlm=max(ptr->xlm,strlen(name));
    for(;ptr->next!=NULL;ptr=(struct tgt_menuhdr*) ptr->next);
    ptr->next=ret;
}

void tgt_menu_insert(short aid,struct tgt_menuhdr * ptr,char *name,short id,short flags,struct tgt_menuhdr *submenu)
{
    struct tgt_menuitm *ret;
    struct tgt_menuitm *mn;
    ret=(struct tgt_menuitm*) malloc(sizeof(struct tgt_menuitm));
    ret->id=id; ret->flags=flags; ret->name=name; ret->submenu=submenu;
    if(submenu) submenu->parent=ptr;

    ptr->xlm=max(ptr->xlm,strlen(name));

    if(ptr->count)
	for(mn=ptr->next;mn->next!=NULL && mn->id!=aid;mn=mn->next);
    else
	mn=(struct tgt_menuitm*) ptr;

    ptr->count++;
    ret->next=mn->next;
    mn->next=ret;
}


void tgt_menu_hdraw(struct tgt_terminal *term,struct tgt_menuhdr *menu,int fg,int bg,int sbg,int x,int y,int axs)
{
    struct tgt_menuitm *itm;    
    struct tgt_menuhdr *mh;
    char *name;
    int mxss,i,n,j,k,sel;
    
    mxss=axs/menu->count;

    tgt_chattr(term,TGT_TA_BOLD,0,0);
    tgt_chattr(term,TGT_TA_FGCOLOR,fg,0);
    tgt_chattr(term,TGT_TA_CURSOR,x,y);

    itm=menu->next; n=menu->count; sel=menu->current;
    for(i=0;i<n;i++,itm=itm->next)
    {

	if(i==sel)
	    tgt_chattr(term,TGT_TA_BGCOLOR,sbg,0);
	else
	    tgt_chattr(term,TGT_TA_BGCOLOR,bg,0);

	name=itm->name;
	for(j=0;j<mxss && name[j]!=0;j++) putchar(name[j]);
	for(;j<mxss;j++) putchar(' ');

    }
    fflush(stdout);
    return;
}


void tgt_menu_draw(struct tgt_terminal *term,struct tgt_menuhdr *menu,int fg,int bg,int ffg,int sbg,int x,int y,int xmax)
{
    struct tgt_menuitm *itm;    
    struct tgt_menuhdr *mh;
    char *name;
    int c,i,py,px,xs,ys,n,k,sel,j,q,l,top,cn,ahf;
    px=0; py=0;
    if(menu->flags & TGT_MENUF_HORIZONTAL)
    {
	tgt_menu_hdraw(term,menu,fg,bg,sbg,x,y,xmax);
	return;
    }
    else
    {
	for(mh=menu->parent;mh!=NULL;mh=mh->parent)
	{
	    if(mh->flags & TGT_MENUF_HORIZONTAL)
	    {
		px+=xmax/mh->count * mh->current;
		py+=1;
	    }
	    else
	    {
		py+=mh->current;
    		px+=mh->xlm/2;
	    }
	}
    }
    mh=menu->parent; ahf=0; if(mh!=NULL) if(mh->flags & TGT_MENUF_HORIZONTAL) ahf=1;
    c=menu->count; sel=menu->current;
    
    /* Size (xs,ys) and position (px,py) calculations */
    xs=menu->xlm+2; if(xs>term->x_size) xs=term->x_size;
    ys=c+2; if(!ahf) py+=y-(ys/2); else py+=y; if(py<0) py=0;
    top=0;
    itm=menu->next;
    n=ys-2;
    if((py+ys)>term->y_size)
    {
	py=term->y_size-ys;
	if(py<=0) { py=1; ys=term->y_size; }
	n=ys-2;
	if(sel>=0 && sel<=menu->count)
	{
	    top=sel/n*n;
	    if((top+n)>menu->count) top=menu->count-n;
	    for(i=0;i<top;i++,itm=itm->next);
	}
    }
    px+=x;
    if((px+xs)>term->x_size) 
    {
	px=term->x_size-xs;
	if(px<=0) { px=1; xs=term->x_size; }
    }    

    tgt_chattr(term,TGT_TA_BOLD,0,0);
    /* Upper border */    
    tgt_chattr(term,TGT_TA_COLORS,ffg,bg);
    tgt_chattr(term,TGT_TA_CURSOR,px,py);
    tgt_int_upperb(term,xs);
    q=xs-2; 

    for(i=0;i<n;i++,itm=itm->next,top++)
    {
	tgt_chattr(term,TGT_TA_CURSOR,px,py+1+i);
	/* Horiz border part*/
	tgt_chattr(term,TGT_TA_FGCOLOR,ffg,0);
	tgt_chattr(term,TGT_TA_GFX,0,0);
	putchar(term->gfx_set[TGT_TC_VL]);
	tgt_chattr(term,TGT_TA_TXT,0,0);
	/* Done */
	
	if(top==sel)
	    tgt_chattr(term,TGT_TA_COLORS,fg,sbg);
	else
	    tgt_chattr(term,TGT_TA_COLORS,fg,bg);

	/* The position name itself */
	name=itm->name;
	l=strlen(name);
	for(j=0;j<l;j++) putchar(name[j]);
	for(;j<q;j++) putchar(' ');

	/* Horizontal border part */
	tgt_chattr(term,TGT_TA_COLORS,ffg,bg);
	tgt_chattr(term,TGT_TA_GFX,0,0);
	putchar(term->gfx_set[TGT_TC_VL]);
    }
    /* Lower border */
    tgt_chattr(term,TGT_TA_CURSOR,px,py+c+1);
    tgt_int_lowerb(term,xs);
    fflush(stdout);
    return;
}


void tgt_menu_chp(struct tgt_menuhdr * mn,int p)
{
    int n,i,j,c;
    struct tgt_menuitm *itm;
    c=mn->count;
    for(j=0;j<c;j++)
    {
	n=mn->current+p;
	if(n<0) n=mn->count-1;
	if(n>=mn->count) n=0;
	mn->current=n;
	for(i=0,itm=mn->next;itm!=NULL && i<n;i++,itm=itm->next);
	if(itm) if(!(itm->flags && TGT_MENUF_NONSELECTABLE)) break;
    }
}

int tgt_menu_enter(struct tgt_menuhdr **ptr)
{
    int i,n;
    struct tgt_menuitm *mi;
    n=(*ptr)->current;
    for(i=0,mi=(*ptr)->next;i<n && mi!=NULL;i++,mi=mi->next);
    if(mi)
    {
	if(mi->submenu)
	{
	    (*ptr)=mi->submenu;
	    return(-1);
	}
	else
	    return(mi->id);    
    }
    else
	return(-1);
}

struct tgt_menuhdr * tgt_menu_parent(struct tgt_menuhdr *mh)
{
    if(mh->parent)
	return(mh->parent);
    else
	return(mh);
}

void tgt_menu_destroycontext(struct tgt_menuhdr *menu)
{
    struct tgt_menuitm *itm;
    struct tgt_menuitm *newitm;
    for(itm=menu->next;itm!=NULL;) 
    {
	newitm=itm->next;
	if(itm->submenu) 
	    tgt_menu_destroycontext(itm->submenu);
	else
	    free(itm);
	itm=newitm;
    }	
    free(menu);
}


void tgt_menu_delete(struct tgt_menuhdr *menu,short id)
{
    struct tgt_menuitm *itm;
    struct tgt_menuitm *oitm;
    for(itm=menu->next,oitm=(struct tgt_menuitm*) menu;itm!=NULL;itm=itm->next) 
    {
	if(itm->id==id) break;
	oitm=itm;
    }
    if(itm)
    {
	if(itm->submenu) tgt_menu_destroycontext(itm->submenu);
	oitm->next=itm->next;
	free(itm);
	menu->count--;
    }
}

void tgt_menu_setactive(struct tgt_menuhdr *menu,int num)
{
    if(num>=0 && num<menu->count) menu->current=num;
}
int tgt_menu_getmaxsize(struct tgt_menuhdr *menu)
{
    return(menu->xlm);
}
int tgt_menu_ishorizontal(struct tgt_menuhdr *menu)
{
    return((menu->flags & TGT_MENUF_HORIZONTAL) ? 1 : 0);
}
