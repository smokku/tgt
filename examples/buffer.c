#include "tgt.h"
#include "tgt_app.h"
#include <stdio.h>
#include <signal.h>

    struct tgt_object *desktop;
    struct tgt_terminal *myterm;
    struct tgt_object *str;
    struct tgt_object *buff;
    void * sid;

/* Callbacks */
void shutdown()
{
    tgt_destroyobject(desktop);
    _exit(0);
}
void execute(void)
{
    char *cmd;
    tgt_get(str,TGTT_STRING_STRING,&cmd);
    sid=tgt_buffer_system(buff,"ping 213.25.115.1");
}
void stop(void)
{
    tgt_buffer_abort(sid);
}

int main()
{
    struct tgt_object *window;
    struct tgt_object *button;
    char sstring[256];
    int i;

    desktop=tgt_start(NULL,&myterm);

    window=tgt_createandlink(desktop,myterm,TGT_CLASS_WINDOW,(tagitem[]) {TGTT_X,10,TGTT_Y,5,TGTT_XS,60,TGTT_YS,15,TGTT_WINDOW_TITLE, "String Class Test",TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,50,TGTT_Y,12,TGTT_BUTTON_CAPTION, "[Exit]",TGTT_CALLBACK, shutdown,TGTT_END,0});
    buff=tgt_createandlink(window,myterm,TGT_CLASS_TEXTBUFFER,(tagitem[]) {TGTT_X,2,TGTT_Y,4,TGTT_XS,45,TGTT_YS,10,TGTT_BG,0,TGTT_BUFFER_BORDERCOLOR,7,TGTT_END,0});
    str=tgt_createandlink(window,myterm,TGT_CLASS_STRING,(tagitem[]) {TGTT_X,2,TGTT_Y,3,TGTT_XS,43,TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,46,TGTT_Y,3,TGTT_BUTTON_CAPTION, "[Exec]",TGTT_CALLBACK, execute,TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,52,TGTT_Y,3,TGTT_BUTTON_CAPTION, "[Stop]",TGTT_CALLBACK, stop,TGTT_END,0});

    tgt_set(buff,TGTA_BUFFER_ADD,"Blaaaah");
    tgt_set(buff,TGTA_BUFFER_ADD,"Trtddt");
    

    tgt_refresh(desktop);
    tgt_chtimes(0,100000);
    tgt_waitkeys(desktop);
}
