#include <stdio.h>
#include <string.h>
#include "tgt.h"

struct tgt_line
{
    struct tgt_line *next;
    struct tgt_line *prev;
    char *buffer;
    int size;
    int used;
    int cursor;
    int top;
};

struct tgt_int_multiline
{
    struct tgt_line * lines;	// UWAGA pola takie same jak w tgt_line
    struct tgt_line * last;
    struct tgt_line * top;
    struct tgt_line * current;
    char *accept;
    int bufferperiod;
};
/* Procedury operujace na liniach */

static struct tgt_line *create_line(int dbfs)
{
    struct tgt_line *ret;
    ret=(struct tgt_line*) malloc(sizeof(struct tgt_line));
    ret->next=NULL; ret->prev=NULL;
    ret->buffer=(char*) malloc(dbfs);
    ret->size=dbfs; ret->used=0; ret->cursor=0; ret->top=0;
}

static void insert_newline(struct tgt_int_multiline *ld)
{
    struct tgt_line *pred;
    struct tgt_line *newone;
    pred=ld->current;
    if(!pred)
	pred=ld;
    else
	pred=ld->pred;

    newone=create_line(ld->bufferperiod);
    if(!ld->top) ld->top=newone;
    if(!ld->current) ld->current=newone;
    
    if(pred->next) pred->next->pred=newone;

    newone->next=pred->next;
    pred->next=newone;
    newone->prev=pred;
}

static void insert(struct tgt_int_multiline *lined,int c)
{
    struct tgt_line *line;
    int i,k,p;
    char *buffer;
    line=lined->current;
    if(!line) return;
    if((line->used+2) > line->size)
    {
	line->size+=lined->bufferperiod;
	line->buffer=realloc(line->buffer,line->size);
    }
    buffer=line->buffer; k=line->used; p=line->cursor;
    for(i=k;i>=p;i--)
	buffer[i+1]=buffer[i];
    buffer[p++]=c;
    p->line=p;
}

    static char emptych='_';

