#include <stdio.h>
#include <string.h>
#include "tgt.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <pthread.h>
		     
struct tgt_int_buffer
{
    char **lines;
    int linesize,ysize;
    short sys,iotype;		// 0-wronly for application 1-rd/wr
    int first,last,top;
    int borderc;
};

void tgt_int_chldhandler(int i)
{
    while(wait3(NULL,WNOHANG,(struct rusage * ) 0)>0);
}
    

void tgt_int_buffertask(void **param)
{
    struct tgt_object *obj;
    struct tgt_int_buffer *id;
    char *inbuffer;
    int fh,ibp,mbp;
    char c;

//    signal(SIGCHLD,tgt_int_chldhandler);
//    fprintf(stderr,"Handler starting\n");
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);

    obj=(struct tgt_object*) param[0];
    fh=(int) param[1];
    ibp=0; mbp=obj->xs;
    inbuffer=(char*) malloc(mbp+1);
    id=obj->class_data;
    if(id->borderc!=-1) mbp-=2;
    
    for(;;)
    {
	    if(read(fh,&c,1)!=1) { close(fh); return; }
	    if(c=='\n') 
	    { 
		inbuffer[ibp]=0; ibp=0;
		tgt_builtin_buffer(obj,TGT_OBJECT_SETTAG,TGTA_BUFFER_ADD,inbuffer);
	    }
	    else
	    {
		inbuffer[ibp++]=c;
		if(ibp>=mbp) 
		{
		    inbuffer[ibp]=0; ibp=0;
		    tgt_builtin_buffer(obj,TGT_OBJECT_SETTAG,TGTA_BUFFER_ADD,inbuffer);
		}
	    }	    
    }
}

void * tgt_buffer_system(struct tgt_object *obj,char *name)
{
    long ret;
    int fd,t;
    int b_pipe[2];
    long *ptr;
    static void *param[2];
    if(pipe(b_pipe)==-1) return;
    signal(SIGCHLD,tgt_int_chldhandler);
    t=fork();
    switch(t)
    {
	case 0:
//	    signal(SIGCHLD,tgt_int_chldhandler);
	    fd=b_pipe[1];
	    dup2(fd,1);
//	    printf("Called child process \n");
	    execlp("ping","ping","213.25.115.1",NULL);
	    _exit(0);
	    break;
	case -1: return;
	default:
	    param[0]=obj; param[1]=(void*) b_pipe[0];
	    pthread_create(&ret,NULL,tgt_int_buffertask,param);
	    fprintf(stderr,"ppid %d\n",ret);
	    ptr=(long*) malloc(sizeof(long)*2);
	    ptr[0]=ret; ptr[1]=t;
	    return((void*) ptr);
    }
}
void tgt_buffer_abort(long *ptr)
{
    kill(ptr[1],SIGKILL);
    fprintf(stderr,"kpid %d childpid %d\n",ptr[0],ptr[1]);
    pthread_cancel(ptr[0]);
    perror("pthread_cancel");
    free(ptr);
}

int tgt_builtin_buffer(struct tgt_object *obj,int type,int a,void *b)
{
    struct tgt_int_buffer *idata;
    int l,f,bfs,ls,ys,i,yp,xp,n;
    char *ptr,**index;
    char fstr[20];
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    idata=(struct tgt_int_buffer*) malloc(sizeof(struct tgt_int_buffer));
	    idata->borderc=(int) tgt_getnumtag(b,TGTT_BUFFER_BORDERCOLOR,-1);

	    if(idata->borderc!=-1)
		idata->sys=obj->ys-2;
	    else
		idata->sys=obj->ys;
		
	    ls=tgt_getnumtag(b,TGTT_BUFFER_LINESIZE,obj->xs+1); idata->linesize=ls;
	    bfs=tgt_getnumtag(b,TGTT_BUFFER_BUFFERSIZE,8192);
	    ys=bfs/ls; idata->ysize=ys;
	    ptr=(char*) malloc(bfs);
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
	        tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a,obj->y+(int) b);
	        tgt_chattr(obj->term,TGT_TA_COLORS,idata->borderc,obj->bg);
	        tgt_int_upperb(obj->term,obj->xs);
	        yp=(int) b+1+obj->y; xp=a+obj->x;
		sprintf(fstr,"%%-%ds",obj->xs-2);
		tgt_chattr(obj->term,TGT_TA_GFX,0,0);
		for(i=0;i<ys;i++,f++,yp++)
		{
		    tgt_chattr(obj->term,TGT_TA_CURSOR,xp,yp);
		    tgt_chattr(obj->term,TGT_TA_FGCOLOR,idata->borderc,0);
		    putchar(obj->term->gfx_set[TGT_TC_VL]);
		    tgt_chattr(obj->term,TGT_TA_TXT,0,0);
		    tgt_chattr(obj->term,TGT_TA_FGCOLOR,obj->fg,0);
		    if(f<l) printf(fstr,index[f%ls]); else printf(fstr,"");
		    tgt_chattr(obj->term,TGT_TA_FGCOLOR,idata->borderc,0);
		    tgt_chattr(obj->term,TGT_TA_GFX,0,0);
		    putchar(obj->term->gfx_set[TGT_TC_VL]);
		}
		tgt_chattr(obj->term,TGT_TA_CURSOR,xp,yp);
	        tgt_chattr(obj->term,TGT_TA_COLORS,idata->borderc,obj->bg);
	        tgt_int_lowerb(obj->term,obj->xs);
		
	    }
	    else
	    {
	        tgt_chattr(obj->term,TGT_TA_COLORS,obj->fg,obj->bg);
		yp=(int) b+obj->y; xp=a+obj->x;
		sprintf(fstr,"%%-%ds",obj->xs);
		for(i=0;i<ys;i++,f++,yp++)
		{
		    tgt_chattr(obj->term,TGT_TA_CURSOR,xp,yp);
		    if(f<l) printf(fstr,index[f%ls]); else printf(fstr,"");
		}
	    }
	    fflush(stdout);												
	    return(1);
	case TGT_OBJECT_SETTAG:
	    idata=obj->class_data;
	    if(a==TGTA_BUFFER_ADD)
	    {
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
	default: return(0);
    }
}
