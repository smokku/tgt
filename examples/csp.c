#include "tgt.h"
#include "tgt_app.h"
#include <stdio.h>

    struct tgt_object *desktop;
    struct tgt_terminal *myterm;
    struct tgt_object *progr;
/* Callbacks */
void shutdown()
{
    tgt_destroyobject(desktop);
    _exit(0);
}
    char buffer[200];

void slider_move(struct tgt_object *obj,int i)
{
    int max,min,end;
    tgt_get(obj,TGTT_SLIDER_MAXVALUE, &max);
    tgt_get(obj,TGTT_SLIDER_MINVALUE, &min);
    tgt_get(progr,TGTT_PROGRESS_ENDVALUE, &end);
//    fprintf(stderr,"max:%d min:%d end:%d i:%d  wynik:%d\n", max,min,end,i,(i-min) * end / (max - min));
    tgt_set(progr,TGTT_PROGRESS_VALUE, (i - min) * end / (max - min));
    tgt_refresh(progr);
}
int main()
{
    struct tgt_object *window;
    char sstring[256];
    int i;

    myterm=tgt_setscreen((void*)0);
    desktop=tgt_getdesktop(myterm);
    window=tgt_createandlink(desktop,myterm,TGT_CLASS_WINDOW,(tagitem[]) {TGTT_X,10,TGTT_Y,5,TGTT_XS,60,TGTT_YS,15,TGTT_WINDOW_TITLE, "checkbox/slider/progress-bar test",TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,50,TGTT_Y,12,TGTT_BUTTON_CAPTION, "[Exit]",TGTT_CALLBACK, shutdown,TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_STRING,(tagitem[]) {TGTT_X,2,TGTT_Y,3,TGTT_XS,30,TGTT_STRING_STRING,"Type something here",TGTT_STRING_MAX,250,TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_CHECKBOX,(tagitem[]) {TGTT_X,2,TGTT_Y,4,TGTT_CHECKBOX_CAPTION,"Jeden",TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_CHECKBOX,(tagitem[]) {TGTT_X,12,TGTT_Y,4,TGTT_CHECKBOX_CAPTION, "Dwa",TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_SLIDER,(tagitem[]) {TGTT_X,2,TGTT_Y,5,TGTT_XS,20,TGTT_SLIDER_CAPTION,"Slide ",TGTT_SLIDER_MINVALUE,50,TGTT_SLIDER_MAXVALUE,140,TGTT_SLIDER_TYPE,TGT_SLIDERF_SHOWVALUE|TGT_SLIDERF_PERCENT,TGTT_CALLBACK,(long) slider_move,TGTT_END,0});
    progr=tgt_createandlink(window,myterm,TGT_CLASS_PROGRESS,(tagitem[]) {TGTT_X,2,TGTT_Y,6,TGTT_XS,20,TGTT_PROGRESS_VALUE,10,TGTT_PROGRESS_ENDVALUE,50,TGTT_FG,6,TGTT_PROGRESS_TYPE,TGT_PROGRESSF_SHOWVALUE,TGTT_END,0});
//    tgt_createandlink(window,myterm,TGT_CLASS_LABEL,(tagitem[]) {TGTT_X,2,TGTT_Y,7,TGTT_LABEL_TEXT,(long) "Numeric :",TGTT_END,0});
//    tgt_createandlink(window,myterm,TGT_CLASS_STRING,(tagitem[]) {TGTT_X,12,TGTT_Y,7,TGTT_XS,20,TGTT_STRING_MAX,30,TGTT_STRING_ACCEPT,(long) "0123456789.-",TGTT_END,0});
    tgt_refresh(desktop);
    tgt_chtimes(0,100000);
    tgt_waitkeys(desktop);
}
