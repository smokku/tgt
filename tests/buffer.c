#include "tgt.h"
#include "tgt_app.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
    struct tgt_object *desktop;
    struct tgt_terminal *myterm;
    struct tgt_object *str;
    struct tgt_object *buff;
    void * sid;
    void *hndlr;

/* Callbacks */
void shutdown()
{
    tgt_destroyobject(desktop);
    _exit(0);
}
void execute(void)
{
    char *cmd;
    tgt_get(str,TGTT_STRING_STRING,(tgtt*) &cmd);
    tgt_set(buff,TGTA_BUFFER_ADD,(tgtt) cmd);
}
void stop(void)
{
//    tgt_buffer_abort(sid);
}

int main()
{
    struct tgt_object *window;

    hndlr=tgt_launch_launcher();


    desktop=tgt_start(NULL,&myterm);

    window=tgt_createandlink(desktop,TGT_CLASS_WINDOW,(tagitem[]) {TGTT_X,10,TGTT_Y,5,TGTT_XS,60,TGTT_YS,15,TGTT_WINDOW_TITLE, (tgtt) "String Class Test",TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,50,TGTT_Y,12,TGTT_BUTTON_CAPTION,(tgtt) "[Exit]",TGTT_CALLBACK,(tgtt) shutdown,TGTT_END,0});
    buff=tgt_createandlink(window,TGT_CLASS_TEXTBUFFER,(tagitem[]) {TGTT_X,2,TGTT_Y,4,TGTT_XS,45,TGTT_YS,10,TGTT_BG,0,TGTT_BUFFER_BORDERCOLOR,7,TGTT_END,0});
    str=tgt_createandlink(window,TGT_CLASS_STRING,(tagitem[]) {TGTT_X,2,TGTT_Y,3,TGTT_XS,43,TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,46,TGTT_Y,3,TGTT_BUTTON_CAPTION,(tgtt) "[Add ]",TGTT_CALLBACK,(tgtt) execute,TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,52,TGTT_Y,3,TGTT_BUTTON_CAPTION,(tgtt) "[Stop]",TGTT_CALLBACK,(tgtt) stop,TGTT_END,0});

    tgt_set(buff,TGTA_BUFFER_ADD,(tgtt) "Blaaaah");
    tgt_set(buff,TGTA_BUFFER_ADD,(tgtt) "Trtddt");

    tgt_refresh(desktop);
    tgt_chtimes(0,100000);
    tgt_waitkeys(desktop);
    for(;;) usleep(20000000);
    return(0);
}
