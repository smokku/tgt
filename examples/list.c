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
int main()
{
    struct tgt_object *window;
    char sstring[256];
    int i;

    myterm=tgt_setscreen((void*)0);
    desktop=tgt_getdesktop(myterm);
    window=tgt_createandlink(desktop,myterm,TGT_CLASS_WINDOW,(long[]) {TGTT_X,10,TGTT_Y,5,TGTT_XS,60,TGTT_YS,15,TGTT_WINDOW_TITLE,(long) "List Class Test",TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_BUTTON,(long[]) {TGTT_X,50,TGTT_Y,12,TGTT_BUTTON_CAPTION,(long) "[Exit]",TGTT_CALLBACK,(long) shutdown,TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_LIST,
	    (long[]) {TGTT_X,5,TGTT_Y,3,
		      TGTT_XS,20,TGTT_YS,6,
		      TGTT_LIST_ITEMS,
     (long) (char* []){ "TGT: Authors","----","Thomas Sterna","smoku","smoku@linux-pl.com","----","Matthew Golicz","mteg","mtg@elsat.net.pl",NULL},
		      TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_LIST,
	    (long[]) {TGTT_X,26,TGTT_Y,3,
		      TGTT_XS,20,TGTT_YS,6,
		      TGTT_LIST_ITEMS,
     (long) (char* []){ "TGT: Authors","----","Thomas Sterna","smoku","smoku@linux-pl.com","----","Matthew Golicz","mteg","mtg@elsat.net.pl",NULL},
                	TGTT_LIST_FRAMECOLOR,6,
		      TGTT_END,0});

    tgt_refresh(desktop);
    tgt_chtimes(0,100000);
    tgt_waitkeys(desktop);
}
