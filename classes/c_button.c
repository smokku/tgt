#include <stdio.h>
#include <stdlib.h>
#include "tgt.h"

struct tgt_int_button
{
    int activebg;
    char *title;
    int state;
};
static int d_actbg=6;

void tgt_builtin_button_rebuild(struct tgt_object *obj)
{
    tgt_cell attr,*buff=obj->visual_buffer;
    int xsl=obj->xs,i;
    unsigned char *title;
    struct tgt_int_button * iw=obj->class_data;
    attr=TGT_T_BUILDCELL(obj->fg,obj->bg,0,0,0);
    title=iw->title;
    if(title)
    {
	if(strlen(title)<xsl) xsl=strlen(title);
	i=obj->xs-xsl;
	while(xsl--) *(buff++)=TGT_T_FCHAR(attr,*(title++));
	while(i--) *(buff++)=TGT_T_FCHAR(attr,' ');
	iw->state=1;
    }
}


int tgt_builtin_button(struct tgt_object *obj,int type,int a,void *b)
{
    int act,n;
    struct tgt_int_button *iw;
    char *title;
    switch(type)
    {
	case TGT_OBJECT_GETSIZES:
	    /* b - pointer to tgt_ac_objectinfo ..*/
	    /* Do we have a title set ? */
	    if((title=(char*) tgt_getptrtag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_BUTTON_CAPTION,NULL)))
	    /* If so - our minimal width is our title length */
		n=strlen(title);
	    else
	    /* If not - ohwell ... */
		n=1;

	    /* Now, if user defined TGT_XS, respect it before title length */	
	    ((struct tgt_ac_objectinfo*) b)->xsize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_XS,n,((struct tgt_ac_objectinfo*) b)->term);
	    ((struct tgt_ac_objectinfo*) b)->ysize=1;
	    ((struct tgt_ac_objectinfo*) b)->sizeflags|=TGT_AC_SF_XFIXED|TGT_AC_SF_YFIXED;
	    /* To be improved                           ^^^^^^^^^^^^^^^^^*/
	    return(1);
	case TGT_OBJECT_CREATE:
	    title=(char*) tgt_getptrtag(b,TGTT_BUTTON_CAPTION,NULL);
	    iw=(struct tgt_int_button*) malloc(sizeof(struct tgt_int_button));
	    obj->class_data=iw;
	    iw->title=title;
	    iw->activebg=tgt_getnumtag(b,TGTT_BUTTON_ACTIVEBG,d_actbg);
	    if(title) if(tgt_getsizetag(b,TGTT_XS,0,obj->term)==0) obj->xs=strlen(title);
	    obj->objectf=(int (*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    return(1);
	case TGT_OBJECT_DESTROY:
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_VISUALINIT:
	    tgt_builtin_button_rebuild(obj);
	    break;
	case TGT_OBJECT_REFRESH:
	    act=tgt_hasfocus(obj);
	    iw=obj->class_data;
	    if(act)
	    {
	        if(iw->state == 1)
		{
		    tgt_change_colors(obj->visual_buffer,obj->xs,obj->fg,iw->activebg,1);
		    iw->state = 2;
		}
	    }
	    else
	    {
		if(iw->state == 2)
		{
		    tgt_change_colors(obj->visual_buffer,obj->xs,obj->fg,obj->bg,0);
		    iw->state = 1;
		}
	    }
	    return(1);
	case TGT_OBJECT_HANDLE:
	    if(a==13 || a==10 || a==32 || a==TGT_KEY_MOUSEUP) 
	    {
		if(obj->objectf!=NULL) obj->objectf(obj);
		return(1);
	    }
	    n=tgt_shalliswitch(obj,a,0);
	    if(n<0) { tgt_activateprev(obj); return(1); }
	    if(n>0) { tgt_activatenext(obj); return(1); }
	    return(0);
	case TGT_OBJECT_SETDEFAULTS:
	    d_actbg=atoi(tgt_getprefs(b,"button","activebg","6"));
	    return(1);
	case TGT_OBJECT_SETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_BUTTON_CAPTION:
		    iw->title=(char*) b; 
		    if(obj->xs==1) obj->xs=strlen(b);
		    tgt_realloc_buffer(obj);
		    tgt_builtin_button_rebuild(obj);
		    return(1);
		case TGTT_BUTTON_ACTIVEBG: iw->activebg=(int) b;tgt_builtin_button_rebuild(obj); return(1);
	    }
	    return(0);
	default: return(0);
    }
}
