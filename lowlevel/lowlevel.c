#include <stdio.h>
#ifndef TGT_DLTERMCAP
#include <termcap.h>
#else
#include <dlfcn.h>
#endif
#include <memory.h>
#include <string.h>
#include "tgt.h"
#define TERMSECTIONNAME "terminal"
#ifdef TGT_DLTERMCAP
    char* (*tgetstr)(char *,char*);
    int (*tgetnum)(char *);
    int (*tgetent)(char *,char*);
#endif


void tgt_chattr(struct tgt_terminal *term,int request,int a,char *b)
{
    int i,n;
    char p;
    /* Uniwersalna funkcja zmieniajaca 'atrybuty' terminala, typu
       zmiana kolorow, pozycji kursora, bold/normal ,etc...
       powinna byc uzywana przez handlery klas do rysowania obiektow
       zamiast bezposredniego wpisywania stringow ze struktury
       tgt_terminal */
    switch(request)
    {
	case TGT_TA_BGCOLOR: if(term->bg!=a) { printf(term->c_bgcolor,a); term->bg=a; } return;
	case TGT_TA_FGCOLOR: if(term->fg!=a) { printf(term->c_fgcolor,a); term->fg=a; } return;
	case TGT_TA_CLEAR: printf(term->c_clear); return;
	case TGT_TA_BOLD: printf(term->c_active); return;
	case TGT_TA_NORMAL: printf(term->c_inactive); term->fg=-1; term->bg=-1; return;
	case TGT_TA_GFX: printf(term->c_graphics); return;
	case TGT_TA_TXT: printf(term->c_text); return;
	case TGT_TA_CURSOR: printf(term->c_move,0,(int)b,(int)a); return;
	case TGT_TA_COLORS: 
	    if(term->bg!=(int) b) { printf(term->c_bgcolor,(int) b); term->bg=(char)(int) b; }
	    if(term->fg!=a) { printf(term->c_fgcolor,a); term->fg=a; }
	    return;
    }
}
void tgt_int_lowerb(struct tgt_terminal *term,int l)
{
    int i,n,p;
    /* Dolna ramka. Narozniki + poziome kreski ... */
    tgt_chattr(term,TGT_TA_GFX,0,0);
    putchar(term->gfx_set[TGT_TC_LL]); p=term->gfx_set[TGT_TC_HL];
    for(i=0,n=l-2;i<n;i++) putchar(p);
    putchar(term->gfx_set[TGT_TC_LR]); 
    tgt_chattr(term,TGT_TA_TXT,0,0);
    return;
}
void tgt_int_upperb(struct tgt_terminal *term,int l)
{
    int i,n,p;
    /* Gorna ramka . Narozniki + poziome kreski ... */
    tgt_chattr(term,TGT_TA_GFX,0,0);
    putchar(term->gfx_set[TGT_TC_UL]); p=term->gfx_set[TGT_TC_HL];
    for(i=0,n=l-2;i<n;i++) putchar(p);
    putchar(term->gfx_set[TGT_TC_UR]); 
    tgt_chattr(term,TGT_TA_TXT,0,0);
    return;
}



void tgt_destroyterminal(struct tgt_terminal *tterm)
{
    /* Zwolnienie calej przydzielonej przez nas i termcap pamieci */
    free(tterm->c_graphics); free(tterm->c_text); free(tterm->c_clear);
    free(tterm->c_fgcolor); free(tterm->c_bgcolor); free(tterm->c_active);
    free(tterm->c_inactive); free(tterm->c_move); 
    tgt_destroylookuptable(tterm->lookup_root);
    free(tterm);
}

void tgt_changedefcolors(struct tgt_terminal * term,unsigned char fg,unsigned char bg)
{
/* Zmiana domyslnych kolorow terminala */
    term->color_fg=fg;
    term->color_bg=bg;
}

