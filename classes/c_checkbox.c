#include <stdio.h>
#include <stdlib.h>
#include "tgt.h"

struct tgt_int_checkbox
{
    int activebg;
    char *title;
    int active;
    int state;
};

static int d_actbg=6;
static char d_checkedchar='x';
static char d_uncheckedchar=' ';

void tgt_builtin_checkbox_rebuild(struct tgt_object *obj)
{
    struct tgt_int_checkbox * idata=obj->class_data;
    tgt_cell attr=TGT_T_BUILDCELL(obj->fg,obj->bg,0,0,0);
    tgt_flprintf(obj->visual_buffer,obj->xs,attr,"[%c] %s", idata->active? d_checkedchar:d_uncheckedchar, idata->title);
    idata->state=1;
}

int tgt_builtin_checkbox(struct tgt_object *obj,int type,int a,void *b)
{
    int n;
    struct tgt_int_checkbox *idata;
    char *title;
    switch(type)
    {
	case TGT_OBJECT_GETSIZES:
	    return(0);
	case TGT_OBJECT_CREATE:
	    title=(char*) tgt_getptrtag(b,TGTT_CHECKBOX_CAPTION,NULL);
	    idata=(struct tgt_int_checkbox*) malloc(sizeof(struct tgt_int_checkbox));
	    if(title==NULL) title="";
	    idata->title=(char *) title;
	    idata->active=tgt_getnumtag(b,TGTT_CHECKBOX_ACTIVE,0);
	    idata->activebg=tgt_getnumtag(b,TGTT_CHECKBOX_ACTIVEBG,d_actbg);
	    if(tgt_getsizetag(b,TGTT_XS,0,obj->term)==0) obj->xs=strlen(title)+4;
	    obj->objectf=(int (*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    obj->class_data=idata; 
	    return(1);
	case TGT_OBJECT_DESTROY:
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_VISUALINIT:
	    tgt_builtin_checkbox_rebuild(obj);
	    break;
	case TGT_OBJECT_REFRESH:
	    idata=obj->class_data;
	    if(tgt_hasfocus(obj))
	    {
		if(idata->state == 1)
		{
		    tgt_change_colors(obj->visual_buffer,obj->xs,obj->fg,idata->activebg,1);
		    idata->state = 2;
		}
	    }
	    else
	    {
	    	if(idata->state == 2)
		{
		    tgt_change_colors(obj->visual_buffer,obj->xs,obj->fg,obj->bg,0);
		    idata->state = 1;
		}
	    }
	    
	    return(1);
	case TGT_OBJECT_HANDLE:
	    idata=obj->class_data;
	    if(a==13 || a==10 || a==32 || a==TGT_KEY_MOUSEUP) 
	    {
		if(idata->active = ! idata->active)
		    obj->visual_buffer[1]=TGT_T_CHAR(obj->visual_buffer[1],d_checkedchar);
		else
		    obj->visual_buffer[1]=TGT_T_CHAR(obj->visual_buffer[1],d_uncheckedchar);
		tgt_refresh(obj);
		if(obj->objectf!=NULL) obj->objectf(obj,idata->state);
		return(1);
	    }
	    n=tgt_shalliswitch(obj,a,0);
	    if(n<0) { tgt_activateprev(obj); return(1); }
	    if(n>0) { tgt_activatenext(obj); return(1); }
	    return(0);
	case TGT_OBJECT_GETTAG:
	    idata=obj->class_data;
	    switch(a)
	    {
		case TGTT_CHECKBOX_ACTIVE: *((int*) b)=idata->active; return(1); 
	    }
	    return(0);
	case TGT_OBJECT_SETTAG:
	    idata=obj->class_data;
	    switch(a)
	    {
		case TGTT_CHECKBOX_ACTIVE: 
		    if(idata->active=(int) b)
			obj->visual_buffer[1]=TGT_T_CHAR(obj->visual_buffer[1],d_checkedchar);
		    else
			obj->visual_buffer[1]=TGT_T_CHAR(obj->visual_buffer[1],d_uncheckedchar);
		    return(1);
		case TGTT_CHECKBOX_ACTIVEBG:
		    idata->activebg=(int) b; 
		    tgt_builtin_checkbox_rebuild(obj);
		    return(1); 
		case TGTT_CHECKBOX_CAPTION: 
		    idata->title=(char*) b;
		    obj->xs=strlen(b)+4;
		    tgt_realloc_buffer(obj); 
		    tgt_builtin_checkbox_rebuild(obj);
		    return(1);
	    }
	    return(0);
        case TGT_OBJECT_SETDEFAULTS:
	    d_actbg=atoi(tgt_getprefs(b,"checkbox","activebg","6"));
	    d_checkedchar=*(tgt_getprefs(b,"checkbox","checked","x"));
	    d_uncheckedchar=*(tgt_getprefs(b,"checkbox","nonchecked"," "));
	    return(1);
				 
	default: return(0);
    }
}
