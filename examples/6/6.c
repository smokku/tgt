/* ----------------------------------------------------------------------------
**      6.c    TGT Usage Example
** ----------------------------------------------------------------------------
**      TGT                                       TextGUIToolkit Library
**      Copyright (C)   2000-2001       Tomasz Sterna and Mateusz Golicz
**
**      LGPL license
**
** ----------------------------------------------------------------------------
**      MAINTAINER      Mateusz Golicz
**
**      Log:
**      
**
*/

#include "tgt.h"
#include "tgt_acmacros.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define CMD_CHANGETEMPO 1
#define CMD_QUIT 2

struct tgt_object * desktop;

int tempo = 5; /* x 100 000 us */

    /* this is called every change of the slider value */
int slidercb(struct tgt_object * obj, int level)
{
    tempo = level;
    return(1);
}

    /* this is called when Quit is hit */
int haltcb(struct tgt_object * obj, int k)
{
    tgt_destroyobject(desktop);
    exit(0);
    return(1);
}

int main(int argc, char ** argv)
{
    struct tgt_object * window;
    int counter = 0;
    unsigned char lbuff[30];
    
    desktop = tgt_start(NULL);
    window = tgt_ac_mkwindow("Asynchronous", TGT_AC_HCENTER|TGT_AC_VCENTER, (tgtt[]) {
					AcSpace(1),
					AcChild, SimpleSliderC("Delay ", 1, 5, 20, slidercb),
					AcChild, AcHorizGroup,
					    AcChild, SimpleLabel("Count: "),
					    AcChild, SimpleLabelI("0", 1001),
					    AcEnd,
					AcChild, SimpleButtonC("Quit", haltcb),
					AcSpace(1),
					AcEnd});	/* create a window */

    tgt_link(window, desktop);
    tgt_refresh(desktop);
    tgt_async_waitkeys(desktop); 		/* wait for keys in background thread */
    
    for(;;)					/* and do some operations in foreground */
    {
	usleep(tempo * 100000);
	snprintf(lbuff, 30, "%d", ++counter);
	tgt_set(tgt_findchild(desktop, 1001), TGTT_LABEL_TEXT, (tgtt) lbuff);
	tgt_refresh(tgt_findchild(desktop, 1001));
	if(counter % 64 == 63) 
	/* this function displays a fully-featured dialog box and blocks until user answers
	   returned value is the selected button number */
	    if(tgt_easyrequest(desktop, "Counter", "We reached a number which gives 63 modulo 64.","Continue|Cancel") == 1)
		haltcb(NULL, 0);	    
    }	
}
