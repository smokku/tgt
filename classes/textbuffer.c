#include <stdio.h>
#include <string.h>
#include "tgt.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
		     
struct tgt_int_buffer
{
    char **lines;
    int linesize,ysize;
    short sys,iotype;		// 0-wronly for application 1-rd/wr
    int first,last,top;
    int borderc;
};

int tgt_builtin_buffer(struct tgt_object *obj,int type,int a,void *b)
{
    struct tgt_int_buffer *idata;
    int l,f,bfs,ls,ys,i,yp,n;
    char *ptr,**index;
    switch(type)
    {
	case TGT_OBJECT_GETSIZES:
            if(tgt_getnumtag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_BUFFER_BORDERCOLOR,-1)==-1)
                n=2;
            else
                n=4;
            ((struct tgt_ac_objectinfo*) b)->xsize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_XS,n);
            ((struct tgt_ac_objectinfo*) b)->ysize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_YS,n);
            return(1);
	case TGT_OBJECT_CREATE:
	    idata=(struct tgt_int_buffer*) malloc(sizeof(struct tgt_int_buffer));
	    idata->borderc=(int) tgt_getnumtag(b,TGTT_BUFFER_BORDERCOLOR,-1);

	    if(idata->borderc!=-1)
		idata->sys=obj->ys-2;
	    else
		idata->sys=obj->ys;
	    
	    obj->objectf=(int (*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    ls=tgt_getnumtag(b,TGTT_BUFFER_LINESIZE,obj->xs+1); idata->linesize=ls;
	    bfs=tgt_getnumtag(b,TGTT_BUFFER_BUFFERSIZE,8192);
	    ys=bfs/ls; idata->ysize=ys;
	    ptr=(char*) malloc(bfs);
	    bzero(ptr, bfs);
	    index=(char**) malloc(sizeof(char*) * ys);
	    for(i=0;i<ys;i++,ptr+=ls) index[i]=ptr;
	    idata->lines=index;
	    idata->first=0; idata->last=0; idata->top=0;
	    obj->class_data=idata;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    idata=obj->class_data;
	    free(idata->lines[0]);
	    free(idata->lines);
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    idata=obj->class_data; ys=idata->sys;
	    index=idata->lines; f=idata->top; l=idata->last; ls=idata->ysize;
    
	    if(idata->borderc!=-1)
	    {
		tgt_cell attr=TGT_T_BUILDCELL(obj->fg,obj->bg,tgt_hasfocus(obj)?1:0,0,0);
		tgt_cell *buff=obj->visual_buffer;
		
	        tgt_int_upperb(buff,obj->xs,TGT_T_FG(attr,idata->borderc));
		buff+=obj->xs;
		for(i=0;i<ys;i++,f++,yp++)
		{
		    *(buff++)=TGT_T_FG(TGT_T_FCHAR(attr,TGT_TC_VL),idata->borderc);
		    if(f<l)
			tgt_flprintf(buff,obj->xs-2,attr,"%s",index[f%ls]);
		    else
			tgt_flprintf(buff,obj->xs-2,attr,"");
		    buff+=obj->xs-2;
		    *(buff++)=TGT_T_FG(TGT_T_FCHAR(attr,TGT_TC_VL),idata->borderc);
		}
	        tgt_int_lowerb(buff,obj->xs,TGT_T_FG(attr,idata->borderc));
		
	    }
	    else
	    {
		tgt_cell attr=TGT_T_BUILDCELL(obj->fg,obj->bg,0,0,0);
		tgt_cell *buff=obj->visual_buffer;

		for(i=0;i<ys;i++,f++,yp++)
		{
		    if(f<l)
			tgt_flprintf(buff,obj->xs,attr,"%s",index[f%ls]);
		    else
			tgt_flprintf(buff,obj->xs,attr,"");
		    buff+=obj->xs;
		}
	    }
	    return(1);
	case TGT_OBJECT_SETTAG:
	    idata=obj->class_data;
	    switch(a)
	    {
	        case TGTA_BUFFER_ADD:
		    strncpy(idata->lines[idata->last%idata->ysize],b,idata->linesize);
		    idata->lines[idata->last%idata->ysize][idata->linesize-1]=0;
		    if((idata->top+idata->sys)==idata->last) idata->top++;
		    for(idata->last++;(idata->last-idata->first)>idata->ysize;idata->first++);
		    tgt_refresh(obj);
		    return(1);
	    }
	    return(0);
	case TGT_OBJECT_GETTAG:
	    idata=obj->class_data;
	    return(0);
	case TGT_OBJECT_HANDLE:
	    idata=obj->class_data;
	    switch(a)
	    {
		case TGT_KEY_DOWN: 
		    idata->top++;
		    if(idata->top>(idata->last-idata->sys))
		    {
			idata->top=idata->last-idata->sys;
			if(idata->top<idata->first) idata->top=idata->first;
		    }
		    tgt_refresh(obj);
		    return(1);
		case TGT_KEY_UP: 
		    idata->top--;
		    if(idata->top<idata->first) idata->top=idata->first;
		    tgt_refresh(obj);
		    return(1);
		case TGT_KEY_PGDN: 
		    idata->top+=idata->sys;
		    if(idata->top>(idata->last-idata->sys))
		    {
			idata->top=idata->last-idata->sys;
			if(idata->top<idata->first) idata->top=idata->first;
		    }
		    tgt_refresh(obj);
		    return(1);
		case TGT_KEY_PGUP: 
		    idata->top-=idata->sys;
		    if(idata->top<idata->first) idata->top=idata->first;
		    tgt_refresh(obj);
		    return(1);
		case TGT_KEY_HOME: idata->top=idata->first; tgt_refresh(obj); return(1);
		case TGT_KEY_END: 
		    idata->top=idata->last-idata->sys;
		    if(idata->top<idata->first) idata->top=idata->first;
		    tgt_refresh(obj);
		    return(1);
	    }
	    n=tgt_shalliswitch(obj,a,0);
	    if(n<0) { tgt_activateprev(obj); return(1); }
	    if(n>0) { tgt_activatenext(obj); return(1); }
	    if(obj->objectf) return(obj->objectf(obj,a));
	    return(0);
	case TGT_OBJECT_MOUSEDRAG:
	    if((int) b < 0) tgt_builtin_buffer(obj,TGT_OBJECT_HANDLE,TGT_KEY_UP,NULL);
	    if((int) b > 0) tgt_builtin_buffer(obj,TGT_OBJECT_HANDLE,TGT_KEY_DOWN,NULL);
	    return(1);
	default: return(0);
    }
}
