#include <stdio.h>
#include <stdlib.h>
#include "tgt.h"

extern void fb_display(char *rgbbuff, int x_size, int y_size, int x_pan, int y_pan, int x_offs, int y_offs);
extern void getCurrentRes(int *x,int *y);

struct imgdata
{
    unsigned char *pixmap;
    unsigned char *resizebuff;
    short x_size,y_size;
    short fbxr,fbyr;
    short cxs,cys;
    short rbx,rby;
};


extern struct tgt_terminal * g_terminal;

void fb_simpleresize(unsigned char * src,int sx,int sy,unsigned char * dest,int dx,int dy)
{
    int y,x,p;
    for(y=0;y<dy;y++)
	for(x=0;x<dx;x++)
	{
	    p=((x*sx/dx)+(y*sy/dy*sx))*3;
	    *(dest++)=src[p];
	    *(dest++)=src[p+1];
	    *(dest++)=src[p+2];
	}
}

int classmain(struct tgt_object *obj,int type,int a,void *b)
{
    struct imgdata *iw;
    int x,y;

    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    iw=(struct imgdata*) malloc(sizeof(struct imgdata));
	    obj->class_data=iw;
	    obj->objflags|=TGT_OBJFLAGS_NONSELECTABLE;
	    iw->pixmap=(unsigned char *) tgt_getptrtag(b,TGTT_FBPIXMAP_BUFFER,NULL);
	    iw->x_size=tgt_getnumtag(b,TGTT_FBPIXMAP_XSIZE,0);
	    iw->y_size=tgt_getnumtag(b,TGTT_FBPIXMAP_YSIZE,0);
	    getCurrentRes(&x,&y);
	    iw->fbxr=x;iw->fbyr=y;
	    iw->cxs=iw->fbxr/g_terminal->x_size;
	    iw->cys=iw->fbyr/g_terminal->y_size;
	    iw->rbx=iw->cxs*obj->xs;
	    iw->rby=iw->cys*obj->ys;
    
	    if(tgt_getnumtag(b,TGTT_FBPIXMAP_RESIZE,0))
		iw->resizebuff=(unsigned char *) malloc(iw->rbx*iw->rby*3);
	    else
		iw->resizebuff=NULL;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    iw=obj->class_data;
	    if(iw->resizebuff) free(iw->resizebuff);
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    iw=obj->class_data;
	    if(iw->pixmap)
	    {
		if(iw->resizebuff)
		{
		    fb_simpleresize(iw->pixmap,iw->x_size,iw->y_size,iw->resizebuff,iw->rbx,iw->rby);
		    fb_display(iw->resizebuff,iw->rbx,iw->rby,0,0,iw->cxs*(obj->x+a),iw->cys*(obj->y+(int) b));
		}
		else
		    fb_display(iw->pixmap,iw->x_size,iw->y_size,0,0,iw->cxs*(obj->x+a),iw->cys*(obj->y+(int) b));
	    }
	    return(1);
	case TGT_OBJECT_SETTAG:
	    iw=obj->class_data;	    
	    switch(a)
	    {
		case TGTT_FBPIXMAP_BUFFER:
		    iw->pixmap=b;
		    tgt_refresh(obj);
		    return(1);
		case TGTT_FBPIXMAP_XSIZE: iw->x_size=(int) b; return(1);
		case TGTT_FBPIXMAP_YSIZE: iw->y_size=(int) b; return(1);
		case TGTT_FBPIXMAP_RESIZE:
		    iw->rbx=iw->cxs*obj->xs;
		    iw->rby=iw->cys*obj->ys;
		    if(iw->resizebuff) { free(iw->resizebuff); iw->resizebuff=NULL; }
    		    if(b) iw->resizebuff=(unsigned char *) malloc(iw->rbx*iw->rby*3);
		    return(1);
	    }
	    return(0);
	case TGT_OBJECT_GETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_FBPIXMAP_CXSIZE:*(int*)b =iw->cxs; return(1);
		case TGTT_FBPIXMAP_CYSIZE:*(int*)b =iw->cys; return(1);
	    }
	default: return(0);
    }
}
