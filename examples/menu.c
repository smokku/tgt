#include "tgt.h"
#include "tgt_app.h"
#include "tgt_menu.h"
#include <stdio.h>
#include <string.h>

    struct tgt_object *desktop;
    struct tgt_terminal *myterm;
    struct tgt_object *menu1;
    struct tgt_object *menu2;
    struct tgt_object *status;
/* Callbacks */
void shutdown()
{
    tgt_destroyobject(desktop);
    _exit(0);
}
    static char buffer[200];
void menu(struct tgt_object *obj,int id)
{
    sprintf(buffer,"User selected menu position, ID %d  \n",id);
    tgt_set(status,TGTT_LABEL_TEXT,buffer);
    tgt_refresh(status);
}
void cyclechange(struct tgt_object *obj,int id)
{
    sprintf(buffer,"User selected cycle position, nr %d  \n",id);
    tgt_set(status,TGTT_LABEL_TEXT,buffer);
    tgt_refresh(status);
}

void newmenu() { tgt_setfocus(menu2); }
void hotkeys(struct tgt_object *obj,int k) { if(k==0x1b) tgt_setfocus(menu1); }

int main()
{
    struct tgt_object *window;
    char sstring[256];
    void *pdmptr,*cxmptr,*sm,*os;
    int i;

    os=tgt_menu_context(0);
    tgt_menu_add(os,"No",104,0,NULL);
    tgt_menu_add(os,"Sure",105,0,NULL);
    tgt_menu_add(os,"I've got no idea",106,0,NULL);

    pdmptr=tgt_menu_context(TGT_MENUF_HORIZONTAL);

    sm=tgt_menu_context(0);
    tgt_menu_add(sm,"Load...",100,0,NULL);
    tgt_menu_add(sm,"Save...",101,0,NULL);
    tgt_menu_add(sm,"---------------",102,TGT_MENUF_NONSELECTABLE,NULL);
    tgt_menu_add(sm,"About...",102,0,NULL);
    tgt_menu_add(sm,"Is that fun? >>",103,0,os);
    tgt_menu_add(sm,"---------------",102,TGT_MENUF_NONSELECTABLE,NULL);
    tgt_menu_add(sm,"Quit",108,0,NULL);

    tgt_menu_add(pdmptr,"File",100,0,sm);
    cxmptr=sm;


    sm=tgt_menu_context(0);
    tgt_menu_add(sm,"Item1",201,0,NULL);
    tgt_menu_add(sm,"Item2",202,0,NULL);
    tgt_menu_add(pdmptr,"Items",100,0,sm);

    sm=tgt_menu_context(0);
    for(i=0;i<40;i++)
    {
	sprintf(sstring,"Item #%d",i);
	tgt_menu_add(sm,strdup(sstring),300+i,0,NULL);
    }
    tgt_menu_add(pdmptr,"Many items",100,0,sm);


    desktop=tgt_start(NULL,&myterm);
    window=tgt_createandlink(desktop,myterm,TGT_CLASS_WINDOW,(long[]) {TGTT_X,10,TGTT_Y,5,TGTT_XS,60,TGTT_YS,15,TGTT_WINDOW_TITLE,(long) "Menu Class Test",TGTT_CALLBACK,hotkeys,TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_BUTTON,(long[]) {TGTT_X,50,TGTT_Y,12,TGTT_BUTTON_CAPTION,(long) "[Exit]",TGTT_CALLBACK,(long) shutdown,TGTT_END,0});
    menu1=tgt_createandlink(window,myterm,TGT_CLASS_MENU,(long[]) {TGTT_X,1,TGTT_Y,1,TGTT_XS,59,TGTT_CALLBACK,(long) menu,TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_BUTTON,(long[]) {TGTT_X,5,TGTT_Y,6,TGTT_BUTTON_CAPTION,(long) "[Context Menu]",TGTT_CALLBACK,(long) newmenu,TGTT_END,0});
    menu2=tgt_createandlink(window,myterm,TGT_CLASS_MENU,(long[]) {TGTT_X,1,TGTT_Y,1,TGTT_XS,59,TGTT_CALLBACK,(long) menu,TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_CYCLE,(long[]) {TGTT_X,5,TGTT_Y,5,TGTT_CYCLE_ITEMS,(char*[]) { "Item One","Item Two","Item Three",NULL },TGTT_CALLBACK,cyclechange,TGTT_END,0});
    status=tgt_createandlink(window,myterm,TGT_CLASS_LABEL,(long[]) {TGTT_X,5,TGTT_Y,12,TGTT_LABEL_TEXT,"Press <ESC> to enter the menu",TGTT_END,0});
    
    tgt_set(menu1,TGTT_MENU_ITEMS,pdmptr);
    tgt_set(menu2,TGTT_MENU_ITEMS,cxmptr);
    
    
    tgt_refresh(desktop);
    tgt_chtimes(0,100000);
    tgt_waitkeys(desktop);
}
