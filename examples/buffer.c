#include "tgt.h"
#include "tgt_app.h"
#include <stdio.h>
#include <signal.h>

    struct tgt_object *desktop;
    struct tgt_terminal *myterm;
    struct tgt_object *str;
    struct tgt_object *buff;
    int pid;

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
    pid=tgt_buffer_system(buff,"ping 213.25.115.1");
}
void stop(void)
{
    kill(pid,SIGKILL);
}

int main()
{
    struct tgt_object *window;
    struct tgt_object *button;
    char sstring[256];
    int i;

    desktop=tgt_start(NULL,&myterm);

    window=tgt_createandlink(desktop,myterm,TGT_CLASS_WINDOW,(long[]) {TGTT_X,10,TGTT_Y,5,TGTT_XS,60,TGTT_YS,15,TGTT_WINDOW_TITLE,(long) "String Class Test",TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_BUTTON,(long[]) {TGTT_X,50,TGTT_Y,12,TGTT_BUTTON_CAPTION,(long) "[Exit]",TGTT_CALLBACK,(long) shutdown,TGTT_END,0});
    buff=tgt_createandlink(window,myterm,TGT_CLASS_TEXTBUFFER,(long[]) {TGTT_X,2,TGTT_Y,4,TGTT_XS,45,TGTT_YS,10,TGTT_BG,0,TGTT_BUFFER_BORDERCOLOR,7,TGTT_END,0});
    str=tgt_createandlink(window,myterm,TGT_CLASS_STRING,(long[]) {TGTT_X,2,TGTT_Y,3,TGTT_XS,43,TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_BUTTON,(long[]) {TGTT_X,46,TGTT_Y,3,TGTT_BUTTON_CAPTION,(long) "[Exec]",TGTT_CALLBACK,(long) execute,TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_BUTTON,(long[]) {TGTT_X,52,TGTT_Y,3,TGTT_BUTTON_CAPTION,(long) "[Stop]",TGTT_CALLBACK,(long) stop,TGTT_END,0});

    tgt_set(buff,TGTA_BUFFER_ADD,"Blaaaah");
    tgt_set(buff,TGTA_BUFFER_ADD,"Trtddt");
    

    tgt_refresh(desktop);
    tgt_chtimes(0,100000);
    tgt_waitkeys(desktop);
}