int tgt_builtin_multiln(struct tgt_object *obj,int type,int a,void *b)
{
    int i,act,n,k,t;
    struct tgt_int_multiline *iw;
    char *rd;
    
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    iw=(struct tgt_int_multiline) malloc(sizeof(struct tgt_int_multiline));
	    obj->objectf=(int(*)()) tgt_gettag(b,TGTT_CALLBACK,(long) NULL);
	    iw->lines=NULL; iw->last=NULL; iw->top=NULL; iw->current=NULL;
	    insert_newline(iw);
	    obj->class_data=iw;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    iw=obj->class_data;
	    free(iw);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    iw=obj->class_data;
	    act=tgt_hasfocus(obj);
	    tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a,obj->y+(int) b);
	    tgt_chattr(obj->term,TGT_TA_BGCOLOR,obj->bg);
	    if(act==1) 
	    {	
		tgt_chattr(obj->term,TGT_TA_BOLD,0,0);
		n=strlen(iw->buffer); k=obj->xs+iw->curpos;
		for(i=iw->curpos;i<k;i++)
		{
		    if(i==iw->cursor)
		        tgt_chattr(obj->term,TGT_TA_FGCOLOR,iw->ccolor);
		    else
			tgt_chattr(obj->term,TGT_TA_FGCOLOR,obj->fg);
		    
		    if(i<n)
			if(iw->passwd) 
			    putchar(iw->passwd);
			else
			    putchar(iw->buffer[i]);
		    else
			putchar(iw->echar);
		}
		t=iw->cursor-iw->curpos;
		if(t>=0 && t <=obj->xs) tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a+t,obj->y+(int) b);
	    }
	    else
	    {
		tgt_chattr(obj->term,TGT_TA_FGCOLOR,obj->fg);
		n=strlen(iw->buffer); k=obj->xs;
		for(i=0;i<k;i++)
		{
		    if(i<n)
			if(iw->passwd) 
			    putchar(iw->passwd);
			else
			    putchar(iw->buffer[i]);
		    else
			putchar(iw->echar);
		}

	    }
	    tgt_chattr(obj->term,TGT_TA_NORMAL,0,0);
	    fflush(stdout);
	    return(1);
	case TGT_OBJECT_HANDLE:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGT_KEY_LEFT: 
		    iw->cursor--;
		    if(iw->cursor<0) iw->cursor=0;
		    if(iw->cursor<iw->curpos) iw->curpos--;
		    tgt_refresh(obj);
		    return(1);
		case TGT_KEY_RIGHT: 
		    iw->cursor++;
		    if(iw->cursor>strlen(iw->buffer)) iw->cursor=strlen(iw->buffer);
		    if(iw->cursor>=(iw->curpos+obj->xs)) iw->curpos++;
		    tgt_refresh(obj);
		    return(1);
		case TGT_KEY_BKSPC:
		    k=iw->cursor; t=strlen(iw->buffer);
		    if(k>0)
		    {
			for(i=k-1;i<t;i++) iw->buffer[i]=iw->buffer[i+1];
			iw->cursor--;
			if(iw->cursor<iw->curpos) iw->curpos--;
			tgt_refresh(obj);
		    }
		    return(1);
		case TGT_KEY_DELETE:
		    k=iw->cursor; t=strlen(iw->buffer);
		    for(i=k;i<t;i++) iw->buffer[i]=iw->buffer[i+1];
		    tgt_refresh(obj);
		    return(1);
		case TGT_KEY_HOME:
		    iw->cursor=0;
		    iw->curpos=0;
		    tgt_refresh(obj);
		    return(1);
		case TGT_KEY_END: 
		    iw->cursor=strlen(iw->buffer);
		    iw->curpos=iw->cursor-obj->xs+1;
		    if(iw->curpos<0) iw->curpos=0;
		    tgt_refresh(obj);
		    return(1);
		case 13: case 10:
		    if(obj->objectf) obj->objectf(obj,0);
		    return(1);
		case TGT_KEY_INSERT: iw->insmode=1-iw->insmode; return(1);
		default:
		    n=tgt_shalliswitch(obj,a,1);
		    if(n<0) { tgt_activateprev(obj); return(1); }
		    if(n>0) { tgt_activatenext(obj); return(1); }
		    if(a>0)
		    {
			if(iw->accept) if(index(iw->accept,a)==NULL) return(0);
			k=iw->cursor;
			if(k<iw->maxsize)
			{
			    t=strlen(iw->buffer);
			    i=t+1;
			    if(i<iw->maxsize)
			    {
				if(iw->insmode) for(;i>k;i--) iw->buffer[i]=iw->buffer[i-1];
				if(iw->buffer[k]==0) iw->buffer[k+1]=0; 
				iw->buffer[k]=a;
				iw->cursor++;
				if(iw->cursor>=(iw->curpos+obj->xs)) iw->curpos++;
				tgt_refresh(obj);
			    }
			}
			return(1);
		    }
		    else
		    {
		    	n=tgt_shalliswitch(obj,a,0);
			if(n<0) { tgt_activateprev(obj); return(1); }
			if(n>0) { tgt_activatenext(obj); return(1); }
			return(0);
		    }

	    }
	    return(0);
	case TGT_OBJECT_SETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_STRING_STRING:
		    snprintf(iw->buffer,iw->maxsize,"%s",b);
		    iw->cursor=0;
		    iw->curpos=0;
		    tgt_refresh(obj);
		    return(1);
	    }
	    return(0);
	case TGT_OBJECT_GETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_STRING_STRING:
		    *((char**)b)=iw->buffer;
		    return(1);
	    }	    
	    return(0);
	case TGT_OBJECT_SETDEFAULTS:
	    emptych=tgt_getprefs(b,"string","empty","_")[0];
	    return(1);
	default: return(0);
    }
}
