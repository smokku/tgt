#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "tgt.h"

struct tgt_ac_objectinfo* tgt_ac_createspace(int xs,int ys);

struct tgt_object * tgt_ac_mkwindow(struct tgt_terminal * terminal,char *title,
				    int alignment,tagitem * description,...)
{
/* Build window object using the description in description (see examples),
   which is of type type; set window title to title and position it on terminal,
   aligning appropiately */
    struct tgt_ac_objectinfo *oi;
    struct tgt_object * win;
    int wxs,wys,xp,yp; /* xsize, ysize, xposition, yposition */
    va_list list;
    
    /* Window taglist */
    tgtt stdtaglist[]={TGTT_X,0,TGTT_Y,0,TGTT_XS,0,TGTT_YS,0,TGTT_WINDOW_TITLE,0,TGTT_END,0};
    
    
    /* Calculate size of whole window contens and create
       objects' descriptions... */
    if(!(oi=tgt_ac_minimalsize(terminal,TGT_AC_VERTICALGROUP,description,0))) return(NULL);
    wxs=oi->xsize+2; wys=oi->ysize+2;
    /* Check if it fits on terminal ... */
    if(wxs>terminal->x_size || wys>terminal->y_size) return(NULL);
    /* WARNING : Memory Leak if it does not ! ^^^ */
    
    /* Align the window position ... */
    xp=0; yp=0; 

    va_start(list,description);
    if(alignment&TGT_AC_RIGHT)  xp+=terminal->x_size-wxs;
    if(alignment&TGT_AC_HCENTER) xp+=(terminal->x_size-wxs)/2;
    if(alignment&TGT_AC_BOTTOM)  yp+=terminal->y_size-wys;
    if(alignment&TGT_AC_VCENTER) yp+=(terminal->y_size-wys)/2;
    if(alignment&TGT_AC_CUSTOM)
    {
	xp=(va_arg(list,int) * terminal->x_size / 100)-(wxs/2);
	yp=(va_arg(list,int) * terminal->y_size / 100)-(wys/2);
	if(xp<0) xp=0;
	if(yp<0) yp=0;
	if(xp>(terminal->x_size - wxs)) xp=terminal->x_size-wxs;
	if(yp>(terminal->y_size - wys)) yp=terminal->x_size-wys;
    }
    va_end(list);
    /* Order objects to choose positions for them , see below */
    tgt_ac_positionself(oi,1,1,oi->xsize,oi->ysize); 
    /* Fill the taglist and create our window .. */
    stdtaglist[1]=xp;
    stdtaglist[3]=yp;
    stdtaglist[5]=wxs;
    stdtaglist[7]=wys;
    stdtaglist[9]=(tgtt) title;
    if(!(win=tgt_createobject(terminal,TGT_CLASS_WINDOW,stdtaglist))) return(NULL);
    
    /* Order objects to create and link themselves, see below .. */
    if(!tgt_ac_createself(win,terminal,oi)) return(NULL);
    return(win);
}


/* Function that is called first ... */

struct tgt_ac_objectinfo * tgt_ac_minimalsize(struct tgt_terminal * term, int type,tagitem *tl,int spacing)
{
    struct tgt_ac_objectinfo * ret;
    
    /* Estimate minimal or exact size of object of type type, described
       in tl, and doing it recursively for all children when necessary
       (eg. for calculating X-size of horizontal group etc); create
       an tgt_ac_objectinfo structure describing object and return it */
     
    ret=(struct tgt_ac_objectinfo*) malloc(sizeof(struct tgt_ac_objectinfo));
    ret->type=type; ret->nchildren=0; ret->sizeflags=0; ret->pointer=NULL;
    ret->term=term;
    /* Defaults, no children, size is the maximal size (see autocreat.h),
       don't save a pointer to object when you finish creating it */ 
    
    if(type<TGT_CLASS_ACBASE)  /* Non-special object (specifically, not a group) */
    {
	ret->ctl=(void*)tl;    /* Save it's taglist for further usage... */
	/* And call class method to estimate object size for us */
	if(!tgt_int_getclass(type)(NULL,TGT_OBJECT_GETSIZES,0,(void*) ret)) return(NULL);
    }
    else
    {
	ret->ctl=NULL; /* empty children list, NULL -> for realloc()  */
	switch(type)
	{
	    case TGT_AC_HORIZONTALGROUP:if(!tgt_ac_minimalsize_g(term,tl,TGT_AC_HORIZONTALGROUP,ret,spacing)) return(NULL);break;
	    case TGT_AC_VERTICALGROUP:if(!tgt_ac_minimalsize_g(term,tl,TGT_AC_VERTICALGROUP,ret,spacing)) return(NULL);break;
	}
    }
    return(ret);
}



