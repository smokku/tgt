#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "tgt.h"
#define max(a,b) ((a) > (b) ? (a) : (b))

static struct termios ourtermios;
static struct termios oldtermios;
static int conswitch_active=0;

void tgt_initconswitch(void)
{
/* Uruchamia mozliwosc przelaczania konsoli z trybu normalnego do trybu
ktory nazwalem tutaj 'raw' -> bez echa znakow na konsole i bez czekania
na koniec linii przed przekazaniem danych programowi
*/
    conswitch_active=1;
    /* Tworzymy wlasny termios (struktura opisujaca atrybuty konsoli w linuxie)... */
    ourtermios.c_iflag=0;
    ourtermios.c_oflag=0;
    ourtermios.c_cflag=0;
    ourtermios.c_lflag=0;
    cfmakeraw(&ourtermios);
    /* ... oraz zapisujemy ten oryginalny (normalny) */
    tcgetattr(0,&oldtermios);
}

long tgt_rawcon(int raw)
{
    if(!conswitch_active) tgt_initconswitch();
    if(raw) 
	tcsetattr(0,TCSANOW,&ourtermios);
    else
	tcsetattr(0,TCSANOW,&oldtermios);
}

int tgt_waitkeys(struct tgt_object *obj)
{
    int c,t,k,i;
    char lastkeys[TGT_MAX_SEQ];
    struct tgt_terminal *term;
    struct tgt_keynode *lt;
    tgt_rawcon(1);
    term=obj->term;
    for(;;)
    {
	k=0;
	for(lt=term->lookup_root;;)
	{
	    c=fgetc(stdin);
	    lastkeys[k++]=c;
	    t=lt[c].type;
	    if(t==TGT_KEYN_LOOKUPTABLE) 
		lt=(struct tgt_keynode*) lt[c].value;
	    else
		break;
	}
	if(t==TGT_KEYN_KEY)
	    tgt_deliver_msg(obj,TGT_OBJECT_HANDLE,(int) lt[c].value,NULL);
	else
	{
	    for(i=0;i<k;i++)
		tgt_deliver_msg(obj,TGT_OBJECT_HANDLE,lastkeys[i],NULL);
	}
    }
}
