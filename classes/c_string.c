#include <stdio.h>
#include <string.h>
#include "tgt.h"

struct tgt_int_string
{
    char *buffer;
    int maxsize;
    int cursor;
    int curpos;
    unsigned char insmode;
    unsigned char passwd;
    unsigned char ccolor;
    char echar;
    char *accept;
};

    static char emptych='_';

int tgt_builtin_string(struct tgt_object *obj,int type,int a,void *b)
{
    int i,act,n,k,t;
    struct tgt_int_string *iw;
    char *rd;
    
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    iw=(struct tgt_int_string*) malloc(sizeof(struct tgt_int_string));
	    obj->objectf=(int(*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    iw->maxsize=tgt_getnumtag(b,TGTT_STRING_MAX,20);
	    iw->passwd=(unsigned char) tgt_getnumtag(b,TGTT_STRING_HASHED,0);
	    iw->ccolor=(unsigned char) tgt_getnumtag(b,TGTT_STRING_CURSORCOLOR,3);
	    iw->echar=(char) tgt_getnumtag(b,TGTT_STRING_EMPTYCHAR,emptych);
	    iw->accept=(char*) tgt_getptrtag(b,TGTT_STRING_ACCEPT,NULL);
	    iw->curpos=0; iw->cursor=0; iw->insmode=1;
	    iw->buffer=(char*) malloc(iw->maxsize+1);
	    bzero(iw->buffer,iw->maxsize+1);
	    rd=(char*) tgt_getptrtag(b,TGTT_STRING_STRING,NULL);
	    if(rd) snprintf(iw->buffer,iw->maxsize,"%s",rd);
	    obj->class_data=iw;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    iw=obj->class_data;
	    free(iw->buffer);
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
		case TGTT_STRING_MAX: iw->maxsize=(int) b; iw->buffer=(char*) realloc(iw->buffer,iw->maxsize); return(1);
		case TGTT_STRING_HASHED: iw->passwd=(unsigned char) b; return(1);
		case TGTT_STRING_CURSORCOLOR: iw->ccolor=(unsigned char) b; return(1);
		case TGTT_STRING_EMPTYCHAR: iw->echar=(char) b; return(1);
		case TGTT_STRING_ACCEPT: iw->accept=(char*) b; return(1);
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
