#include <stdio.h>
#include <termcap.h>
#include <memory.h>
#include <string.h>
#include "tgt.h"

void tgt_drawtitleb(struct tgt_terminal *term,int a,char *b,int fg,int bg,int tit)
{
    int i,n,p;
    /* Rysuje 'belke tytulowa' okna */
    printf(term->c_graphics); /* Tryb graficzny ... */
    printf(term->c_fgcolor,fg); printf(term->c_bgcolor,bg); /* Kolorki ... */
    putchar(term->gfx_set[TGT_TC_UL]); p=term->gfx_set[TGT_TC_HL]; 
    /* naroznik i znaczek "przekreconego T" do rozpoczecia tytulu... */
    
    putchar(term->gfx_set[TGT_TC_TS]); printf(term->c_text);
    printf(term->c_fgcolor,tit);
    printf(" %s ",b);
    /* Tytul... */
    
    printf(term->c_fgcolor,fg);
    printf(term->c_graphics); putchar(term->gfx_set[TGT_TC_TE]); /* znak "przekreconego w druga strone T :)) */
    for(i=0,n=a-6-strlen(b);i<n;i++) putchar(p); /* cala reszta oprocz 2 znakow na narozniki + 2 na 'T' + 2 na odstepy i dlugosci
    stringa tytulowego to poziome kreski */
    putchar(term->gfx_set[TGT_TC_UR]); /* naroznik */
    return;
}
void tgt_drawlowerb(struct tgt_terminal *term,int l)
{
    int i,n,p;
    /* Dolna ramka okna . Narozniki + poziome kreski ... */
    putchar(term->gfx_set[TGT_TC_LL]); p=term->gfx_set[TGT_TC_HL];
    for(i=0,n=l-2;i<n;i++) putchar(p);
    putchar(term->gfx_set[TGT_TC_LR]); 
    return;
}
void tgt_drawsideb(struct tgt_terminal *term,int x,int y,int l,int lxs)
{
    char *tmpbuff;
    int i;
    /* Rysuje boczna ramke okna poczynajac od pozycji x,y o ilosci
       kolumn lxs i wierszy l */
    tmpbuff=(char*) malloc(term->x_size+1);
    memset(tmpbuff,' ',term->x_size);
    tmpbuff[0]=term->gfx_set[TGT_TC_VL];  /* pionowa linia */
    tmpbuff[lxs-1]=term->gfx_set[TGT_TC_VL];
    tmpbuff[lxs]=0;
    /* Tworzymy sobie string pomocniczy , cos a'la "|         |" */
    
    
    for(i=0;i<l;i++)
    {
	/* No i teraz przemieszczamy kursor corazto nizej i wypluwamy
	   nasz string */
	printf(term->c_move,0,y+i,x);
	printf(tmpbuff);
    }
    /* Zwalniamy pomocniczy string */
    free(tmpbuff);
}


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
	case TGT_TA_BGCOLOR: printf(term->c_bgcolor,a); return;
	case TGT_TA_FGCOLOR: printf(term->c_fgcolor,a); return;
	case TGT_TA_CLEAR: printf(term->c_clear); return;
	case TGT_TA_BOLD: printf(term->c_active); return;
	case TGT_TA_NORMAL: printf(term->c_inactive); return;
	case TGT_TA_GFX: printf(term->c_graphics); return;
	case TGT_TA_TXT: printf(term->c_text); return;
	case TGT_TA_CURSOR: printf(term->c_move,0,(int)b,(int)a); return;
    }
}
void tgt_destroyterminal(struct tgt_terminal *tterm)
{
    /* Zwolnienie calej przydzielonej przez nas i termcap pamieci */
    free(tterm->c_graphics); free(tterm->c_text); free(tterm->c_clear);
    free(tterm->c_fgcolor); free(tterm->c_bgcolor); free(tterm->c_active);
    free(tterm->c_inactive); free(tterm->c_move); free(tterm->c_cursu);
    free(tterm->c_cursd); free(tterm->c_cursl); free(tterm->c_cursr);
    free(tterm->c_del); free(tterm->c_bkspc);
    free(tterm);
}

void tgt_changedefcolors(struct tgt_terminal * term,unsigned char fg,unsigned char bg)
{
/* Zmiana domyslnych kolorow terminala */
    term->color_fg=fg;
    term->color_bg=bg;
}

