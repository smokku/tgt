#include "tgt.h"
#include "tgt_app.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
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
    sid=(void*) tgt_launcher_system(hndlr,cmd);
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

    window=tgt_ac_mkwindow(myterm,"Termemu Test",TGT_AC_HCENTER|TGT_AC_VCENTER,(tagitem[])
		    {TGT_AC_ALIGNMENT,TGT_AC_LEFT,
		     TGT_AC_SPACE,1,
		     TGT_AC_CHILD,TGT_AC_HORIZONTALGROUP,
		         TGT_AC_SPACE,1,
		         TGT_AC_SAVEAS,(tgtt) &str,
		         TGT_AC_CHILD,TGT_CLASS_STRING,
			     TGTT_XS,-60,
			     TGTT_END,0,
			 TGT_AC_CHILD,TGT_CLASS_BUTTON,
			     TGTT_BUTTON_CAPTION,(tgtt) "[Exec]",
			     TGTT_CALLBACK,(tgtt) execute,
			     TGTT_END,0,
			 TGT_AC_END,0,
		     TGT_AC_SPACE,1,
		     TGT_AC_CHILD,TGT_AC_HORIZONTALGROUP,
		         TGT_AC_SPACE,1,
		         TGT_AC_SAVEAS,(tgtt) &buff,
		         TGT_AC_CHILD,TGT_CLASS_TERMEMU,
			     TGTT_XS,-60,
			     TGTT_YS,-60,
			     TGTT_BG,TGT_COLOR_BLACK,
			     TGTT_END,0,
			 TGT_AC_ALIGNMENT,TGT_AC_VCENTER,
			 TGT_AC_CHILD,TGT_CLASS_BUTTON,
			     TGTT_BUTTON_CAPTION,(tgtt) "[Exit]",
			     TGTT_CALLBACK,(tgtt) shutdown,
			     TGTT_END,0,
			 TGT_AC_END,0,
		    TGT_AC_SPACE,1,
		    TGT_AC_END,0});

    tgt_link(window,desktop);
    tgt_attach_launcher(hndlr,buff);

    tgt_refresh(desktop);
    tgt_chtimes(0,100000);
    tgt_waitkeys(desktop);
    return(0);
}
