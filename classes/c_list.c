#include <stdio.h>
#include <string.h>
#include "tgt.h"

struct tgt_int_list
{
    int current;
    int top;
    int realys;
    void *data;
    int (*dataf)(void*,int,int,void*,int,int);
    char *outbuffer;
    int framecolor;
    int activebg;
    int activefg;
};

int tgt_int_listdataf(char **data,int c,int n,char *buffer,int k,int a)
{
    int i,l;
    if(c==TGT_LISTREQ_GETMAX)
    {
	for(i=0;data[i]!=NULL;i++);
	return(i);
    }
    else
    {
	snprintf(buffer,k,"%s",data[n]);
	l=strlen(data[n]); if(k<l) l=k;
	for(i=0;i<l;i++) buffer[i]=data[n][i];
	for(i=l;i<k;i++) buffer[i]=' ';
	buffer[k]=0;
    }
}

    static int defactbg=6;
int tgt_builtin_list(struct tgt_object *obj,int type,int a,void *b)
{
    int i,act,n,k,t,yp,xp,maxx,j;
    struct tgt_int_list *iw;
    char *rd;
    
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    iw=(struct tgt_int_list*) malloc(sizeof(struct tgt_int_list));
	    obj->objectf=(int(*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    obj->class_data=(void*) iw;
	    iw->current=0; iw->top=0;
	    iw->data=(void*) tgt_getptrtag(b,TGTT_LIST_ITEMS,NULL);
	    iw->dataf=(int(*)(void*,int,int,void*,int,int)) tgt_getptrtag(b,TGTT_LIST_DATACALLBACK,(long) tgt_int_listdataf);
	    if(iw->framecolor=tgt_getnumtag(b,TGTT_LIST_FRAMECOLOR,0))
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
	    tgt_chattr(obj->term,TGT_TA_BGCOLOR,obj->bg,0);
	    if(act==1) tgt_chattr(obj->term,TGT_TA_BOLD,0,0);
	    
	    tgt_chattr(obj->term,TGT_TA_FGCOLOR,obj->fg);
	    if(iw->framecolor) 
	    {
		tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a,obj->y+(int) b);
		tgt_chattr(obj->term,TGT_TA_FGCOLOR,iw->framecolor,0);
		tgt_int_upperb(obj->term,obj->xs);
		yp=(int) b+obj->y+1;
		maxx=obj->xs-2;
	    }
	    else
	    {
		yp=(int) b+obj->y;
		maxx=obj->xs;
	    }
	    t=iw->realys;
	    n=iw->dataf(iw->data,TGT_LISTREQ_GETMAX,0,NULL,0,0);
	    xp=a+obj->x; k=iw->top;
	    for(i=0;i<t;i++,k++)
	    {
		tgt_chattr(obj->term,TGT_TA_CURSOR,xp,yp++);
		if(iw->framecolor) { tgt_chattr(obj->term,TGT_TA_COLORS,iw->framecolor,obj->bg);tgt_chattr(obj->term,TGT_TA_GFX,0,0); putchar(obj->term->gfx_set[TGT_TC_VL]); tgt_chattr(obj->term,TGT_TA_TXT,0,0); }
		if(k<n)
		{
		    if(k==iw->current)
		    {
			tgt_chattr(obj->term,TGT_TA_COLORS,iw->activefg,iw->activebg);
			iw->dataf(iw->data,TGT_LISTREQ_GETITEM,k,iw->outbuffer,maxx,1);
		    }
		    else
		    {
		        tgt_chattr(obj->term,TGT_TA_COLORS,obj->fg,obj->bg);
			iw->dataf(iw->data,TGT_LISTREQ_GETITEM,k,iw->outbuffer,maxx,0);
		    }
		    printf("%s",iw->outbuffer);
		}
		else
		{
		    tgt_chattr(obj->term,TGT_TA_BGCOLOR,obj->bg);
		    for(j=0;j<maxx;j++) putchar(' ');
		}
		if(iw->framecolor) { tgt_chattr(obj->term,TGT_TA_COLORS,iw->framecolor,obj->bg);tgt_chattr(obj->term,TGT_TA_GFX,0,0); putchar(obj->term->gfx_set[TGT_TC_VL]); tgt_chattr(obj->term,TGT_TA_TXT,0,0); }
	    }
	    if(iw->framecolor) 
	    {
		tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a,obj->y+(int) b+obj->ys-1);
		tgt_chattr(obj->term,TGT_TA_COLORS,iw->framecolor,obj->bg);
		tgt_int_lowerb(obj->term,obj->xs);
	    }

	    tgt_chattr(obj->term,TGT_TA_NORMAL,0,0);
	    fflush(stdout);
	    return(1);
	case TGT_OBJECT_HANDLE:
	    iw=obj->class_data;
	    switch(a)
	    {
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
		    return(1);
		case TGT_KEY_UP: 
		    iw->current--;
		    if(iw->current<0) iw->current=0;
		    if(iw->current<iw->top) iw->top=iw->current;
		    if(iw->top<0) iw->top=0;
		    tgt_refresh(obj);
		    return(1);
		case TGT_KEY_HOME:
		    iw->current=0;
		    iw->top=0;
		    tgt_refresh(obj);
		    return(1);
		case TGT_KEY_END: 
		    iw->current=iw->dataf(iw->data,TGT_LISTREQ_GETMAX,0,NULL,0,0)-1;
		    iw->top=iw->current-iw->realys+1;
		    if(iw->top<0) iw->top=0;
		    tgt_refresh(obj);
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
		case TGTT_LIST_SELECTED: *((int*) b)=iw->current; return(1);
	    }
	    return(0);
	case TGT_OBJECT_SETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
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
		    if(iw->framecolor=(int) b)
			iw->realys=obj->ys-2;
		    else
			iw->realys=obj->ys;
		    return(1);
		case TGTT_LIST_ACTIVEBG: iw->activebg=(int) b; return(1);
		case TGTT_LIST_ACTIVEFG: iw->activefg=(int) b; return(1);
	    }
	case TGT_OBJECT_SETDEFAULTS:
	    defactbg=atoi(tgt_getprefs(b,"list","activebg","6"));    
	    return(1);
	default: return(0);
    }
}