struct tgt_terminal * tgt_setscreen(char *name)
{
    int te;
    char *str;
    char *buffer;
    struct tgt_terminal *ret;
/*

Funkcja pobiera z termcapa parametry terminala podanego jako argument name
(ew. NULL) jak ma odczytac ze zmiennej $TERM, kapsulkuje ja do wlasnej struktury,
(tgt_terminal -> tgt_terminal.h), na ktora uprzednio przydziela pamiec, 
i zwraca wskaznik na stworzona strukture aplikacji

TODO: sprawdzanie czy atrybuty nie sa aby rowne NULL i przypisywanie jakichs
difoltowych wartosci

*/    

    if(name==NULL) name=(char*) getenv("TERM");
    if(name==NULL) return(NULL);

/* Otwarcie wpisu dla naszego terminala */
    
    buffer=(char*) malloc(2048);  /* wymagane przez POSIX */
    te=tgetent(buffer,name);
    if(te!=1) { free(buffer); return(NULL); }

    ret=(struct tgt_terminal *) malloc(sizeof(struct tgt_terminal));
    /* domyslne kolory terminala: szary na niebieskim */
    ret->color_bg=4; ret->color_fg=7;
    /* ilosc kolumn i linii*/
    ret->x_size=tgetnum("co");
    ret->y_size=tgetnum("li");
    /* znaki sterujace: zmiana trybu semigrafika/text, czyszczenie ekranu*/
    ret->c_graphics=tgetstr("as",NULL);
    ret->c_text=tgetstr("ae",NULL);
    ret->c_clear=tgetstr("cl",NULL);

    /* zmiana kolorow napisu i tla, BOLD-jasniejsze kolory (active)
       i NORMAL (inactive)*/
    ret->c_fgcolor=tgetstr("AF",NULL);
    ret->c_bgcolor=tgetstr("AB",NULL);
    ret->c_active=tgetstr("md",NULL);
    ret->c_inactive=tgetstr("me",NULL);
    /* Przesuniecie kursora do zadanej pozycji */
    ret->c_move=tgetstr("cm",NULL);
    /* 
	Dla obslugi klawiatury: kursor w gore/w dol/w lewo/w prawo
	Caly ten cyrk z sprintf() robimy dlatego, ze wyjscie z termcapa
	moze zaierac jakies sekwencje sterujace, np \E , a my chcemy porownywac
	to z tym co zczytamy z klawiatury (czyli np. 0x1B w przypadku \E)
    */
    
    str=tgetstr("ku",NULL); ret->c_cursu=(char*) malloc(strlen(str)+1); sprintf(ret->c_cursu,str); free(str);
    str=tgetstr("kd",NULL); ret->c_cursd=(char*) malloc(strlen(str)+1); sprintf(ret->c_cursd,str); free(str);
    str=tgetstr("kl",NULL); ret->c_cursl=(char*) malloc(strlen(str)+1); sprintf(ret->c_cursl,str); free(str);
    str=tgetstr("kr",NULL); ret->c_cursr=(char*) malloc(strlen(str)+1); sprintf(ret->c_cursr,str); free(str);    

/* To samo dla delete i bkspc*/

    str=tgetstr("dc",NULL);
    if(str==NULL) str=strdup("\x1b\x5b\x33\x7e");
    /* Tak u mnie (Slack 7.1) wyglada delete :) "\E[3~" */
    ret->c_del=(char*) malloc(strlen(str)+1); sprintf(ret->c_del,str); free(str); 

    str=tgetstr("bc",NULL); 
    if(str==NULL) str=strdup("\010"); /* czyli ^H */
    ret->c_bkspc=(char*) malloc(strlen(str)+1); sprintf(ret->c_bkspc,str); free(str); 

    memcpy(ret->gfx_set,"qxutlkmj",8);
    /* Odpowiedniki znakow semigraficznych. W trybie graficznym napisanie
     'q' powoduje wyswietlenie linii poziomej, 'j' to dolny prawy naroznik
     ramki etc. Patrz stale w tgt_terminal.h */

    if(ret->c_clear!=NULL) printf(ret->c_clear);
    /* To do obslugi kolejki klawiatury */
    ret->num=0;
    ret->pos=0;
    return(ret);
}
