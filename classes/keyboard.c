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

/* ... no i teraz w zaleznosci od potrzeb ustawiamy odpowiednie termiosy */

long tgt_rawcon(void)
{
    if(conswitch_active!=0)
    {
	tcsetattr(0,TCSANOW,&ourtermios);
	conswitch_active=2;
    }
    else
	tgt_initconswitch();
}
void tgt_normalcon(void)
{
    if(conswitch_active!=0)
    {
	tcsetattr(0,TCSANOW,&oldtermios);
	conswitch_active=1;
    }
    else
	tgt_initconswitch();
}
/* Pobiera klawisz z terminala tm ... UGLY !!! BARDZO UGLY !!! */

int tgt_get_key(struct tgt_terminal *tm)
{
    char c;
    char gchars[10];
    int n,k,i;
/*
Ta procedurka to prowizorka w pelnym znaczeniu tego slowa... nadaje
sie do rewrite'a... nie bede jej opisywal krok po kroku moze lepiej
opisze ogolny sposob dzialania...

Caly problem polega na tym ze niektore sekwencje znakowe zajmuja wiecej
niz 1 znak... a my w jednym cyklu fgetc() mozemy odczytac tylko jeden
znak, i tylko jeden znak musimy zwrocic na wyjsciu.. Mozna by oczywiscie
przepuszczac wszystko 'jak leci' i sprawdzac za kazdym razem czy nie bylo
gdzies tam po drodze znaku sterujacego- ale jesli np. obiekt stringa odnotowal
juz nam klawisze '\E[1A' (kursor w gore) to co mamy robic ? kazac mu je kasowac?
nonsens... No wiec moj pomysl jest taki:

Jesli mamy cos w kolejce terminala (zaraz sie okaze po co ona tak wogole)
to bierzemy znak do analizy stamtad... Jesli nie-ze stdin...
Jesli pobrany znak nie przypomina nam zadnego kodu sterujacego-nima problemu,
zwracamy go ... Jesli jednak-pobieramy kolejno znaki (z kolejki lub z stdin)
dopoki nie odczytamy ktoregos z kodow w calosci (-> wtedy wysylamy jako
jeden int kod tego znaku,np TGT_KEY_LEFT) badz tez nie odczytamy tyle
znakow, ile liczyl sobie najdluzszy kod sposrod tych ktorych poczatki rozpoznalismy,
wtedy mamy Problem(TM) ... I w tym momencie przydaje sie kolejka-dopisujemy
do niej nasze znaki i zwracamy pierwszy ktory odczytalismy (powinien byc
to kolejny nastepny znak ogolnie w kolejce-to jest do poprawienia:)
... reszta zajmiemy sie w nastepnym cyklu dzialania procedury...
*/
    k=-1;
    sprintf(gchars,"         ");
    if(tm->num>0) 
	{ tm->num--; tm->pos++; c=tm->gchars[tm->pos %10]; }
    else
         c=fgetc(stdin); 
    
    gchars[0]=c;

    if(c==tm->c_cursl[0]) k=max(k,strlen(tm->c_cursl));
    if(c==tm->c_cursr[0]) k=max(k,strlen(tm->c_cursr));
    if(c==tm->c_cursu[0]) k=max(k,strlen(tm->c_cursu));
    if(c==tm->c_cursd[0]) k=max(k,strlen(tm->c_cursd));
    if(c==tm->c_bkspc[0]) k=max(k,strlen(tm->c_bkspc));
    if(c==tm->c_del[0]) k=max(k,strlen(tm->c_del));

    if(k==-1) return(c);    

    
    for(i=1;i<(k+1);i++)
    {
	if(tm->num>0) 
	    { tm->num--; tm->pos++; c=tm->gchars[tm->pos%10]; }
        else
    	    c=fgetc(stdin); 
        gchars[i]=c;
	if(memcmp(gchars,tm->c_cursl,strlen(tm->c_cursl))==0) return(TGT_KEY_LEFT);
	if(memcmp(gchars,tm->c_cursr,strlen(tm->c_cursr))==0) return(TGT_KEY_RIGHT);
	if(memcmp(gchars,tm->c_cursu,strlen(tm->c_cursu))==0) return(TGT_KEY_UP);
	if(memcmp(gchars,tm->c_cursd,strlen(tm->c_cursd))==0) return(TGT_KEY_DOWN);
	if(memcmp(gchars,tm->c_del,strlen(tm->c_del))==0) return(TGT_KEY_DELETE);
	if(memcmp(gchars,tm->c_bkspc,strlen(tm->c_bkspc))==0) return(TGT_KEY_BKSPC);
    }
    if(k>1)
    {
	for(i=1;i<=k;i++) tm->gchars[(i+tm->pos+tm->num) % 10]=gchars[i];
	tm->num+=k;
//	tm->pos=0;
    }
    return(c);
}
