#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tgt.h"

int isprint(int c);


struct tgt_int_string
{
    unsigned char *buffer;
    int maxsize;
    int cursor;
    int curpos;
    unsigned char insmode;
    unsigned char passwd;
    unsigned char ccolor;
    char echar;
    unsigned char *accept;
};

    extern int tgt_preferred_x,tgt_preferred_y;
    static char emptych='_';

int tgt_builtin_string(struct tgt_object *obj,int type,int a,void *b)
{
    int i,act,n,k,t;
    struct tgt_int_string *iw;
    char *rd;
    tgt_cell attr;
    tgt_cell * buff;
    
    switch(type)
    {
	case TGT_OBJECT_GETSIZES:
	    ((struct tgt_ac_objectinfo*) b)->xsize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_XS,20,((struct tgt_ac_objectinfo*) b)->term);
	    ((struct tgt_ac_objectinfo*) b)->ysize=1;
	    ((struct tgt_ac_objectinfo*) b)->sizeflags=TGT_AC_SF_YFIXED;
	    return(1);
	case TGT_OBJECT_CREATE:
	    iw=(struct tgt_int_string*) malloc(sizeof(struct tgt_int_string));
	    obj->objectf=(int(*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    iw->maxsize=tgt_getnumtag(b,TGTT_STRING_MAX,obj->xs);
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
	    attr=(act=tgt_hasfocus(obj)) ?
		TGT_T_BUILDCELL(obj->fg,obj->bg,1,0,0) :
		TGT_T_BUILDCELL(obj->fg,obj->bg,0,0,0);
	    
	    buff=obj->visual_buffer;
	    
	    if(act==1) 
	    {	
		n=strlen(iw->buffer); k=obj->xs+iw->curpos;
		for(i=iw->curpos;i<k;i++)
		{
		    if(i==iw->cursor)
			attr=TGT_T_FG(attr,iw->ccolor);
		    else
			attr=TGT_T_FG(attr,obj->fg);
		    
		    if(i<n)
		    {
			if(iw->passwd)
			    *(buff++)=TGT_T_FCHAR(attr,iw->passwd);
			else
			    *(buff++)=TGT_T_FCHAR(attr,iw->buffer[i]);
		    }
		    else
			*(buff++)=TGT_T_FCHAR(attr,iw->echar);
		}
		t=iw->cursor-iw->curpos;
		if(t>=0 && t <=obj->xs) 
		{
		    tgt_preferred_x=obj->x+a+t;
		    tgt_preferred_y=(obj->y+(int) b);
		}
	    }
	    else
	    {
		n=strlen(iw->buffer); k=obj->xs;
		for(i=0;i<k;i++)
		{
		    if(i<n)
			if(iw->passwd) 
			    *(buff++)=TGT_T_FCHAR(attr,iw->passwd);
			else
			    *(buff++)=TGT_T_FCHAR(attr,iw->buffer[i]);
		    else
			*(buff++)=TGT_T_FCHAR(attr,iw->echar);
		}

	    }
	    return(1);
	case TGT_OBJECT_HANDLE:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGT_KEY_MOUSEDOWN:
		    iw->cursor=tgt_mouse_get_x() + iw->curpos;
		    if(iw->cursor>strlen(iw->buffer)) iw->cursor=strlen(iw->buffer);
		    tgt_refresh(obj);
		    return(1);
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
			if(iw->accept) 
			{
			    if(index(iw->accept,a)==NULL) return(0);
			}
			else
			    if(a < 0x20) return(0);
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
		    snprintf(iw->buffer,iw->maxsize,"%s",(char*) b);
		    iw->cursor=0;
		    iw->curpos=0;
		    tgt_refresh(obj);
		    return(1);
		case TGTT_STRING_MAX: iw->maxsize=(int) b; iw->buffer=(char*) realloc(iw->buffer,iw->maxsize); return(1);
		case TGTT_STRING_HASHED: iw->passwd=(unsigned char)((int) b); return(1);
		case TGTT_STRING_CURSORCOLOR: iw->ccolor=(unsigned char)((int) b); return(1);
		case TGTT_STRING_EMPTYCHAR: iw->echar=(char)((int) b); return(1);
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