static void tgt_ac_addspaces(struct tgt_ac_objectinfo * ret,int k,int type,int *mx,int *my)
{
    ret->ctl=(void*) realloc(ret->ctl,(sizeof(struct tgt_ac_objectinfo*))*(ret->nchildren+1));
    if(type==TGT_AC_HORIZONTALGROUP) /* Horizontal or vertical space ? */
    {
	((struct tgt_ac_objectinfo**)ret->ctl) [ret->nchildren++] = tgt_ac_createspace(k,1);
	(*mx) += k;
    }
    else
    {
	((struct tgt_ac_objectinfo**)ret->ctl) [ret->nchildren++] = tgt_ac_createspace(1,k);
	(*my) += k;
    }
}

int tgt_ac_minimalsize_g(struct tgt_terminal * term,tgtt *tl,int type,struct tgt_ac_objectinfo * ret,int spacing)
{
/* Recursively calculate minimal size of group of type type , which attributes
   and children data are contained in tl[] ; encapsulate calculated data in
   a preallocated struct tgt_ac_objectinfo, which is pointed by ret */
   
    int i,level,alignment=0; /* Index, recursion level (do NOT touch anything 
			with level>0), current alignment */
    int my=0,mx=0,k; /* X and Y size */
    void *pointer=NULL;
    int chldspacing=spacing; 
    int fixed = 1;
	/* A location where to save the pointer to the next child at */

    if(spacing) tgt_ac_addspaces(ret,spacing,type,&mx,&my);
    
    for(level=0,i=0;level>=0;i++)
    {
	switch(tl[i++]) /* Well well well .. what do we have here ... ? */
	{
	    case TGT_AC_FIX_X:
		ret->sizeflags|=TGT_AC_SF_XFIXED;
		break;
	    case TGT_AC_FIX_Y:
		ret->sizeflags|=TGT_AC_SF_YFIXED;
		break;
	    case TGT_AC_XS:
		if((k=tl[i]) < 0) k = (0-k * term->x_size) / 100;
		if(k > mx) mx = k;
		break;
	    case TGT_AC_YS:
		if((k=tl[i]) < 0) k = (0-k * term->y_size) / 100;
		if(k > my) my = k;
		break;
	    case TGT_AC_SAVEAS: /* Save next object pointer as ... */
		if(level==0) pointer=(tgt_ptrtype) tl[i];
		break;
	    case TGT_AC_ALIGNMENT: /* Set current alignment to ... */
		if(level==0) alignment=tl[i];
		break;
	    case TGT_AC_SPACING:
		if(level==0) chldspacing=tl[i];
		break;
	    case TGT_AC_SPACE: /* Create a space between children,
			specifically, create a dummy child description
			(see tgt_ac_createspace() below) */
		if(level==0)
		{
		    tgt_ac_addspaces(ret,tl[i],type,&mx,&my);
		    /* Reallocate children list ... */
		}
		break;
	    case TGT_AC_CHILD: /* Add a child by calling tgt_ac_minimalsize recursively .. */
		if(level==0)
		{
		    struct tgt_ac_objectinfo * subinfo;
		    /* Recursive call .. after executing it, we have the description
		       of our new child at subinfo */
		    if(!(subinfo=tgt_ac_minimalsize(term,tl[i],tl+i+1,chldspacing))) return(0);
		    /* Set alignment to current alignment */
		    subinfo->alignment=alignment;
		    /* Set save-as pointer, if TGT_AC_CHILD was predesced by
		       TGT_AC_SAVEAS */
		    if(pointer) { subinfo->pointer=pointer; pointer=NULL; }
		    if(type==TGT_AC_HORIZONTALGROUP)
		    {
			/* Are we a horizontal group? Then add child X
			   size to our current size, and make our new
			   Y size equal max(our_old_y_size,child_y_size) */
			mx+=subinfo->xsize;if(subinfo->ysize>my) my=subinfo->ysize;
			if(! subinfo->sizeflags & TGT_AC_SF_XFIXED) fixed = 0;
		    }
		    else
		    {
			/* Are we a vertical group? Then add child Y
			   size to our current size, and make our new
			   X size equal max(our_old_x_size,child_x_size) */
			my+=subinfo->ysize;if(subinfo->xsize>mx) mx=subinfo->xsize;
			if(! subinfo->sizeflags & TGT_AC_SF_YFIXED) fixed = 0;
		    }
		    
		    /* Reallocate children list ... */
		    ret->ctl=(void*) realloc(ret->ctl,(sizeof(struct tgt_ac_objectinfo*))*(ret->nchildren+1));
		    /* And save there a pointer to our new child information ... */
		    ((struct tgt_ac_objectinfo**)ret->ctl) [ret->nchildren++] = subinfo;
		    
		}
		level++; /* Increase recurse level if we reached 
			TGT_AC_CHILD : DO NOT PARSE TAGS UNTIL WE ARE BACK TO LEVEL
			ZERO OF RECURSION */
		break;
	    case TGT_AC_END: level--; break; /* An END tag ? Then decrease recursion level ... 
	       if we are on level zero, then it'll go negative, and will stop the loop
	       (see for() condition above) */
	}
    }
    if(spacing) tgt_ac_addspaces(ret,spacing,type,&mx,&my);
    if(fixed)
    {
	if(type==TGT_AC_HORIZONTALGROUP)
	{
	    ret->sizeflags|=TGT_AC_SF_XFIXED;
	}
	else
	{
	    ret->sizeflags|=TGT_AC_SF_YFIXED;
	}
    }
    ret->xsize=mx; /* Save calculated values to struct tgt_ac_objectinfo */
    ret->ysize=my;
    return(1);
}


