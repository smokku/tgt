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
void mkexit(int id)
{
    if(id==100)
    {    
	tgt_app_queue(mq,QUIT,0,0);
    }
    else
    {
	tgt_unlink(cwindow);
	tgt_refresh(desktop);
	tgt_destroyobject(cwindow);
    }
}

void refresh(int a) 
{
    tgt_refresh(desktop);
    tgt_app_queue(mq,0,0,"Refresh");
}
/* Callback dla przycisku otwarcia dodatkowegookna */
void newwindow(int a)
{
    cwindow=tgt_createandlink_int(desktop,myterm,TGT_CLASS_WINDOW,
	(long[]) { TTGT_X,6,TTGT_Y,8,
		    TTGT_XS,20,TTGT_YS,7,
		    TTGT_WINDOW_TITLE,"New window",
		    TTGT_PREV_KEYS,(int[]) {'w',0},
		    TTGT_END,0});
    tgt_createandlink_int(cwindow,myterm,TGT_CLASS_LABEL,
	(long[]) {TTGT_X,2,TTGT_Y,2,TTGT_LABEL_TEXT,"Strange color:",TTGT_END,0});

    tgt_createandlink_int(cwindow,myterm,TGT_CLASS_BUTTON,
	(long[]) { TTGT_X,2,TTGT_Y,3,TTGT_BUTTON_CAPTION,"Close",
		    TTGT_ID,101,TTGT_CALLBACK,mkexit,TTGT_BG,1,TTGT_END,0});

    tgt_createandlink_int(cwindow,myterm,TGT_CLASS_BUTTON,
	(long[]) { TTGT_X,2,TTGT_Y,4,TTGT_BUTTON_CAPTION,"Freshen",
		    TTGT_ID,101,TTGT_CALLBACK,refresh,TTGT_END,0});

    tgt_activate(cwindow);
}

main()
{
    struct tgt_object *window;
    struct tgt_object *button;
    struct tgt_object *astatus;
    struct tgt_queue_msg msg;
    char sstring[256];
    int i;

    myterm=tgt_setscreen((void*)0);
    desktop=tgt_getdesktop(myterm);

/* mozna linkowac od razu... */
    window=tgt_createandlink_int(desktop,myterm,TGT_CLASS_WINDOW,(long[]) {TTGT_XS,20,TTGT_YS,10,TTGT_WINDOW_TITLE,"Test",TTGT_END,0});

/* albo po stworzeniu obiektu ... */
    button=tgt_createobject_int(myterm,TGT_CLASS_BUTTON,(long[]) {TTGT_X,3,TTGT_Y,3,TTGT_BUTTON_CAPTION,"[   Quit    ]",TTGT_CALLBACK,mkexit,TTGT_ID,100,TTGT_END,0});
    tgt_link(button,window);

    button=tgt_createobject_int(myterm,TGT_CLASS_BUTTON,(long[]) {TTGT_X,3,TTGT_Y,4,TTGT_BUTTON_CAPTION,"[Open window]",TTGT_CALLBACK,newwindow,TTGT_END,0});
    tgt_link(button,window);


    astatus=tgt_createandlink_int(desktop,myterm,TGT_CLASS_LABEL,(long[]) {TTGT_X,0,TTGT_Y,24,TTGT_LABEL_TEXT,"",TTGT_END,0});

    sprintf(sstring,"1..2...3... Start");

    astatus->classf(astatus,TGTM_LABEL_CHANGETEXT,0,sstring);

/* obiekty nie refreshuja sie same od siebie... */
    tgt_refresh(desktop);

    mq=tgt_app_createqueue();
    
    /* W tym miejscu w zasadzie uruchamiamy cala maszynerie GUI */
    tgt_async_waitkeys(desktop);
    
    i=0;
    for(;;)
    {
	if(tgt_app_checkmsg(mq,&msg,1,0)==1)
	{
	    i=0;
	    if(msg.code==QUIT) break;
    	    sprintf(sstring,"MESSAGE: %s  ",msg.pointer);
	}
	else
    	    sprintf(sstring,"No message within last %d seconds  ",++i);

	tgt_refresh(astatus);
    }
    tgt_destroyobject(desktop);
}
