#include <stdio.h>
#include <stdlib.h>
#include "tgt.h"

void tgt_wint_drawtitleb(tgt_cell * buff,int a,unsigned char *b,tgt_cell attr,int titlecolor)
{
    int i,n;
    tgt_cell p,tattr;
    int tc;
    /* Rysuje 'belke tytulowa' okna */
    
    *(buff++)=TGT_T_FCHAR(attr,TGT_TC_UL);
    *(buff++)=TGT_T_FCHAR(attr,TGT_TC_TS);
    *(buff++)=TGT_T_FCHAR(attr,' ');

    tattr=TGT_T_FG(attr,titlecolor); tc=strlen(b);
    for(i=0;i<tc;i++) *(buff++)=TGT_T_FCHAR(tattr,*(b++));

    *(buff++)=TGT_T_FCHAR(attr,' ');
    *(buff++)=TGT_T_FCHAR(attr,TGT_TC_TE);

    p=TGT_T_FCHAR(attr,TGT_TC_HL);
    for(i=0,n=a-6-tc;i<n;i++) *(buff++)=p;
    /* cala reszta oprocz 2 znakow na narozniki + 2 na 'T' + 2 na odstepy i dlugosci
    stringa tytulowego to poziome kreski */
    *(buff++)=TGT_T_FCHAR(attr,TGT_TC_UR);
    return;
}
void tgt_wint_drawsideb(tgt_cell * buffer,int xs,int ys,tgt_cell attr)
{
    register int i,j;
    tgt_cell p=TGT_T_FCHAR(attr,' ');
    
    xs-=2;
    for(i=0;i<ys;i++)
    {
	*(buffer++)=TGT_T_FCHAR(attr,TGT_TC_VL);
	for(j=0;j<xs;j++) *(buffer++)=p;
	*(buffer++)=TGT_T_FCHAR(attr,TGT_TC_VL);
    }
}

    static char defaulttitle[]="Window";
    int wswitches_next[]={'\t',TGT_KEY_DOWN,TGT_KEY_RIGHT,0};
    int wswitches_prev[]={TGT_KEY_NONE,TGT_KEY_UP,TGT_KEY_LEFT,0};

struct tgt_int_window
{
    int borderfg;
    int titlefg;
    unsigned char *title;
};

    static char defb=6,deftit=7;

int tgt_builtin_window(struct tgt_object *obj,int type,int a,void *b)
{
    int n;
    int act,attr;
    struct tgt_int_window *iw;
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    iw=(struct tgt_int_window*) malloc(sizeof(struct tgt_int_window));
	    obj->class_data=iw;
	    obj->objectf=(int(*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    iw->title=(char*) tgt_getptrtag(b,TGTT_WINDOW_TITLE,defaulttitle);
	    iw->borderfg=tgt_getnumtag(b,TGTT_WINDOW_BORDERCOLOR,defb);
	    iw->titlefg=tgt_getnumtag(b,TGTT_WINDOW_TITLECOLOR,deftit);
            if(!(obj->next_keys)) obj->next_keys=wswitches_next;
	    if(!(obj->prev_keys)) obj->prev_keys=wswitches_prev;		
	    return(1);
	case TGT_OBJECT_DESTROY:
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    act=tgt_isactive(obj);
	    iw=obj->class_data;
	    attr=TGT_T_BUILDCELL(iw->borderfg,obj->bg,(act ? 1 : 0),0,0);
	    tgt_wint_drawtitleb(obj->visual_buffer,obj->xs,iw->title,attr,iw->titlefg);
	    tgt_wint_drawsideb(obj->visual_buffer+obj->xs,obj->xs,obj->ys-2,attr);
	    tgt_int_lowerb(obj->visual_buffer+(obj->xs*(obj->ys-1)),obj->xs,attr);
	    return(1);
	case TGT_OBJECT_HANDLE:
	    iw=obj->class_data;
	    n=tgt_shalliswitch(obj,a,0);
	    if(n<0) { tgt_activateprev(obj); return(1); }
	    if(n>0) { tgt_activatenext(obj); return(1); }
// in future eventually window moving (for instance) functions by keys
// like alt + -> 

/* Here comes the Future ... */
	    switch(a)
	    {
		case TGT_KEY_MOUSEUP:
		case TGT_KEY_MOUSEDOWN:
		    return(1);
		case 'd':
		case TGT_WINDOW_KEY_RIGHT:
		    if(obj->ln.parent)
		    {
	    	        if( (obj->x+obj->xs) < (obj->ln.parent->xs) ) obj->x++;
			tgt_single_refresh(obj->ln.parent);
		    }
		    return(1);
		case 'a':
		case TGT_WINDOW_KEY_LEFT:
		    if(obj->ln.parent)
		    {
			if( obj->x > 0 ) obj->x--;
			tgt_single_refresh(obj->ln.parent);
		    }
		    return(1);
		case 'x':
		case TGT_WINDOW_KEY_DOWN:
		    if(obj->ln.parent)
		    {
			if( (obj->y+obj->ys) < (obj->ln.parent->ys) ) obj->y++;
			tgt_single_refresh(obj->ln.parent);
		    }
		    return(1);
		case 'w':
		case TGT_WINDOW_KEY_UP:
		    if(obj->ln.parent)
		    {
			if( obj->y > 0) obj->y--;
			tgt_single_refresh(obj->ln.parent);
		    }
		    return(1);
		default:
		    if(obj->objectf) return(obj->objectf(obj,a));
		    return(0);
	    }
	    return(0);
	case TGT_OBJECT_SETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_WINDOW_TITLE:	iw->title=(char*) b; return(1);
		case TGTT_WINDOW_BORDERCOLOR: iw->borderfg=(int) b; return(1);
		case TGTT_WINDOW_TITLECOLOR: iw->titlefg=(int) b; return(1);
	    }
	    return(0);
	case TGT_OBJECT_SETDEFAULTS:
	    defb=atoi(tgt_getprefs(b,"window","framecolor","6"));
	    deftit=atoi(tgt_getprefs(b,"window","titlecolor","7"));
	    return(1);
	case TGT_OBJECT_MOUSEDRAG:
	    if(obj->ln.parent)
	    {
		obj->x += a;
		obj->y += (int) b;
		if(obj->x < 0) obj->x = 0;
		if(obj->y < 0) obj->y = 0;
		if((obj->x + obj->xs) > obj->ln.parent->xs) obj->x = obj->ln.parent->xs - obj->xs;
		if((obj->y + obj->ys) > obj->ln.parent->ys) obj->y = obj->ln.parent->ys - obj->ys;
		tgt_single_refresh(obj->ln.parent);
	    }
	    return(1);
	default: return(0);
    }
}
