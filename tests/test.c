#include "tgt.h"
#include "tgt_app.h"
#include <stdio.h>
#define MSG 1
#define QUIT 2

    struct tgt_object *desktop;
    struct tgt_object *cwindow;
    struct tgt_terminal *myterm;
    struct tgt_queue *mq;

/* Callback dla przyciskow - wyjscie z programu i zamkniecie okna dodatkowego*/
void mkexit(struct tgt_object *obj)
{
    if(obj->id==100)
    {    
	tgt_app_queue(mq,QUIT,0,0);
    }
    else
    {
	tgt_unlink(obj->ln.parent);
	tgt_refresh(desktop);
	tgt_destroyobject(obj->ln.parent);
    }
}

void refresh(int a) 
{
    tgt_refresh(desktop);
    tgt_app_queue(mq,0,0,"Refresh");
}
/* Callback dla przycisku otwarcia dodatkowegookna */
void newwindow(struct tgt_object *obj)
{
    if(!tgt_haschild(desktop,11121))
    {
	cwindow=tgt_createandlink(desktop,TGT_CLASS_WINDOW,
	(tagitem[]) { TGTT_X,6,TGTT_Y,8,
		    TGTT_XS,20,TGTT_YS,7,
		    TGTT_ID,11121,
		    TGTT_WINDOW_TITLE,"New window",
		    TGTT_PREV_KEYS,(int[]) {'w',0},
		    TGTT_END,0});
	tgt_createandlink(cwindow,TGT_CLASS_LABEL,
	(tagitem[]) {TGTT_X,2,TGTT_Y,2,TGTT_LABEL_TEXT,"Strange color:",TGTT_END,0});

	tgt_createandlink(cwindow,TGT_CLASS_STRING,
	(tagitem[]) { TGTT_X,2,TGTT_Y,3,TGTT_XS,10,TGTT_ID,101,TGTT_END,0});

	tgt_createandlink(cwindow,TGT_CLASS_BUTTON,
	(tagitem[]) { TGTT_X,2,TGTT_Y,4,TGTT_BUTTON_CAPTION,"Freshen",
		    TGTT_ID,101,TGTT_CALLBACK,refresh,TGTT_END,0});

	tgt_setfocus(cwindow);
    }
}

main()
{
    struct tgt_object *window;
    struct tgt_object *button;
    struct tgt_object *astatus;
    struct tgt_queue_msg msg;
    char sstring[256];
    int i;
    desktop=tgt_start(NULL,&myterm);

/* mozna linkowac od razu... */
    window=tgt_createandlink(desktop,TGT_CLASS_WINDOW,(tagitem[]) {TGTT_XS,20,TGTT_YS,10,TGTT_WINDOW_TITLE,"Test",TGTT_END,0});

/* albo po stworzeniu obiektu ... */
    button=tgt_createobject(TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,3,TGTT_Y,3,TGTT_BUTTON_CAPTION,"[   Quit    ]",TGTT_CALLBACK,mkexit,TGTT_ID,100,TGTT_END,0});
    tgt_link(button,window);

    button=tgt_createobject(TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,3,TGTT_Y,4,TGTT_BUTTON_CAPTION,"[Open window]",TGTT_CALLBACK,newwindow,TGTT_END,0});
    tgt_link(button,window);


    astatus=tgt_createandlink(desktop,TGT_CLASS_LABEL,(tagitem[]) {TGTT_X,0,TGTT_Y,24,TGTT_LABEL_TEXT,"",TGTT_END,0});

    sprintf(sstring,"1..2...3... Start");


    tgt_set(astatus,TGTT_LABEL_TEXT,sstring);

/* obiekty nie refreshuja sie same od siebie... */
    tgt_refresh(desktop);
    mq=tgt_app_createqueue();
    
    /* W tym miejscu w zasadzie uruchamiamy cala maszynerie GUI */
    tgt_chtimes(0,100000);
    tgt_async_waitkeys(desktop);
    
    i=0;
    for(;;)
    {
	if(tgt_app_waitmsg(mq,&msg)==1)
	{
	    i=0;
	    if(msg.code==QUIT) break;
    	    sprintf(sstring,"MESSAGE: %s  ",msg.pointer);
	}
	tgt_refresh(astatus);
    }
    tgt_destroyobject(desktop);
}
