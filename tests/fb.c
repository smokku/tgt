#include "tgt.h"
#include "tgt_app.h"
#include <stdio.h>
#include <stdlib.h>

struct tgt_object *desktop;
struct tgt_queue *q;

void destroy()
{
    tgt_app_queue(q,0,0,NULL);
}

int main()
{
    struct tgt_terminal *term;
    struct tgt_object *window;
    struct tgt_object *fbo;
    struct tgt_queue_msg m;

    int i,k,j;

    unsigned char *buffer;

    desktop=tgt_start(NULL,&term); /* Get root object */
    
    window=tgt_createobject(term,TGT_CLASS_WINDOW, /* Create a window object */
	(tagitem[]){TGTT_X, 20, TGTT_Y, 3,
		 TGTT_XS,40, TGTT_YS,20,
		 TGTT_WINDOW_TITLE,(tgtt) "Hello World",
		 TGTT_END,0});
    
    tgt_link(window,desktop); /* Attach it to the root object */
    
    printf("load\n");
    if(!tgt_loadclass(1002,"./fbpic.class")) printf("Dynamic class loading failed\n");
    printf("done\ninit\n");
    
    tgt_createandlink(window,TGT_CLASS_BUTTON, /* 2 in 1 ... Create and attach in one function */
	(tagitem[]){TGTT_X, 28, TGTT_Y, 17,
	         TGTT_BUTTON_CAPTION, (tgtt) "[Quit]",
		 TGTT_CALLBACK,(tgtt) destroy, /*This will be called when the button is pressed*/
		 TGTT_END,0});

    printf("buffer\n");
    buffer=(unsigned char *) malloc(100*100*3);
    for(i=0;i<30000;i++) buffer[i]=0;

    printf("creat\n");
    
    fbo=tgt_createandlink(window,1002,(tagitem[]){TGTT_X, 2, TGTT_Y, 2,
		 TGTT_FBPIXMAP_RESIZE,1,
		 TGTT_FBPIXMAP_XSIZE,100,
		 TGTT_FBPIXMAP_YSIZE,100,
		 TGTT_XS,36,
		 TGTT_YS,13,
		 TGTT_END,0});
    tgt_set(fbo,TGTT_FBPIXMAP_BUFFER,(tgtt) buffer);

    q=tgt_app_createqueue();

    tgt_refresh(desktop);    
    tgt_async_waitkeys(desktop);
    for(;;)
    {
	if(tgt_app_checkmsg(q,&m,0,0)) break;
	for(j=0;j<99;j++)
	{
	    for(i=0;i<100;i++)
	    {
		buffer[(i*100+j)*3]=buffer[(i*100+j+1)*3];
		buffer[(i*100+j)*3+1]=buffer[(i*100+j+1)*3+1];
		buffer[(i*100+j)*3+2]=buffer[(i*100+j+1)*3+2];
	    }
	}
	k=rand() % 100;
	for(i=0;i<100;i++)
	{
		buffer[(i*100+99)*3]=0;
		buffer[(i*100+99)*3+1]=0;
		buffer[(i*100+99)*3+2]=0;
	}
	buffer[(k*100+99)*3]=rand()%255;
	buffer[(k*100+99)*3+1]=rand()%255;
	buffer[(k*100+99)*3+2]=rand()%255;
	

	tgt_refresh(fbo);
    }
    tgt_destroyobject(desktop);
    return(0);
}