void tgt_term_addkey(struct tgt_terminal *term,char *capability,char *def,int key)
{
    char *str;
    char kbbuffer[TGT_MAX_SEQ];
/* Dodaje sekwencje zawarta w atrybucie terminala term capability do tablic
   znajdowania znakow wprowadzanych z klawiatury .. Jesli sekwencji nie uda
   sie odczytac, przyjmowana jest domyslna sekwencja def. Wyjscie
   z termcapa moze zawierac jakies niepozadane znaki typu "\","E"
   niwelujemy je (tzn przeksztalcamy w naszym przykladzie do 0x1b)
   za pomoca snprintf() */
   
    str=tgetstr(capability,NULL);
    if(str==NULL)
    	snprintf(kbbuffer,TGT_MAX_SEQ-1,def);
    else
    {
	snprintf(kbbuffer,TGT_MAX_SEQ-1,str);
	free(str);
    }
    tgt_addkeyseq(term->lookup_root,kbbuffer,key);
}
    extern void * g_prefs;

void tgt_term_addkey_int(int te,struct tgt_terminal *term,char *capability,char *def,char *conf_name,int key)
{
    char *str;
    char *pstr;
    char kbbuffer[TGT_MAX_SEQ];
/* Dodaje sekwencje zawarta w atrybucie terminala term capability do tablic
   znajdowania znakow wprowadzanych z klawiatury .. Jesli sekwencji nie uda
   sie odczytac, przyjmowana jest domyslna sekwencja def. Wyjscie
   z termcapa moze zawierac jakies niepozadane znaki typu "\","E"
   niwelujemy je (tzn przeksztalcamy w naszym przykladzie do 0x1b)
   za pomoca snprintf() */
    str=NULL;
    if(pstr=tgt_getprefs(g_prefs,TERMSECTIONNAME,conf_name,NULL))
	def=pstr;
    else
	if(te) str=tgetstr(capability,NULL);

    if(!str)
    	snprintf(kbbuffer,TGT_MAX_SEQ-1,def);
    else
    {
	snprintf(kbbuffer,TGT_MAX_SEQ-1,str);
	free(str);
    }
    tgt_addkeyseq(term->lookup_root,kbbuffer,key);
}

char *tgt_tgetstrd(int te,char *name,char *def,char *conf_name)
{
    char *ret;
/* tgt_tgetstr() pobiera atrybut z config-file, jesli nie to termcapa , a jesli
   i to sie nie powiedzie, przyjmuje def (default) */
    if(ret=tgt_getprefs(g_prefs,TERMSECTIONNAME,conf_name,NULL)) return(strdup(ret));
    if(te && !ret) ret=tgetstr(name,NULL);
    if(!ret) ret=strdup(def);
    return(ret);
}



struct tgt_terminal * tgt_setscreen(char *name)
{
    int te,n;
    char *str;
    char *buffer;
    struct tgt_terminal *ret;
    char kbbuffer[TGT_MAX_SEQ];
/*

Funkcja pobiera z termcapa parametry terminala podanego jako argument name
(ew. NULL jak ma odczytac ze zmiennej $TERM), kapsulkuje ja do wlasnej struktury,
(tgt_terminal -> tgt_terminal.h), na ktora uprzednio przydziela pamiec, 
i zwraca wskaznik na stworzona strukture aplikacji

*/    
#ifdef TGT_DLTERMCAP
    void *dlh;
    dlh=dlopen("libtermcap.so",RTLD_LAZY);
    if(dlh)
    {
	tgetent=(int(*)(char *,char*)) dlsym(dlh,"tgetent");
	tgetnum=(int(*)(char *)) dlsym(dlh,"tgetnum");
	tgetstr=(char*(*)(char *,char*)) dlsym(dlh,"tgetstr");
	if(!tgetent || !tgetnum || !tgetstr)
	{
	    dlclose(dlh);
	    dlh=NULL;
	}
    }
#endif


/* Ewentualny odczyt zmiennej srodowiskowej */

    if(name==NULL) name=(char*) getenv("TERM");

/* Otwarcie wpisu dla naszego terminala */
    buffer=(char*) malloc(2048);  /* wymagane przez POSIX */
    te=0;
#ifdef TGT_DLTERMCAP
    if(dlh) if(name) te=tgetent(buffer,name);
#else
    if(name) te=tgetent(buffer,name);
#endif