void tgt_ac_positionself(struct tgt_ac_objectinfo * oi,int xp,int yp,int xs,int ys)
{
/*
   Imagine:
   You are object , which information about is contained in oi. You can use
   area between (xp,yp)-(xp+xs,yp+ys)... Choose yourself a place, depending
   on your alignment or whatever .. if you are a group, then recursively
   tell your children to do the same ;) */  
   
    int i,n,x,y;
    struct tgt_ac_objectinfo *child;

    /* If object's size is fixed and the area it can spread is too large,
       then align it , according to oi->alignment value ... */

    if(oi->xsize<xs && oi->sizeflags&TGT_AC_SF_XFIXED)
    {
        if(oi->alignment&TGT_AC_RIGHT)  xp+=xs-oi->xsize;
        if(oi->alignment&TGT_AC_HCENTER) xp+=(xs-oi->xsize)/2;
    }
    else
	oi->xsize=xs;
	
    if(oi->ysize<ys && oi->sizeflags&TGT_AC_SF_XFIXED)
    {
        if(oi->alignment&TGT_AC_BOTTOM)  yp+=ys-oi->ysize;
        if(oi->alignment&TGT_AC_VCENTER) yp+=(ys-oi->ysize)/2;
    }
    else
	oi->ysize=ys;

	
    x=oi->xpos=xp; /* Save your position .. */
    y=oi->ypos=yp;

//    printf("Type %d position %d, %d alignment %d\n",oi->type,xp,yp,oi->alignment);

    /* ... and if you're a group ... */

    switch(oi->type)
    {
	case TGT_AC_HORIZONTALGROUP:
	    /* ... recursively find coordinates for your children and
	       tell them to position themselves */

	    for(i=0,n=oi->nchildren;i<n;i++) 
	    {
		child=((struct tgt_ac_objectinfo**) oi->ctl)[i];
		tgt_ac_positionself(child,x,y,child->xsize,ys);
		x+=child->xsize;		
	    }
	    break;
	case TGT_AC_VERTICALGROUP:
	    for(i=0,n=oi->nchildren;i<n;i++) 
	    {
		child=((struct tgt_ac_objectinfo**) oi->ctl)[i];
		tgt_ac_positionself(child,x,y,xs,child->ysize);
		y+=child->ysize;
	    }
	    break;
    }
}

int tgt_ac_createself(struct tgt_object *linkto,struct tgt_terminal * term,struct tgt_ac_objectinfo * oi)
{
    struct tgt_object * k;
/*  Imagine:
    You are an object which description is contained in oi... You have there
    also your new coordinates and size ... You should link to object linkto
    and use terminal type term ... SO WHAT ARE YOU WAITING FOR? Materialize
    yourself .. ;) */

    if(oi->type<TGT_CLASS_ACBASE)
    {
	/* Regular object? Fairly easy ... See new tgt_createobject_int() ... */
	if(!(k=tgt_createobject_int(term,tgt_int_getclass(oi->type),(tagitem*) oi->ctl,oi->xsize,oi->ysize,oi->xpos,oi->ypos))) return(0);
	tgt_link(k,linkto);
	if(oi->pointer) *(oi->pointer)=k; /* Save pointer if it was requested by TGT_AC_SAVEAS */
    }
    else
    {
	/* Group? Then create all your children recursively... */
	int i,n;
	for(i=0,n=oi->nchildren;i<n;i++) 
	    if(!tgt_ac_createself(linkto,term,((struct tgt_ac_objectinfo**) oi->ctl)[i])) return(0);
	    /* Free children list .. */
	if(oi->nchildren) free(oi->ctl);
    }
    free(oi); /* Free object description */
    return(1);
}





struct tgt_ac_objectinfo* tgt_ac_createspace(int xs,int ys)
{
    /* Create a description of a blank object sized xs per ys,
       it's being used for spacing between objects */
    struct tgt_ac_objectinfo * ret;
    ret=(struct tgt_ac_objectinfo*) malloc(sizeof(struct tgt_ac_objectinfo));
    ret->type=TGT_AC_SPACINGOBJECT;
    ret->nchildren=0;
    ret->sizeflags=0;
    ret->pointer=NULL;
    ret->xsize=xs;
    ret->ysize=ys;
    ret->sizeflags=TGT_AC_SF_XFIXED|TGT_AC_SF_YFIXED;
    ret->alignment=0;
    return(ret);
}
