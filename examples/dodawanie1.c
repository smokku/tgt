/* ----------------------------------------------------------------------------
**      dodawanie1.c    TGT Usage Example
** ----------------------------------------------------------------------------
**      TGT                                       TextGUIToolkit Library
**      Copyright (C)   2000-2001   Piotrek Kaczmarek and Mateusz Golicz
**      
**      LGPL license
**      
** ----------------------------------------------------------------------------
**      MAINTAINER      Piotrek Kaczmarek <kaczorek@msg.beta.pl>
**
**      Log:
**      - changed screen messages to english .. ;)
**
*/

/* This code still needs some cleanup */

#include "tgt.h"
#include <stdio.h>
#include <stdlib.h>
       
    struct tgt_object *win,*win2,*desk,*buff,*str,*str2;
    struct tgt_terminal *term;
    void *hndlr;
    unsigned int opened, user_added;

void shutdown(struct tgt_object *obj)
{
    tgt_destroyobject(desk);
    _exit(0);
}

void close_passwd_win(struct tgt_object *obj)
{
    opened=0;
    tgt_launcher_kill(hndlr);
    tgt_unlink(win2);
    tgt_refresh(desk);
}

void add(struct tgt_object *obj)
{
    if (!user_added)
    {
	char *p,*q;
	char buffer[200];
    
	user_added=1;

	tgt_get(str,TGTT_STRING_STRING,(tgtt*) &p);
	tgt_get(str2,TGTT_STRING_STRING,(tgtt*) &q);
	
	sprintf(buffer,"useradd -c \"%s\" %s",q,p);
	system(buffer);
	
	tgt_easyrequest(desk,"Add user","User has been added","[OK]");
    }
}

void passwd(struct tgt_object *obj)
{
    if(!opened && user_added)
    {
	char *p;
	char buffer[100];
    
	opened=1;
	
        tgt_link(win2,desk);
        tgt_refresh(desk);
    
	if(!tgt_hasfocus(win2)) tgt_setfocus(win2);
	
	tgt_get(str,TGTT_STRING_STRING,(tgtt*) &p);
	
	sprintf(buffer,"passwd %s",p);
	tgt_launcher_system(hndlr,buffer);
    }
}

int main(int argc,char **argv)
{
    desk=tgt_start(NULL,&term);
    
    hndlr=tgt_launch_launcher();

    win=tgt_ac_mkwindow(term,"Add user",TGT_AC_HCENTER|TGT_AC_VCENTER,(tgtt[])
	{TGT_AC_SPACE,1,
	TGT_AC_CHILD,TGT_AC_HORIZONTALGROUP,
	    TGT_AC_SPACE,1,
	    TGT_AC_CHILD,TGT_CLASS_LABEL,
	        TGTT_LABEL_TEXT,(tgtt) "Login: ",
	        TGT_AC_END,0,
	    TGT_AC_SAVEAS,(tgtt) &str,
	    TGT_AC_CHILD,TGT_CLASS_STRING,
	        TGTT_XS,45,
	        TGTT_STRING_MAX,250,
	        TGT_AC_END,0,
	TGT_AC_END,0,
	TGT_AC_SPACE,1,
	TGT_AC_CHILD,TGT_AC_HORIZONTALGROUP,
	    TGT_AC_SPACE,1,
	    TGT_AC_CHILD,TGT_CLASS_LABEL,
	        TGTT_LABEL_TEXT,(tgtt) "Name: ",
	        TGT_AC_END,0,
	    TGT_AC_SAVEAS,(tgtt) &str2,
	    TGT_AC_CHILD,TGT_CLASS_STRING,
	        TGTT_XS,46,
	        TGTT_STRING_MAX,250,
	        TGT_AC_END,0,
	    TGT_AC_SPACE,1,
	TGT_AC_END,0,
	TGT_AC_SPACE,1,TGT_AC_ALIGNMENT,TGT_AC_HCENTER,
	TGT_AC_CHILD,TGT_AC_HORIZONTALGROUP,
	    TGT_AC_CHILD,TGT_CLASS_BUTTON,
	        TGTT_BUTTON_CAPTION, (tgtt) "[Add]",
    	        TGTT_CALLBACK,(tgtt) add,
	        TGT_AC_END,0,
	    TGT_AC_SPACE,1,
	    TGT_AC_CHILD,TGT_CLASS_BUTTON,
	        TGTT_BUTTON_CAPTION, (tgtt) "[Passwd]",
    	        TGTT_CALLBACK,(tgtt) passwd,
	        TGT_AC_END,0,
	    TGT_AC_SPACE,1,
	    TGT_AC_CHILD,TGT_CLASS_BUTTON,
	        TGTT_BUTTON_CAPTION, (tgtt) "[Exit]",
    	        TGTT_CALLBACK,(tgtt) shutdown,
	        TGT_AC_END,0,	
	TGT_AC_END,0,
	TGT_AC_SPACE,1,
	TGT_AC_END,0,
	});

    win2=tgt_ac_mkwindow(term,"Change password",TGT_AC_HCENTER|TGT_AC_VCENTER,(tgtt[])
	{TGT_AC_SPACE,1,
	TGT_AC_CHILD,TGT_AC_HORIZONTALGROUP,
	    TGT_AC_SPACE,1,
    	    TGT_AC_SAVEAS,(tgtt) &buff,
    	    TGT_AC_CHILD,TGT_CLASS_TERMEMU,
        	TGTT_XS,-55,
		TGTT_YS,-45,
		TGTT_BG,TGT_COLOR_BLACK,
	    TGT_AC_END,0,
	    TGT_AC_SPACE,1,
	TGT_AC_END,0,
	TGT_AC_SPACE,1,TGT_AC_ALIGNMENT,TGT_AC_HCENTER,
	TGT_AC_CHILD,TGT_CLASS_BUTTON,
	    TGTT_BUTTON_CAPTION,(tgtt) "[Close]",
    	    TGTT_CALLBACK,(tgtt) close_passwd_win,
	    TGTT_END,0,
	TGT_AC_SPACE,1,
	TGT_AC_END,0,
	});

    tgt_attach_launcher(hndlr,buff);

    tgt_link(win,desk);
    tgt_refresh(desk);
    tgt_waitkeys(desk);
    return(0);
}