    if(te!=1)
    {
	free(buffer);
	printf("Termcap library or termcap entry NOT FOUND\n");
	printf("Attempting to use config file directives or defaults\n");
	usleep(2000000);
	te=0;
    }
/* Przydzielamy pamiec... */
    ret=(struct tgt_terminal *) malloc(sizeof(struct tgt_terminal));

/* domyslne kolory terminala: szary na niebieskim */
    ret->color_bg=4; ret->color_fg=7;

/* ilosc kolumn i linii*/
    if(te)
    {
	ret->x_size=tgetnum("co");
	ret->y_size=tgetnum("li");
    }
    else
    {
	ret->x_size=80; ret->y_size=25;
    }

    if(n=atoi(tgt_getprefs(g_prefs,TERMSECTIONNAME,"columns","0"))) ret->x_size=n;
    if(n=atoi(tgt_getprefs(g_prefs,TERMSECTIONNAME,"lines","0"))) ret->y_size=n;
    
    ret->c_clear=tgt_tgetstrd(te,"cl","\033[H\033[J","clear");
    ret->c_move=tgt_tgetstrd(te,"cm","\033[%i%d;%dH","move");

/* tgt_tgetstr() w razie braku atrybutu klonuje nam pusty string, zeby
   sie nie wykraszowalo tgt_chattr */

    ret->c_graphics=tgt_tgetstrd(te,"as","\x0e","gfx_start");
    ret->c_text=tgt_tgetstrd(te,"ae","\x0f","gfx_end");

/* zmiana kolorow napisu i tla, BOLD-jasniejsze kolory (active) i NORMAL (inactive)*/
    ret->c_fgcolor=tgt_tgetstrd(te,"AF","\033[3%dm","ch_fg");
    ret->c_bgcolor=tgt_tgetstrd(te,"AB","\033[4%dm","ch_bg");
    ret->c_active=tgt_tgetstrd(te,"md","\033[1m","bold");
    ret->c_inactive=tgt_tgetstrd(te,"me","\033[0m","normal");

/* Stworzenie tablic (struktury drzewa) przeszukujacych klawiature...
   W sumie dosc kosztowna pamieciowo zabawa... Jedna taka tablica
   to 2048 bajtow, aktualnie, w przypadku terminala standartowego
   linux-t tworzone jest ok. :) 3 tablic czyli 6 kB */
   
    ret->lookup_root=tgt_initroottable();
    tgt_term_addkey_int(te,ret,"ku","\033[A","c_up",TGT_KEY_UP);
    tgt_term_addkey_int(te,ret,"kd","\033[B","c_down",TGT_KEY_DOWN);
    tgt_term_addkey_int(te,ret,"kl","\033[D","c_left",TGT_KEY_LEFT);
    tgt_term_addkey_int(te,ret,"kr","\033[C","c_right",TGT_KEY_RIGHT);
    tgt_term_addkey_int(te,ret,"kD","\x1b\x5b\x33\x7e","delete",TGT_KEY_DELETE);
    tgt_term_addkey_int(te,ret,"kb","\x7f","backspace",TGT_KEY_BKSPC);
    tgt_term_addkey_int(te,ret,"kI","\x1b\x5b\x32\x7e","insert",TGT_KEY_INSERT);
    tgt_term_addkey_int(te,ret,"kh","\x1b\x5b\x31\x7e","home",TGT_KEY_HOME);
    tgt_term_addkey_int(te,ret,"@7","\x1b\x5b\x34\x7e","end",TGT_KEY_END);

    memcpy(ret->gfx_set,tgt_getprefs(g_prefs,TERMSECTIONNAME,"gfx","qxutlkmj"),8);
    /* Odpowiedniki znakow semigraficznych. W trybie graficznym napisanie
     'q' powoduje wyswietlenie linii poziomej, 'j' to dolny prawy naroznik
     ramki etc. Patrz stale w tgt_terminal.h */
    ret->fg=-1; ret->bg=-1;
#ifdef TGT_DLTERMCAP
    if(dlh) dlclose(dlh);
#endif
    return(ret);
}
