#include <stdio.h>
#include <termcap.h>
#include <memory.h>
#include <string.h>
#include "tgt.h"


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
char *tgt_tgetstr(char *name)
{
    char *ret;
/* tgt_tgetstr() pobiera atrybut z termcapa , a w razie braku atrybutu klonuje
   nam pusty string, zeby sie nie wykraszowalo tgt_chattr */
    ret=tgetstr(name,NULL);
    if(ret==NULL) ret=strdup("");
    return(ret);
}
char *tgt_tgetstrd(char *name,char *def)
{
    char *ret;
/* tgt_tgetstr() pobiera atrybut z termcapa , lub def (default) */
    ret=tgetstr(name,NULL);
    if(ret==NULL) ret=strdup(def);
    return(ret);
}

struct tgt_terminal * tgt_setscreen(char *name)
{
    int te;
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


/* Ewentualny odczyt zmiennej srodowiskowej */

    if(name==NULL) name=(char*) getenv("TERM");
    if(name==NULL) return(NULL);

/* Otwarcie wpisu dla naszego terminala */
    
    buffer=(char*) malloc(2048);  /* wymagane przez POSIX */
    te=tgetent(buffer,name);
    if(te!=1) { free(buffer); return(NULL); }

/* Przydzielamy pamiec... */
    ret=(struct tgt_terminal *) malloc(sizeof(struct tgt_terminal));

/* domyslne kolory terminala: szary na niebieskim */
    ret->color_bg=4; ret->color_fg=7;

/* ilosc kolumn i linii*/
    ret->x_size=tgetnum("co");
    ret->y_size=tgetnum("li");

/*
    Kody dla kasowania ekranu i przenoszenia kursora. Bez nich sie NIE OBEJDZIEMY,
    bez kolorkow, bez grafiki to bedzie jeszcze jakos wygladac (krzaki ale bedzie
    dalo sie *cos* chociaz zrobic), natomiast nie bedac w stanie przenosic
    kursora nie jestesmy w stanie rysowac wszystkiego tam gdzie trzeba...
*/    
    if((ret->c_clear=tgetstr("cl",NULL))==NULL) { free(ret); return(NULL); }
    if((ret->c_move=tgetstr("cm",NULL))==NULL) { free(ret->c_clear); free(ret); return(NULL); }

/* tgt_tgetstr() w razie braku atrybutu klonuje nam pusty string, zeby
   sie nie wykraszowalo tgt_chattr */

    ret->c_graphics=tgt_tgetstrd("as","\x0e");
    ret->c_text=tgt_tgetstrd("ae","\x0f");

/* zmiana kolorow napisu i tla, BOLD-jasniejsze kolory (active) i NORMAL (inactive)*/
    ret->c_fgcolor=tgt_tgetstrd("AF","\033[3%dm");
    ret->c_bgcolor=tgt_tgetstrd("AB","\033[4%dm");
    ret->c_active=tgt_tgetstrd("md","\033[1m");
    ret->c_inactive=tgt_tgetstrd("me","\033[0m");

/* Stworzenie tablic (struktury drzewa) przeszukujacych klawiature...
   W sumie dosc kosztowna pamieciowo zabawa... Jedna taka tablica
   to 2048 bajtow, aktualnie, w przypadku terminala standartowego
   linux-t tworzone jest ok. :) 3 tablic czyli 6 kB */
   
    ret->lookup_root=tgt_initroottable();
    tgt_term_addkey(ret,"ku","\033[A",TGT_KEY_UP);
    tgt_term_addkey(ret,"kd","\033[B",TGT_KEY_DOWN);
    tgt_term_addkey(ret,"kl","\033[D",TGT_KEY_LEFT);
    tgt_term_addkey(ret,"kr","\033[C",TGT_KEY_RIGHT);
    tgt_term_addkey(ret,"kD","\x1b\x5b\x33\x7e",TGT_KEY_DELETE);
    tgt_term_addkey(ret,"kb","\x7f",TGT_KEY_BKSPC);
    tgt_term_addkey(ret,"kI","\x1b\x5b\x32\x7e",TGT_KEY_INSERT);
    tgt_term_addkey(ret,"kh","\x1b\x5b\x31\x7e",TGT_KEY_HOME);
    tgt_term_addkey(ret,"@7","\x1b\x5b\x34\x7e",TGT_KEY_END);

    memcpy(ret->gfx_set,"qxutlkmj",8);
    /* Odpowiedniki znakow semigraficznych. W trybie graficznym napisanie
     'q' powoduje wyswietlenie linii poziomej, 'j' to dolny prawy naroznik
     ramki etc. Patrz stale w tgt_terminal.h */
    ret->fg=-1; ret->bg=-1;
    return(ret);
}
