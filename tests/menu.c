#include "tgt.h"
#include "tgt_app.h"
#include "tgt_menu.h"
#include <stdio.h>
#include <string.h>

    struct tgt_object *desktop;
    struct tgt_terminal *myterm;
    struct tgt_object *menu;
    struct tgt_object *status;
/* Callbacks */
void shutdown()
{
    tgt_destroyobject(desktop);
    _exit(0);
}
void cyclechange(struct tgt_object *obj,int id)
{
    char buffer[100];
    sprintf(buffer,"User selected cycle position, nr %d  \n",id);
    tgt_set(status,TGTT_LABEL_TEXT,(tgtt) buffer);
    tgt_refresh(status);
}

void hotkeys(struct tgt_object *obj,int k)
{
    if(k==TGT_KEY_ESC) 
    tgt_menu_enable(menu,5,0);
}

int main()
{
    struct tgt_object *window;
    struct tgt_object *submenu;
    

    desktop=tgt_start(NULL,&myterm);
    window=tgt_createandlink(desktop,TGT_CLASS_WINDOW,(tagitem[]) {TGTT_X,10,TGTT_Y,5,TGTT_XS,60,TGTT_YS,15,TGTT_WINDOW_TITLE,(tgtt) "Menu Class Test",TGTT_CALLBACK,(tgtt) hotkeys,TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,50,TGTT_Y,12,TGTT_BUTTON_CAPTION,(tgtt) "[Exit]",TGTT_CALLBACK, (tgtt) shutdown,TGTT_END,0});
//    tgt_createandlink(window,TGT_CLASS_BUTTON,(tagitem[]) {TGTT_X,5,TGTT_Y,6,TGTT_BUTTON_CAPTION,(tgtt) "[Context Menu]",TGTT_CALLBACK,(tgtt) newmenu,TGTT_END,0});
//    menu2=tgt_createandlink(window,TGT_CLASS_MENU,(tagitem[]) {TGTT_X,1,TGTT_Y,1,TGTT_XS,59,TGTT_CALLBACK, (tgtt) menu,TGTT_END,0});
    tgt_createandlink(window,TGT_CLASS_SELECTBOX,(tagitem[]) {TGTT_X,5,TGTT_Y,5,TGTT_XS,20,TGTT_SELECTBOX_ITEMS,(tgtt) (char*[]) { "Item One","Item Two","Item Three",NULL },TGTT_CALLBACK,(tgtt) cyclechange,TGTT_END,0});
    status=tgt_createandlink(window,TGT_CLASS_LABEL,(tagitem[]) {TGTT_X,5,TGTT_Y,12,TGTT_LABEL_TEXT,(tgtt) "Press <ESC> to enter the menu",TGTT_END,0});
    
    submenu=tgt_menu_getcontext(myterm);
    tgt_menu_add(submenu,"Submenu Item Number One",0,NULL);
    tgt_menu_add(submenu,NULL,0,NULL);
    tgt_menu_add(submenu,"Submenu Nonselectable item",1,NULL);
    tgt_menu_add(submenu,"Submenu Item Number Two",0,NULL);
    
    
    menu=tgt_menu_getcontext(myterm);
    tgt_menu_add(menu,"Main menu position one",0,NULL);
    tgt_menu_add(menu,NULL,0,NULL);
    tgt_menu_add(menu,"Main menu position two",0,NULL);
    tgt_menu_add(menu,"Main menu position three",0,NULL);
    tgt_menu_add(menu,NULL,0,NULL);
    tgt_menu_add(menu,"Submenu gateway       >>",0,submenu);
    tgt_menu_add(menu,NULL,0,NULL);
    tgt_menu_add(menu,"Main menu position four",0,NULL);

    tgt_link(menu,window);
    
    tgt_refresh(desktop);
    tgt_chtimes(0,100000);
    tgt_waitkeys(desktop);
    return(0);
}
