#include "tgt.h"
#include <stdio.h>
    struct tgt_object *win,*desk;


void vbcb(struct tgt_object *obj)
{
    tgt_destroyobject(desk);
    printf("Exit, ID : %d\n",obj->id);
    _exit(0);
}

int main(int argc,char **argv)
{
    struct tgt_terminal *term;
     
    desk=tgt_start(NULL,&term);
   
    win=tgt_ac_mkwindow(term,"Test",TGT_AC_CUSTOM,(tagitem[])
      	   {TGT_AC_ALIGNMENT,TGT_AC_BOTTOM,
	    TGT_AC_SPACING,1,
	    TGT_AC_CHILD,TGT_CLASS_BUTTON,
		TGTT_XS,20,
		TGTT_BUTTON_CAPTION, (tgtt) "VButton 1",
		TGTT_CALLBACK,(tgtt) vbcb,
		TGTT_ID,100,
		TGTT_END,0,
	    TGT_AC_CHILD,TGT_CLASS_BUTTON,
		TGTT_XS,20,
		TGTT_BUTTON_CAPTION, (tgtt) "VButton 2",
		TGTT_CALLBACK,(tgtt) vbcb,
		TGTT_ID,101,
		TGTT_END,0,
	    TGT_AC_CHILD,TGT_CLASS_BUTTON,
		TGTT_XS,20,
		TGTT_BUTTON_CAPTION, (tgtt) "VButton 3",
		TGTT_CALLBACK,(tgtt) vbcb,
		TGTT_ID,102,
		TGTT_END,0,
	    TGT_AC_CHILD,TGT_CLASS_BUTTON,
		TGTT_XS,20,
		TGTT_BUTTON_CAPTION, (tgtt) "VButton 4",
		TGTT_CALLBACK,(tgtt) vbcb,
		TGTT_ID,103,
		TGTT_END,0,
	    TGT_AC_CHILD,TGT_AC_HORIZONTALGROUP,
		TGT_AC_CHILD,TGT_CLASS_BUTTON,
		    TGTT_XS,10,
		    TGTT_BUTTON_CAPTION, (tgtt) "HButton 1",
		    TGTT_CALLBACK,(tgtt) vbcb,
		    TGTT_ID,104,
		    TGTT_END,0,
		TGT_AC_CHILD,TGT_CLASS_BUTTON,
		    TGTT_XS,10,
		    TGTT_BUTTON_CAPTION, (tgtt) "HButton 2",
		    TGTT_CALLBACK,(tgtt) vbcb,
		    TGTT_ID,105,
		    TGTT_END,0,
		TGT_AC_CHILD,TGT_AC_VERTICALGROUP,
		    TGT_AC_CHILD,TGT_CLASS_BUTTON,
			TGTT_XS,10,
			TGTT_BUTTON_CAPTION, (tgtt) "HVButton 1",
			TGTT_CALLBACK,(tgtt) vbcb,
			TGTT_ID,106,
			TGTT_END,0,
		    TGT_AC_CHILD,TGT_CLASS_BUTTON,
			TGTT_XS,-20,
			TGTT_BUTTON_CAPTION, (tgtt) "HVButton 2",
			TGTT_CALLBACK,(tgtt) vbcb,
			TGTT_ID,107,
		        TGTT_END,0,
		    TGT_AC_END,0,
		TGT_AC_END,0,
	    TGT_AC_ALIGNMENT,TGT_AC_HCENTER,
	    TGT_AC_CHILD,TGT_CLASS_LABEL,
		TGTT_LABEL_TEXT,(tgtt) "To jest test engine samolayoutujacego",
		TGTT_END,0,
	    TGT_AC_CHILD,TGT_CLASS_STRING,TGTT_END,0,
	    TGT_AC_CHILD,TGT_CLASS_SLIDER,TGTT_END,0,
	    TGT_AC_CHILD,TGT_CLASS_PROGRESS,TGTT_END,0,
	    TGT_AC_END,0},60,75);
	
	tgt_link(win,desk);
	tgt_refresh(desk);
	tgt_waitkeys(desk);
	return(0);
}