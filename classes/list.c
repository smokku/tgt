#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tgt.h"

struct tgt_int_list
{
    int current;
    int top;
    int realys;
    void *data;
    int (*dataf)(void*,int,int,void*,int,int);
    int (*chf)(struct tgt_object*,int);
    char *outbuffer;
    int framecolor;
    int activebg;
    int activefg;
};

extern int tgt_preferred_x,tgt_preferred_y;

int tgt_int_listdataf(char **data,int c,int n,char *buffer,int k,int a)
{
    int i,l;
    if(c==TGT_LISTREQ_GETMAX)
    {
	if(!data) return(0);
	for(i = 0;data[i]!=NULL;i++);
	return(i);
    }
    else
    {
	if(!data)
	{
	    l = 0;
	}
	else
	{    
	    snprintf(buffer,k,"%s",data[n]);
	    l=strlen(data[n]); if(k<l) l=k;
	    for(i=0;i<l;i++) buffer[i]=data[n][i];
	}
	for(i=l;i<k;i++) buffer[i]=' ';
	buffer[k]=0;
    }
    return(1);
}

    static int defactbg=6;
int tgt_builtin_list(struct tgt_object *obj,int type,int a,void *b)
{
    int i,act,n,k,t,xp,maxx;
    struct tgt_int_list *iw;
    tgt_cell attr;
    tgt_cell *buff;
    switch(type)
    {
	case TGT_OBJECT_GETSIZES:
	    if(tgt_getnumtag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_LIST_FRAMECOLOR,-1) != -1)
		n=4;
	    else
		n=2;
            ((struct tgt_ac_objectinfo*) b)->xsize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_XS,n);
            ((struct tgt_ac_objectinfo*) b)->ysize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_YS,n);
	    return(1);
					    
	case TGT_OBJECT_CREATE:
	    iw=(struct tgt_int_list*) malloc(sizeof(struct tgt_int_list));
	    obj->objectf=(int(*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    obj->class_data=(void*) iw;
	    iw->current=tgt_getnumtag(b,TGTT_LIST_SELECTED,0);
	    iw->top=0;
	    iw->data=(void*) tgt_getptrtag(b,TGTT_LIST_ITEMS,NULL);
	    iw->dataf=(int(*)(void*,int,int,void*,int,int)) tgt_getptrtag(b,TGTT_LIST_DATACALLBACK,(long) tgt_int_listdataf);
	    iw->chf=(int(*)(struct tgt_object*,int)) tgt_getptrtag(b,TGTT_LIST_CHANGECALLBACK,0);
	    if((iw->framecolor=tgt_getnumtag(b,TGTT_LIST_FRAMECOLOR,-1))!=-1)
		iw->realys=obj->ys-2;
	    else
		iw->realys=obj->ys;
		
	    iw->activebg=tgt_getnumtag(b,TGTT_LIST_ACTIVEBG,defactbg);
	    iw->activefg=tgt_getnumtag(b,TGTT_LIST_ACTIVEFG,obj->fg);

	    iw->outbuffer=(char*) malloc(obj->xs+1);
	    return(1);
	case TGT_OBJECT_DESTROY:
	    iw=obj->class_data;
	    free(iw->outbuffer);
	    free(iw);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    iw=obj->class_data;
	    act=tgt_hasfocus(obj);
	    attr=act?
		TGT_T_BUILDCELL(obj->fg,obj->bg,1,0,0) :
		TGT_T_BUILDCELL(obj->fg,obj->bg,0,0,0);
	    
	    buff=obj->visual_buffer;
	    if(iw->framecolor!=-1) 
	    {
		tgt_int_upperb(buff,obj->xs,TGT_T_FG(attr,iw->framecolor));
		buff+=obj->xs;
		maxx=obj->xs-2;
	    }
	    else
		maxx=obj->xs;

	    t=iw->realys;
	    n=iw->dataf(iw->data,TGT_LISTREQ_GETMAX,0,NULL,0,0);
	    xp=a+obj->x; k=iw->top;
	    for(i=0;i<t;i++,k++)
	    {
		if(iw->framecolor!=-1) 
		    *(buff++)=TGT_T_FG(TGT_T_CHAR(attr,TGT_TC_VL),iw->framecolor);
		if(k<n)
		{
		    if(k==iw->current)
		    {
			if(act) 
			{
			    if(iw->framecolor!=-1)
			    {
			        tgt_preferred_x=a+obj->x+1;
			        tgt_preferred_y=(int) b + obj->y+i+1; 
			    }
			    else
			    {
			        tgt_preferred_x=a+obj->x;
			        tgt_preferred_y=(int) b + obj->y+i; 
			    }
			}			    
			iw->dataf(iw->data,TGT_LISTREQ_GETITEM,k,iw->outbuffer,maxx,1);
			tgt_flprintf(buff,maxx,TGT_T_FG(TGT_T_BG(attr,iw->activebg),iw->activefg),"%s",iw->outbuffer);
		    }
		    else
		    {
			iw->dataf(iw->data,TGT_LISTREQ_GETITEM,k,iw->outbuffer,maxx,0);
			tgt_flprintf(buff,maxx,attr,"%s",iw->outbuffer);
		    }
		    buff+=maxx;
		}
		else
		{
		    tgt_flprintf(buff,maxx,attr,"");
		    buff+=maxx;
		}

		if(iw->framecolor!=-1) 
		    *(buff++)=TGT_T_FG(TGT_T_CHAR(attr,TGT_TC_VL),iw->framecolor);
	    }
	    if(iw->framecolor!=-1) 
		tgt_int_lowerb(buff,obj->xs,TGT_T_FG(attr,iw->framecolor));
	    return(1);
	case TGT_OBJECT_HANDLE:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGT_KEY_MOUSEDOWN:
		    k=tgt_mouse_get_y();
		    if(iw->framecolor!=-1)
		    {
			if(k >= (obj->ys-1)) k=obj->ys-2;
			if(k < 1) k = 1;
			k-=1;
		    }
		    iw->current=iw->top+k;
		    n=iw->dataf(iw->data,TGT_LISTREQ_GETMAX,0,NULL,0,0)-1;
		    if(iw->current>n) iw->current=n;
		    if(iw->current<0) iw->current=0;
		    tgt_refresh(obj);
		    if(iw->chf && n>=0) iw->chf(obj,iw->current);
		    return(1);
		case TGT_KEY_PGDN:
		    iw->current+=iw->realys-1;
		case TGT_KEY_DOWN: 
		    iw->current++;
		    n=iw->dataf(iw->data,TGT_LISTREQ_GETMAX,0,NULL,0,0)-1;
		    if(iw->current>n) iw->current=n;
		    if(iw->current-iw->top >= iw->realys)
		    {
			iw->top=iw->current-iw->realys+1;
			if(iw->top<0) iw->top=0;
		    }
		    tgt_refresh(obj);
		    if(iw->chf && (n>=0)) iw->chf(obj,iw->current);
		    return(1);
		case TGT_KEY_PGUP:
		    iw->current-=iw->realys-1;
		case TGT_KEY_UP: 
		    iw->current--;
		    n=iw->dataf(iw->data,TGT_LISTREQ_GETMAX,0,NULL,0,0);
		    if(iw->current<0) iw->current=0;
		    if(iw->current<iw->top) iw->top=iw->current;
		    if(iw->top<0) iw->top=0;
		    tgt_refresh(obj);
		    if(iw->chf && (n>0)) iw->chf(obj,iw->current);
		    return(1);
		case TGT_KEY_HOME:
		    iw->current=0;
		    iw->top=0;
		    tgt_refresh(obj);
		    n=iw->dataf(iw->data,TGT_LISTREQ_GETMAX,0,NULL,0,0);
		    if(iw->chf && (n>0)) iw->chf(obj,iw->current);
		    return(1);
		case TGT_KEY_END: 
		    iw->current=iw->dataf(iw->data,TGT_LISTREQ_GETMAX,0,NULL,0,0)-1;
		    iw->top=iw->current-iw->realys+1;
		    if(iw->top<0) iw->top=0;
		    tgt_refresh(obj);
		    if(iw->chf && (n>=0)) iw->chf(obj,iw->current);
		    return(1);
		case 13: case 10:
		    if(obj->objectf) obj->objectf(obj,iw->current);
		    return(1);
		default:
		    n=tgt_shalliswitch(obj,a,0);
		    if(n<0) { tgt_activateprev(obj); return(1); }
		    if(n>0) { tgt_activatenext(obj); return(1); }
		    return(0);
	    }
	    return(0);
	case TGT_OBJECT_GETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_LIST_SELECTED: *((tgtt*) b)=(tgtt) iw->current; return(1);
		case TGTT_LIST_ITEMS: *((tgtt*) b)=(tgtt) iw->data; return(1);
		case TGTT_LIST_FRAMECOLOR: *((tgtt*) b)=(tgtt) iw->framecolor; return(1);
		case TGTT_LIST_ACTIVEBG: *((tgtt*) b)=(tgtt) iw->activebg; return(1);
		case TGTT_LIST_ACTIVEFG: *((tgtt*) b)=(tgtt) iw->activefg; return(1);
	    }
	    return(0);
	case TGT_OBJECT_SETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_XS:
		    iw->outbuffer=(char*) realloc(iw->outbuffer, ((int) b) + 1);
		    return(1);
		case TGTT_YS:
		    obj->ys=(int) b;
		    if(iw->framecolor!=-1)
			iw->realys=obj->ys-2;
		    else
			iw->realys=obj->ys;
		    return(1);
		case TGTT_LIST_SELECTED: iw->current=(int) b; return(1);
		case TGTT_LIST_ZERO: 
		    iw->current=0;
		    iw->top=0;
		    return(1);
		case TGTT_LIST_ADJUST:
		    n=iw->dataf(iw->data,TGT_LISTREQ_GETMAX,0,NULL,0,0)-1;
		    if(iw->current>n) iw->current=n;
		    if(iw->current<0) iw->current=0;
		    if(iw->current-iw->top >= iw->realys)
		    {
			iw->top=iw->current-iw->realys+1;
			if(iw->top<0) iw->top=0;
		    }
		    if(iw->current<iw->top) iw->top=iw->current;
		    if(iw->top<0) iw->top=0;
		    return(1);
		case TGTT_LIST_ITEMS: iw->data=b; return(1);
		case TGTT_LIST_FRAMECOLOR:
		    if((iw->framecolor=(int) b)!=-1)
			iw->realys=obj->ys-2;
		    else
			iw->realys=obj->ys;
		    return(1);
		case TGTT_LIST_ACTIVEBG: iw->activebg=(int) b; return(1);
		case TGTT_LIST_ACTIVEFG: iw->activefg=(int) b; return(1);
		case TGTT_LIST_DATACALLBACK: iw->dataf=(int(*)(void*,int,int,void*,int,int)) b; return(1);
		case TGTT_LIST_CHANGECALLBACK: iw->chf=(int(*)(struct tgt_object*,int)) b; return(1);
	    }
	    return(0);
	case TGT_OBJECT_SETDEFAULTS:
	    defactbg=atoi(tgt_getprefs(b,"list","activebg","6"));    
	    return(1);
	case TGT_OBJECT_MOUSEDRAG:
	    if((int) b < 0) tgt_builtin_list(obj,TGT_OBJECT_HANDLE,TGT_KEY_UP,NULL);
	    if((int) b > 0) tgt_builtin_list(obj,TGT_OBJECT_HANDLE,TGT_KEY_DOWN,NULL);
	    return(1);
	default: return(0);
    }
}
