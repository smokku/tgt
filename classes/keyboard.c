/* ----------------------------------------------------------------------------
**      keyboard.c  keyboard handling
** ----------------------------------------------------------------------------
**      TGT                                       TextGUIToolkit Library
**      Copyright (C)   2000-2001       Mateusz Golicz and Tomasz Sterna
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

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include "tgt.h"
#ifdef TGT_GPMMOUSE
#include "gpm.h"
#endif
#define max(a,b) ((a) > (b) ? (a) : (b))

static struct termios ourtermios; /* Our console data */
static struct termios oldtermios; /* Console data before we were started */
static int conswitch_active = 0;  /* Did we initialized above ? */
static int mouse_inactive   = 1;  /* Mouse needs initialization */
int async_semaphore = 1;	  /* Noone using getkey() at the moment ? */
static int console_mode = 0;	  /* 1 - NOECHO/NOBUFFER  0 - normal */

/*
 * tgt_initconswitch: Initialize termios structures 
 *
 */

void tgt_initconswitch(void)
{
    conswitch_active = 1; /* We are initialized now */

    tcgetattr(0, &oldtermios); /* Get terminal properties */
    memcpy(&ourtermios,&oldtermios,sizeof(struct termios));
			    /* Copy them to our structure */
    ourtermios.c_lflag&=~(ECHO|ICANON);
			    /* ... and set there NOECHO and no input buffering*/
}

/*
 * tgt_rawcon: Switch console into NOECHO/NOBUFFER mode or into normal mode
 *             that we met on startup.
 */

void tgt_rawcon(int raw)
{
    if(raw == console_mode) return;
    if(! conswitch_active) tgt_initconswitch(); /* The structures haven't been
						    initialized yet */
    if(raw) 
	tcsetattr(0, TCSANOW, &ourtermios);	/* Set our termios */
    else
	tcsetattr(0, TCSANOW, &oldtermios);	/* Set old termios */
	
    console_mode = raw;
}


int d_seconds=0, d_micros=500000;	/* Timeouts for escape */

/*
 * tgt_chtimes: Change escape timeouts
 *
 */

void tgt_chtimes(int a, int b)
{
    d_seconds=a;
    d_micros=b;
}

/*
 * tgt_fgetc: Timeouted getc w/mouse support
 *
 */

#ifdef TIMEOUT_KEYBOARD
int tgt_fgetc(int s,int us)
{
    struct timeval tv;
    unsigned char c;
    fd_set fds;
    
    tv.tv_sec = s;
    tv.tv_usec = us;	/* Set timeouts */
    FD_ZERO(&fds);
    FD_SET(0,&fds);
#ifdef TGT_GPMMOUSE
    if(gpm_fd != -1) FD_SET(gpm_fd,&fds);	/* .. if we have mouse connection*/
    if(select(((gpm_fd==-1) ? 1 : (gpm_fd+1)),&fds,NULL,NULL,&tv) != -1)
    {
	if(gpm_fd!=-1) if(FD_ISSET(gpm_fd,&fds)) return(0); /* 0 -> mouse event */
	if(FD_ISSET(0,&fds))
#else
    if(select(1,&fds,NULL,NULL,&tv) == 1)
#endif

	{
	    read(0,&c,1);	/* Input arrived at stdio */
	    return((int) c);
	}

#ifdef TGT_GPMMOUSE
    }
#endif
    return(-1);	/* -1 -> timeout */
}
#else
int tgt_fgetc(int s,int us)
{
    return(fgetc(stdin));
}
#endif

static int windowmove=0;

/*
 * tgt_getkey - acquire single key sequence from input and pass it to given
 *              object's class function.
 */

void tgt_getkey(struct tgt_object *obj)
{
    int c, t, k, i;
    unsigned char lastkeys[TGT_MAX_SEQ];
    struct tgt_terminal * term;
    struct tgt_keynode  * lt;
#ifdef TGT_GPMMOUSE
    if(mouse_inactive) /* We need mouse intialization ... */
    {
	tgt_init_mouse();
	mouse_inactive=0;
    }
#endif
    k = 0;
    tgt_rawcon(1); /* Switch console to rawmode */
    term = obj->term;
    for(lt = term->lookup_root;;)
    {
        while( ! (c = tgt_fgetc(d_seconds,d_micros)) )
	    tgt_handle_mouse(obj); /* Do until we get something else than mouse events */

        if(c == -1) break; /* Timeout - end acquiring sequence */
        lastkeys[k++] = c; /* Add to received keys table */
        t = lt[c].type;	   /* Check the entry at keyboard-lookup table
			      (see doc/en/KEYBOARD) */
        if(t == TGT_KEYN_LOOKUPTABLE) 
    	    lt=(struct tgt_keynode*) lt[c].value;
	else
    	    break;	/* End of sequence */
    }
#ifdef TIMEOUT_KEYBOARD
    if(k == 0) return;	/* Timed out, return */
#endif
    if(t == TGT_KEYN_KEY) /* Concrete key value */
    {
	int key = (int) lt[c].value;
	
	/* Remap keys if we are at window move mode */
	if(windowmove)
	{
	    switch(key)
	    {
		case TGT_KEY_LEFT: key = TGT_WINDOW_KEY_LEFT;  break;
		case TGT_KEY_RIGHT: key = TGT_WINDOW_KEY_RIGHT; break;
		case TGT_KEY_UP: key = TGT_WINDOW_KEY_UP; break;
		case TGT_KEY_DOWN: key = TGT_WINDOW_KEY_DOWN; break;
	    }
	}
	if(key == TGT_KEY_WINDOWMOVE) windowmove = 1 - windowmove;
	
        tgt_deliver_msg(obj,TGT_OBJECT_HANDLE,key,NULL);
    }
    else
    {
	/* Nothing interesting, deliver all received keycodes */
        for(i=0;i<k;i++)
    	    tgt_deliver_msg(obj,TGT_OBJECT_HANDLE,lastkeys[i],NULL);
    }
}

/*
 * tgt_waitkeys - Wait infinitely for a keypress and deliver it to given
 *                object
 */
int tgt_waitkeys(struct tgt_object *obj)
{
    while(1) 
	if(async_semaphore)
	    tgt_getkey(obj);
	else
	    usleep(d_micros);
}
