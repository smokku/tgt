#include "tgt.h"
#include "tgt_app.h"
#include <stdio.h>
#include <unistd.h>

    struct tgt_object *desktop;
    struct tgt_object *progr;
/* Callbacks */
void shutdown()
{
    tgt_destroyobject(desktop);
    _exit(0);
}
    char buffer[200];

void slider_move(struct tgt_object *obj,int i)
{
    int max,min,end;
    tgt_get(obj,TGTT_SLIDER_MAXVALUE,(tgtt*) &max);
    tgt_get(obj,TGTT_SLIDER_MINVALUE,(tgtt*) &min);
    tgt_get(progr,TGTT_PROGRESS_ENDVALUE,(tgtt*) &end);
//    fprintf(stderr,"max:%d min:%d end:%d i:%d  wynik:%d\n", max,min,end,i,(i-min) * end / (max - min));
    tgt_set(progr,TGTT_PROGRESS_VALUE, (i - min) * end / (max - min));
    tgt_refresh(progr);
}
int main()
{
	struct tgt_object *window;
	struct tgt_terminal *myterm;

	desktop = tgt_start(NULL);
	myterm = tgt_getterminal();
	
	window = tgt_createandlink(desktop,TGT_CLASS_WINDOW,
	 (tgtt[]) {TGTT_WINDOW_TITLE, (tgtt) "checkbox/slider/progress-bar test",
		   TGTT_XS,39,TGTT_YS,7,
		   TGTT_END,0});
	tgt_set(window,TGTT_X,(myterm->x_size-39)/2);
	tgt_set(window,TGTT_Y,(myterm->y_size-7)/2);
	
	tgt_createandlink(window,TGT_CLASS_BUTTON,
	 (tgtt[]) {TGTT_X,31,TGTT_Y,5,
		   TGTT_BUTTON_CAPTION, (tgtt) "[Exit]",
		   TGTT_CALLBACK, (tgtt) shutdown,
		   TGTT_END,0});
	tgt_createandlink(window,TGT_CLASS_CHECKBOX,
	 (tgtt[]) {TGTT_X,2,TGTT_Y,2,
		   TGTT_CHECKBOX_CAPTION,
		   (tgtt) "One",
		   TGTT_END,0});
	tgt_createandlink(window,TGT_CLASS_CHECKBOX,
	 (tgtt[]) {TGTT_X,12,TGTT_Y,2,
		   TGTT_CHECKBOX_CAPTION,
		   (tgtt) "Two",
		   TGTT_END,0});
	tgt_createandlink(window,TGT_CLASS_SLIDER,
	 (tgtt[]) {TGTT_X,2,TGTT_Y,3,
		   TGTT_XS,30,
		   TGTT_SLIDER_CAPTION,(tgtt) "Slide ",
		   TGTT_SLIDER_MINVALUE,50,
		   TGTT_SLIDER_MAXVALUE,140,
		   TGTT_SLIDER_TYPE,TGT_SLIDERF_SHOWVALUE|TGT_SLIDERF_PERCENT,
		   TGTT_CALLBACK,(tgtt) slider_move,
		   TGTT_END,0});
	progr = tgt_createandlink(window,TGT_CLASS_PROGRESS,
	 (tgtt[]) {TGTT_X,2,TGTT_Y,4,
		   TGTT_XS,20,
		   TGTT_FG,6,
		   TGTT_PROGRESS_VALUE,10,
		   TGTT_PROGRESS_ENDVALUE,50,
		   TGTT_PROGRESS_TYPE,TGT_PROGRESSF_SHOWVALUE,
		   TGTT_END,0});

	tgt_refresh(desktop);
	tgt_chtimes(0,100000);
	tgt_waitkeys(desktop);
	return(1);
}
