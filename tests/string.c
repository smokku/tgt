#include "tgt.h"
#include "tgt_app.h"
#include <stdio.h>
#include <unistd.h>

    struct tgt_object *desktop;
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
    tgt_get(obj,TGTT_STRING_STRING,(tgtt*) &p);
    sprintf(buffer,"The password is %s",p);
    tgt_set(str,TGTT_STRING_STRING,(tgtt) buffer);
}
    const char hit[]="You hit enter!!";
void clearstr(struct tgt_object *obj,int i)
{
    tgt_set(obj,TGTT_STRING_STRING,(tgtt) hit);
}
int main()
{
    struct tgt_object *window;

    desktop=tgt_start(NULL);
    window=tgt_createandlink(desktop,TGT_CLASS_WINDOW,(tagitem[]) {TGTT_X,10,TGTT_Y,5,TGTT_XS,60,TGTT_YS,15,TGTT_WINDOW_TITLE, (tgtt) "String Class Test",TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,50,TGTT_Y,12,TGTT_BUTTON_CAPTION,(tgtt)  "[Exit]",TGTT_CALLBACK, (tgtt) shutdown,TGTT_END,0});
    str=tgt_createandlink(window,TGT_CLASS_STRING,(tagitem[]) {TGTT_X,2,TGTT_Y,3,TGTT_XS,30,TGTT_STRING_STRING,(tgtt) "Type something here",TGTT_STRING_MAX,250,TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_LABEL,(tagitem[]) {TGTT_X,2,TGTT_Y,4,TGTT_LABEL_TEXT,(tgtt)  "Password:",TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_STRING,(tagitem[]) {TGTT_X,12,TGTT_Y,4,TGTT_XS,20,TGTT_STRING_MAX,30,TGTT_CALLBACK,(tgtt)  passwd,TGTT_STRING_HASHED,'*',TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_STRING,(tagitem[]) {TGTT_X,2,TGTT_Y,5,TGTT_XS,20,TGTT_STRING_STRING,(tgtt) "Hit enter over here",TGTT_STRING_MAX,30,TGTT_CALLBACK,(tgtt)  clearstr,TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_STRING,(tagitem[]) {TGTT_X,2,TGTT_Y,6,TGTT_XS,20,TGTT_STRING_MAX,50,TGTT_STRING_STRING, (tgtt) "Nicer colours and empty char", TGTT_STRING_EMPTYCHAR,'.',TGTT_FG,6,TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_LABEL,(tagitem[]) {TGTT_X,2,TGTT_Y,7,TGTT_LABEL_TEXT, (tgtt) "Numeric :",TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_STRING,(tagitem[]) {TGTT_X,12,TGTT_Y,7,TGTT_XS,20,TGTT_STRING_MAX,30,TGTT_STRING_ACCEPT, (tgtt) "0123456789.-",TGTT_END,0});
    tgt_refresh(desktop);
    tgt_chtimes(0,100000);
    tgt_waitkeys(desktop);
    return(0);
}
