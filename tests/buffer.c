/* ----------------------------------------------------------------------------
**      buffer.c Example of text buffer usage in TGT
** ----------------------------------------------------------------------------
**      TGT                                       TextGUIToolkit Library
**      Copyright (C)   2000-2001       Mateusz Golicz and Tomasz Sterna
**
**      LGPL license
**
** ----------------------------------------------------------------------------
**      MAINTAINER      Mateusz Golicz
**
**
**      DO NOT TOUCH THIS CODE. IT'S UGLY, UGLY, UGLY, UGLY!
**
*/



#include "tgt.h"
#include "tgt_app.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
    struct tgt_object *desktop;
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

int main()
{
    struct tgt_object *window;
    struct tgt_object *obj;
    int xs, ys;
    struct tgt_terminal *myterm;

    hndlr=tgt_launch_launcher();


    desktop=tgt_start(NULL);
    myterm=tgt_getterminal();

    window=tgt_createandlink(desktop,TGT_CLASS_WINDOW,(tagitem[]) {TGTT_WINDOW_TITLE, (tgtt) "String Class Test",TGTT_END,0});
    tgt_set(window, TGTT_XS, xs = myterm->x_size * 3 / 4);
    tgt_set(window, TGTT_YS, ys = myterm->y_size / 2);
    tgt_set(window, TGTT_X,  (myterm->x_size - xs) / 2);
    tgt_set(window, TGTT_Y,  (myterm->y_size - ys) / 2);
    obj = tgt_createandlink(window,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_BUTTON_CAPTION,(tgtt) "[Exit]",TGTT_CALLBACK,(tgtt) shutdown,TGTT_END,0});
    tgt_set(obj, TGTT_X, xs - 10);
    tgt_set(obj, TGTT_Y, ys - 3);
    
    buff=tgt_createandlink(window,TGT_CLASS_TEXTBUFFER,(tagitem[]) {TGTT_X,2,TGTT_Y,4,TGTT_XS,45,TGTT_YS,10,TGTT_BG,0,TGTT_BUFFER_BORDERCOLOR,7,TGTT_END,0});
    str=tgt_createandlink(window,TGT_CLASS_STRING,(tagitem[]) {TGTT_X,2,TGTT_Y,3,TGTT_XS,43,TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,46,TGTT_Y,3,TGTT_BUTTON_CAPTION,(tgtt) "[Add ]",TGTT_CALLBACK,(tgtt) execute,TGTT_END,0});

    tgt_set(buff,TGTA_BUFFER_ADD,(tgtt) "Blaaaah");
    tgt_set(buff,TGTA_BUFFER_ADD,(tgtt) "Trtddt");

    tgt_refresh(desktop);
    tgt_chtimes(0,100000);
    tgt_waitkeys(desktop);
    for(;;) usleep(20000000);
    return(0);
}
