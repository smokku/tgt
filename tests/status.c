#include "tgt.h"
#include "tgt_app.h"
#include <stdio.h>

    struct tgt_object *desktop;
    struct tgt_terminal *myterm;
    struct tgt_object *status1,*status2,*string1;
/* Callbacks */
void shutdown()
{
    tgt_destroyobject(desktop);
    _exit(0);
}

void rewritestatus(struct tgt_object *obj,int i)
{
    char *text;
    tgt_get(string1,TGTT_STRING_STRING, (tgtt*) &text);
    tgt_set(status1,TGTT_STATUS_TEXT, (tgtt) text);
    tgt_set(status2,TGTT_STATUS_TEXT, (tgtt) text);
    tgt_refresh(status1);
    tgt_refresh(status2);
}
	
int main()
{
    struct tgt_object *window;

    myterm=tgt_setscreen((void*)0);
    desktop=tgt_getdesktop(myterm);
    window=tgt_createandlink(desktop,TGT_CLASS_WINDOW,(tagitem[]) {TGTT_X,10,TGTT_Y,5,TGTT_XS,60,TGTT_YS,15,TGTT_WINDOW_TITLE, (tgtt) "statusbar test",TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,50,TGTT_Y,11,TGTT_BUTTON_CAPTION,(tgtt) "[Exit]",TGTT_CALLBACK,(tgtt) shutdown,TGTT_END,0});
    string1=tgt_createandlink(window,TGT_CLASS_STRING,(tagitem[]) {TGTT_X,2,TGTT_Y,3,TGTT_XS,30,TGTT_STRING_STRING,(tgtt) "Type your desired status here",TGTT_STRING_MAX,250,TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,20,TGTT_Y,10,TGTT_BUTTON_CAPTION, (tgtt) "[ Show me status ]",TGTT_CALLBACK, (tgtt) rewritestatus,TGTT_END,0});
    status1=tgt_createandlink(window,TGT_CLASS_STATUS,(tagitem[]) {TGTT_FG,3,TGTT_BG,2,TGTT_STATUS_TEXT, (tgtt) "Your status here...",TGTT_STATUS_TYPE,TGT_STATUSF_ALIGNED,TGTT_END,0});
    status2=tgt_createandlink(desktop,TGT_CLASS_STATUS,(tagitem[]) {TGTT_FG,7,TGTT_BG,1,TGTT_STATUS_TEXT,(tgtt)  "Your status here...",TGTT_STATUS_TYPE,TGT_STATUSF_ATTACHED,TGTT_END,0});
//    tgt_createandlink(string1,TGT_CLASS_STATUS,(tagitem[]) {TGTT_Y,1,TGTT_BG,5,TGTT_STATUS_TEXT, (tgtt) "strange string's status here...",TGTT_STATUS_TYPE,TGT_STATUSF_ATTACHED,TGTT_END,0});
    tgt_refresh(desktop);
    tgt_chtimes(0,100000);
    tgt_waitkeys(desktop);
    return(0);
}
