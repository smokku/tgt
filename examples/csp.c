#include "tgt.h"
#include "tgt_app.h"
#include <stdio.h>

    struct tgt_object *desktop;
    struct tgt_terminal *myterm;
    struct tgt_object *str;
/* Callbacks */
void shutdown()
{
    tgt_destroyobject(desktop);
    _exit(0);
}
    char buffer[200];

void passwd(struct tgt_object *obj,int i)
{
    char *p;
    tgt_get(obj,TGTT_STRING_STRING,&p);
    sprintf(buffer,"The password is %s",p);
    tgt_set(str,TGTT_STRING_STRING,buffer);
}
    const char hit[]="You hit enter!!";
void clearstr(struct tgt_object *obj,int i)
{
    tgt_set(obj,TGTT_STRING_STRING,hit);
}
int main()
{
    struct tgt_object *window;
    char sstring[256];
    int i;

    desktop=tgt_start(NULL,&myterm);
    window=tgt_createandlink(desktop,myterm,TGT_CLASS_WINDOW,(long[]) {TGTT_X,10,TGTT_Y,5,TGTT_XS,60,TGTT_YS,15,TGTT_WINDOW_TITLE,(long) "checkbox/slider/progress-bar test",TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_BUTTON,(long[]) {TGTT_X,50,TGTT_Y,12,TGTT_BUTTON_CAPTION,(long) "[Exit]",TGTT_CALLBACK,(long) shutdown,TGTT_END,0});
    str=tgt_createandlink(window,myterm,TGT_CLASS_STRING,(long[]) {TGTT_X,2,TGTT_Y,3,TGTT_XS,30,TGTT_STRING_STRING,(long)"Type something here",TGTT_STRING_MAX,250,TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_CHECKBOX,(long[]) {TGTT_X,2,TGTT_Y,4,TGTT_CHECKBOX_CAPTION,(long) "Jeden",TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_CHECKBOX,(long[]) {TGTT_X,12,TGTT_Y,4,TGTT_CHECKBOX_CAPTION,(long) "Dwa",TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_SLIDER,(long[]) {TGTT_X,2,TGTT_Y,5,TGTT_XS,20,TGTT_SLIDER_CAPTION,(long)"Slide ",TGTT_SLIDER_MINVALUE,50,TGTT_SLIDER_MAXVALUE,70,TGTT_SLIDER_TYPE,TGT_SLIDERT_SHOWVALUE|TGT_SLIDERT_PERCENT,TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_PROGRESS,(long[]) {TGTT_X,2,TGTT_Y,6,TGTT_XS,20,TGTT_PROGRESS_ENDVALUE,50,TGTT_FG,6,TGTT_END,0});
//    tgt_createandlink(window,myterm,TGT_CLASS_LABEL,(long[]) {TGTT_X,2,TGTT_Y,7,TGTT_LABEL_TEXT,(long) "Numeric :",TGTT_END,0});
//    tgt_createandlink(window,myterm,TGT_CLASS_STRING,(long[]) {TGTT_X,12,TGTT_Y,7,TGTT_XS,20,TGTT_STRING_MAX,30,TGTT_STRING_ACCEPT,(long) "0123456789.-",TGTT_END,0});
    tgt_refresh(desktop);
    tgt_chtimes(0,100000);
    tgt_waitkeys(desktop);
}
