#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include "tgt.h"
#define max(a,b) ((a) > (b) ? (a) : (b))

static struct termios ourtermios;
static struct termios oldtermios;
static int conswitch_active=0;
int async_semaphore=1;

void tgt_initconswitch(void)
{
/* Uruchamia mozliwosc przelaczania konsoli z trybu normalnego do trybu
ktory nazwalem tutaj 'raw' -> bez echa znakow na konsole i bez czekania
na koniec linii przed przekazaniem danych programowi
*/
    conswitch_active=1;
    /* Tworzymy wlasny termios (struktura opisujaca atrybuty konsoli w linuxie)... */
    /* ... oraz zapisujemy ten oryginalny (normalny) */
    tcgetattr(0,&oldtermios);
    memcpy(&ourtermios,&oldtermios,sizeof(struct termios));
    ourtermios.c_lflag&=!(ECHO|ICANON);
}

long tgt_rawcon(int raw)
{
    if(!conswitch_active) tgt_initconswitch();
    if(raw) 
	tcsetattr(0,TCSANOW,&ourtermios);
    else
	tcsetattr(0,TCSANOW,&oldtermios);
}


int d_seconds=0,d_micros=500000;

void tgt_chtimes(int a,int b)
{
    d_seconds=a;
    d_micros=b;
}
#ifdef TIMEOUT_KEYBOARD
int tgt_fgetc(int s,int us)
{
    struct timeval tv;
    char c;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0,&fds);
    tv.tv_sec=s;
    tv.tv_usec=us;
    if(select(1,&fds,NULL,NULL,&tv)==1)
    {
	read(0,&c,1);
	return((int) c);
    }
    else
	return(-1);
}
#else
int tgt_fgetc(int s,int us)
{
    return(fgetc(stdin));
}
#endif
void tgt_getkey(struct tgt_object *obj)
{
    int c,t,k,i;
    char lastkeys[TGT_MAX_SEQ];
    struct tgt_terminal *term;
    struct tgt_keynode *lt;
    k=0;
    tgt_rawcon(1);
    term=obj->term;
    for(lt=term->lookup_root;;)
    {
        c=tgt_fgetc(d_seconds,d_micros);
        if(c==-1) break;
        lastkeys[k++]=c;
        t=lt[c].type;
        if(t==TGT_KEYN_LOOKUPTABLE) 
    	    lt=(struct tgt_keynode*) lt[c].value;
	else
    	    break;
    }
#ifdef TIMEOUT_KEYBOARD
    if(k==0) return;
#endif
    if(t==TGT_KEYN_KEY)
        tgt_deliver_msg(obj,TGT_OBJECT_HANDLE,(int) lt[c].value,NULL);
    else
    {
        for(i=0;i<k;i++)
    	tgt_deliver_msg(obj,TGT_OBJECT_HANDLE,lastkeys[i],NULL);
    }
}
int tgt_waitkeys(struct tgt_object *obj)
{
    while(1) if(async_semaphore) tgt_getkey(obj); else usleep(d_micros);
}
