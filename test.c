#include "tgt.h"

    struct tgt_object *desktop;
    struct tgt_object *cwindow;
    struct tgt_terminal *myterm;

/* Callback dla przyciskow - wyjscie z programu i zamkniecie okna dodatkowego*/
void mkexit(int id)
{
    if(id==100)
    {    
	tgt_destroyobject(desktop);
	_exit(0);
    }
    else
    {
	tgt_unlink(cwindow);
	tgt_refresh(desktop);
	tgt_destroyobject(cwindow);
    }
}

/* Callback dla przycisku otwarcia dodatkowegookna */
void newwindow(int a)
{
    cwindow=tgt_createandlink_int(desktop,myterm,TGT_CLASS_WINDOW,
	(long[]) { TTGT_X,6,TTGT_Y,7,
		    TTGT_XS,20,TTGT_YS,5,
		    TTGT_WINDOW_TITLE,"New window",TTGT_END,0});
    tgt_createandlink_int(cwindow,myterm,TGT_CLASS_LABEL,
	(long[]) {TTGT_X,2,TTGT_Y,2,TTGT_LABEL_TEXT,"Strange color:",TTGT_END,0});

    tgt_createandlink_int(cwindow,myterm,TGT_CLASS_BUTTON,
	(long[]) { TTGT_X,2,TTGT_Y,3,TTGT_BUTTON_CAPTION,"Close",
		    TTGT_ID,101,TTGT_CALLBACK,mkexit,TTGT_BG,1,TTGT_END,0});
    tgt_activate(cwindow);
}

main()
{
    struct tgt_object *window;
    struct tgt_object *button;
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

/* obiekty nie refreshuja sie same od siebie... */
    tgt_refresh(desktop);
/* oczywiscie waitkeys mozemy wywolac jako pthread (hmm szczerze mowiac to 
 _jeszcze_ nie probowalem ;)) */
    tgt_waitkeys(desktop);
}
