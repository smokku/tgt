#include "tgt.h"
#include <string.h>
#include <setjmp.h>
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) > (b) ? (b) : (a))
#endif

extern int async_running,async_semaphore;
jmp_buf jb;

int util_req_btc(struct tgt_object *obj)
{
    longjmp(jb,obj->id+1);
}

int tgt_easyrequest(struct tgt_object *ref,char *title,char *body,char *buttons)
{
    int xs,ys,xc,yc;
    char **linepointers;
    char *str,*stra;
    int k,i,ml,mll,len,b;
    struct tgt_object *req;
    struct tgt_object *bt;
    int id;
    
    linepointers=(char **) malloc(sizeof(char*)*ref->ys);

    for(k=0,ml=0,str=body=strdup(body);;k++)
    {
	if(k>(ref->ys-5)) break;
	linepointers[k]=str;
	if(!(str=index(str,'\n'))) { ml=max(ml,strlen(linepointers[k])); break; }
	*str=0; ml=max(ml,strlen(linepointers[k])); str++;
	if(*str==0) break;
    }
    k+=1;
    xs=min(max(max(strlen(title)+6,ml+4),strlen(buttons)+4),ref->xs); if(xs>ref->xs) return(-1);
    ys=k+5; if(ys>ref->ys) return(-1);
    xc=(ref->xs-xs)/2; yc=(ref->ys-ys)/2;

    req=tgt_createobject(ref->term,TGT_CLASS_WINDOW,NULL);
    tgt_set(req,TGTT_X,xc); tgt_set(req,TGTT_Y,yc);
    tgt_set(req,TGTT_XS,xs); tgt_set(req,TGTT_YS,ys);
    tgt_set(req,TGTT_WINDOW_TITLE,title);
    
    buttons=strdup(buttons);
    for(b=1,str=buttons;str=index(str+1,'|');b++);   
    xc=2;
    for(str=buttons,i=0;i<b;i++)
    {
	if(stra=index(str,'|')) *stra=0;
	bt=tgt_createandlink(req,ref->term,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_CALLBACK,util_req_btc,TGTT_END,0});
	tgt_set(bt,TGTT_X,xc); xc+=strlen(str)+1;
	tgt_set(bt,TGTT_Y,ys-3);
	tgt_set(bt,TGTT_BUTTON_CAPTION,str);
	tgt_set(bt,TGTT_ID,i);
	if(stra) str=stra+1; else break;
    }
    for(i=0;i<k;i++)
    {
	bt=tgt_createandlink(req,ref->term,TGT_CLASS_LABEL,(tagitem[]) {TGTT_X,2,TGTT_END,0});
	tgt_set(bt,TGTT_Y,2+i);
	tgt_set(bt,TGTT_XS,xs-4);
	tgt_set(bt,TGTT_LABEL_TEXT,linepointers[i]);
    }

    
    if(id=setjmp(jb))
    {
	tgt_unlink(req);
	tgt_destroyobject(req);
	free(body); free(linepointers); free(buttons);
	async_semaphore=1;
	return(id-1);
    }
    
    tgt_link(req,ref);
    tgt_refresh(req);
    
    if(async_running) async_semaphore=0;
    while(1) tgt_getkey(req);
}
