#include <unistd.h>
#include <stdio.h>
#ifndef TGT_DLTERMCAP
#include <termcap.h>
#else
#include <dlfcn.h>
#endif
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include "tgt.h"
#include <stdlib.h>
#define TERMSECTIONNAME "terminal"
#ifdef TGT_DLTERMCAP
    char* (*tgetstr)(char *,char*);
    int (*tgetnum)(char *);
    int (*tgetent)(char *,char*);
    char* (*tparam)(const char *,char *,int, ...);
    int (*tputs)(char *,int,int(*)(int));
#endif
#define PRINTF_BUFFER_LENGTH 512

static void *termcap_dlh;

int tgt_preferred_x=-1,tgt_preferred_y=-1;

static inline void tgt_out_param1(char * fstring,int param)
{
    char tbuffer[40];
    char *buf;
#ifdef TGT_DLTERMCAP
    if(!termcap_dlh) 
	printf(fstring,param);
    else
    {
#endif
	buf=tparam(fstring,tbuffer,40,param);
	tputs(buf,1,putchar);
	if(buf!=tbuffer) free(buf);
#ifdef TGT_DLTERMCAP
    }
#endif
}
static inline void tgt_out_param2(char * fstring,int param1,int param2)
{
    char tbuffer[40];
    char *buf;
#ifdef TGT_DLTERMCAP
    if(!termcap_dlh) 
	printf(fstring,param1,param2);
    else
    {
#endif
	buf=tparam(fstring,tbuffer,40,param1,param2);
	tputs(buf,1,putchar);
	if(buf!=tbuffer) free(buf);
#ifdef TGT_DLTERMCAP
    }
#endif
}

#ifndef TGT_DLTERMCAP
#define tgt_tputs(a,b,c) tputs(a,b,c)
#else
inline static void tgt_tputs(char *string,int l,int (*out)(int))
{
    if(termcap_dlh) tputs(string,l,out); else printf(string);
}
#endif

void tgt_chattr(struct tgt_terminal *term,int request,int a,int b)
{
    /* Uniwersalna funkcja zmieniajaca 'atrybuty' terminala, typu
       zmiana kolorow, pozycji kursora, bold/normal ,etc...
       powinna byc uzywana przez handlery klas do rysowania obiektow
       zamiast bezposredniego wpisywania stringow ze struktury
       tgt_terminal */
    switch(request)
    {
	case TGT_TA_BGCOLOR: if(term->bg!=a) { tgt_out_param1(term->c_bgcolor,a); term->bg=a; } return;
	case TGT_TA_FGCOLOR: if(term->fg!=a) { tgt_out_param1(term->c_fgcolor,a); term->fg=a; } return;
	case TGT_TA_CLEAR: tgt_tputs(term->c_clear,1,putchar); return;
	case TGT_TA_BOLD: tgt_tputs(term->c_active,1,putchar); return;
	case TGT_TA_NORMAL: tgt_tputs(term->c_inactive,1,putchar); term->fg=-1; term->bg=-1; return;
	case TGT_TA_GFX: tgt_tputs(term->c_graphics,1,putchar); return;
	case TGT_TA_TXT: tgt_tputs(term->c_text,1,putchar); return;
	case TGT_TA_VISIBLECURSOR: tgt_tputs(term->c_enablecurs,1,putchar); return;
	case TGT_TA_INVISIBLECURSOR: tgt_tputs(term->c_disablecurs,1,putchar); return;
	case TGT_TA_CURSOR: tgt_out_param2(term->c_move,b,a); return;
	case TGT_TA_COLORS: 
	    if(term->bg!=b) { tgt_out_param1(term->c_bgcolor,b); term->bg=(char) b; }
	    if(term->fg!=a) { tgt_out_param1(term->c_fgcolor,a); term->fg=(char) a; }
	    return;
    }
}
void tgt_int_lowerb(tgt_cell * buffer,int l,tgt_cell attr)
{
    int i;
    tgt_cell p;
    *(buffer++)=TGT_T_FCHAR(attr,TGT_TC_LL);
    l-=2; p=TGT_T_FCHAR(attr,TGT_TC_HL);
    for(i=0;i<l;i++) *(buffer++)=p;
    *(buffer++)=TGT_T_FCHAR(attr,TGT_TC_LR);
    return;
}
void tgt_int_upperb(tgt_cell * buffer,int l,tgt_cell attr)
{
    int i;
    tgt_cell p;
    *(buffer++)=TGT_T_FCHAR(attr,TGT_TC_UL);
    l-=2; p=TGT_T_FCHAR(attr,TGT_TC_HL);
    for(i=0;i<l;i++) *(buffer++)=p;
    *(buffer++)=TGT_T_FCHAR(attr,TGT_TC_UR);
    return;
}

void tgt_fill_buffer(tgt_cell * buffer,int xs,int ys,tgt_cell cell)
{
    register int i,m;
    m=xs*ys;
    for(i=0;i<m;i++)
	*(buffer++)=cell;
}

void tgt_change_colors(tgt_cell * buffer,int length,int fg,int bg,int bold)
{
    register int i;
    tgt_cell cell=TGT_T_BOLD(TGT_T_FG(TGT_T_BG(0,bg),fg),bold);
    for(i=0;i<length;i++)
	*(buffer++)=(*buffer & 0xff)|cell;
}

void tgt_blit(tgt_cell * dest,int dxs,tgt_cell *src,int xs,int ys,int xp,int yp)
{
    register int dxsd,i;
    dest+=yp*dxs+xp;
    dxsd=xs << 1;
    for(i=0;i<ys;i++)
    {
	memcpy(dest,src,dxsd);
	dest+=dxs;
	src+=xs;
    }
}

static unsigned char tgt_printf_buffer[PRINTF_BUFFER_LENGTH+1];

int tgt_printf(tgt_cell *buffer,int max,tgt_cell attr,char *format,...)
{
    int n,i;
    va_list vl;
    va_start(vl,format);
    n=vsnprintf(tgt_printf_buffer,max+1,format,vl);
    for(i=0;i<n;i++) *(buffer++)=TGT_T_FCHAR(attr,tgt_printf_buffer[i]);
    va_end(vl);
    return(n);
}
void tgt_flprintf(tgt_cell *buffer,int max,tgt_cell attr,char *format,...)
{
    int n,i;
    va_list vl;
    va_start(vl,format);
    n=vsnprintf(tgt_printf_buffer,max+1,format,vl);
    for(i=0;i<n;i++) *(buffer++)=TGT_T_FCHAR(attr,tgt_printf_buffer[i]);
    n=max-n;
    if(n>0) for(i=0;i<n;i++) *(buffer++)=TGT_T_FCHAR(attr,' ');
    va_end(vl);
}

void tgt_update_screen(struct tgt_terminal * term)
{
    tgt_cell * buffer=term->terminal_buffer;
    tgt_cell * screen=term->contents_buffer;
    int y,ys=term->y_size,linelength=term->x_size << 1,xs=term->x_size;
    int x,xl,inc;
    int bold,fg,bg;
    int c_gfx=-1,c_bold=-1,c_fg=-1,c_bg=-1;
    tgt_cell cell;
    int c;    
    
    for(y=0;y<ys;y++)
    {
	if(memcmp(buffer,screen,linelength))
	{
	    for(x=0;;x++) if(buffer[x]!=screen[x]) break; /* First different character */
	    for(xl=xs-1;;xl--) if(buffer[xl]!=screen[xl]) break;
					    /* Last different character */
	    xl-=x; buffer+=x; screen+=x;
	    tgt_chattr(term,TGT_TA_CURSOR,x,y);
	    inc=xs-x-xl-1;
	    fflush(stderr);
	    for(x=0;x<=xl;x++)
	    {
		cell=*(buffer++);
		*(screen++)=cell;

		if((bold=TGT_T_GETBOLD(cell)) != c_bold)
		{
		    if((c_bold=bold))
			tgt_chattr(term,TGT_TA_BOLD,0,0);
		    else
		    {
			tgt_chattr(term,TGT_TA_NORMAL,0,0);
			c_fg=-1; c_bg=-1;
		    }
		}

		if((fg=TGT_T_GETFG(cell)) != c_fg) tgt_chattr(term,TGT_TA_FGCOLOR,c_fg=fg,0);
		if((bg=TGT_T_GETBG(cell)) != c_bg) tgt_chattr(term,TGT_TA_BGCOLOR,c_bg=bg,0);

		c=TGT_T_GETCHAR(cell);
		
		if(c<32)
		{
		    if(c<8)
		    {
			if(c_gfx!=1)
			{
			    c_gfx=1;
			    tgt_chattr(term,TGT_TA_GFX,0,0);
			}
//			putchar('!');
			putchar(term->gfx_set[c]);
		    }
		    else
			putchar(' ');
		}
		else
		{
		    if(c_gfx)
		    {
			tgt_chattr(term,TGT_TA_TXT,0,0);
			c_gfx=0;
		    }
		    putchar(c);
		}
	    }
	    buffer+=inc;
	    screen+=inc;
	}
	else
	{
	    buffer+=term->x_size;
	    screen+=term->x_size;
	}
    }
    if(tgt_preferred_x!=-1)
    {
	tgt_chattr(term,TGT_TA_CURSOR,tgt_preferred_x,tgt_preferred_y);
	tgt_preferred_x=-1;
    }
    fflush(stdout);
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
    if((pstr=tgt_getprefs(g_prefs,TERMSECTIONNAME,conf_name,NULL)))
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
    if((ret=tgt_getprefs(g_prefs,TERMSECTIONNAME,conf_name,NULL))) return(strdup(ret));
    if(te && !ret) ret=tgetstr(name,NULL);
    if(!ret) ret=strdup(def);
    return(ret);
}



struct tgt_terminal * tgt_setscreen(char *name)
{
    int te=0,n;
    char *buffer,*sb;
    struct tgt_terminal *ret;
//    char kbbuffer[TGT_MAX_SEQ];
/*

Funkcja pobiera z termcapa parametry terminala podanego jako argument name
(ew. NULL jak ma odczytac ze zmiennej $TERM), kapsulkuje ja do wlasnej struktury,
(tgt_terminal -> tgt_terminal.h), na ktora uprzednio przydziela pamiec, 
i zwraca wskaznik na stworzona strukture aplikacji

*/    
#ifdef TGT_DLTERMCAP
    void *dlh;
    dlh=dlopen("libtermcap.so",RTLD_LAZY);
    if(!dlh) dlh=dlopen("libtermcap.so.2",RTLD_LAZY);
    if(dlh)
    {
	tgetent=(int(*)(char *,char*)) dlsym(dlh,"tgetent");
	tgetnum=(int(*)(char *)) dlsym(dlh,"tgetnum");
	tgetstr=(char*(*)(char *,char*)) dlsym(dlh,"tgetstr");
	tparam=(char*(*)(const char *,char*,int,...)) dlsym(dlh,"tparam");
	tputs=(int(*)(char *,int,int(*)(int))) dlsym(dlh,"tputs");
	if(!tgetent || !tgetnum || !tgetstr || !tputs)
	{
	    dlclose(dlh);
	    dlh=NULL;
	}
	if(!tparam)
	{
	    dlclose(dlh);
	    dlh=NULL;
	    printf("You DO NOT HAVE a REAL termcap, your libtermcap.so is just a ncurses termcap replacement!\n");
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
	if(!tgt_getprefs(g_prefs,TERMSECTIONNAME,"ignore_termcap_type",NULL))
	{
	    printf("Press ENTER to continue");
	    getchar();
	}
	te=0;
    }
/* Przydzielamy pamiec... */
    ret=(struct tgt_terminal *) malloc(sizeof(struct tgt_terminal));

/* domyslne kolory terminala: szary na niebieskim */
    ret->color_bg=4; ret->color_fg=7;

/* ilosc kolumn i linii*/

    if((sb=getenv("COLUMNS")))
        ret->x_size=atoi(sb);
    else
        if(te) ret->x_size=tgetnum("co"); else ret->x_size=80;

    if((sb=getenv("LINES")))
        ret->y_size=atoi(sb);
    else
        if(te) ret->y_size=tgetnum("li"); else ret->y_size=25;


    if((n=atoi(tgt_getprefs(g_prefs,TERMSECTIONNAME,"columns","0")))) ret->x_size=n;
    if((n=atoi(tgt_getprefs(g_prefs,TERMSECTIONNAME,"lines","0")))) ret->y_size=n;
    
    ret->c_clear=tgt_tgetstrd(te,"cl","\033[H\033[J","clear");
    ret->c_move=tgt_tgetstrd(te,"cm","\033[%d;%dH","move");

/* tgt_tgetstr() w razie braku atrybutu klonuje nam pusty string, zeby
   sie nie wykraszowalo tgt_chattr */

    ret->c_graphics=tgt_tgetstrd(te,"as","\x0e","gfx_start");
    ret->c_text=tgt_tgetstrd(te,"ae","\x0f","gfx_end");

/* zmiana kolorow napisu i tla, BOLD-jasniejsze kolory (active) i NORMAL (inactive)*/
    ret->c_fgcolor=tgt_tgetstrd(te,"AF","\033[3%dm","ch_fg");
    ret->c_bgcolor=tgt_tgetstrd(te,"AB","\033[4%dm","ch_bg");
    ret->c_active=tgt_tgetstrd(te,"md","\033[1m","bold");
    ret->c_inactive=tgt_tgetstrd(te,"me","\033[0m","normal");
    ret->c_enablecurs=tgt_tgetstrd(te,"ve","\033[?25h","cursenable");
    ret->c_disablecurs=tgt_tgetstrd(te,"vi","\033[?25l","cursdisable");


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
    tgt_term_addkey_int(te,ret,"kP","\x1b\x5b\x35\x7e","pgup",TGT_KEY_PGUP);
    tgt_term_addkey_int(te,ret,"kN","\x1b\x5b\x36\x7e","pgdn",TGT_KEY_PGDN);
    tgt_term_addkey_int(te,ret,"WM","\x17","windowmove",TGT_KEY_WINDOWMOVE);

    memcpy(ret->gfx_set,tgt_getprefs(g_prefs,TERMSECTIONNAME,"gfx","qxutlkmj"),8);
    /* Odpowiedniki znakow semigraficznych. W trybie graficznym napisanie
     'q' powoduje wyswietlenie linii poziomej, 'j' to dolny prawy naroznik
     ramki etc. Patrz stale w tgt_terminal.h */
    ret->fg=-1; ret->bg=-1;

    ret->terminal_buffer=(tgt_cell*) malloc(sizeof(tgt_cell)*ret->x_size*ret->y_size);
    ret->contents_buffer=(tgt_cell*) malloc(sizeof(tgt_cell)*ret->x_size*ret->y_size);

    tgt_fill_buffer(ret->terminal_buffer,ret->x_size,ret->y_size,TGT_T_BUILDCELL(ret->color_fg,ret->color_bg,0,0,' '));
    tgt_fill_buffer(ret->contents_buffer,ret->x_size,ret->y_size,0);

#ifdef TGT_DLTERMCAP
//    if(dlh) dlclose(dlh);
    termcap_dlh=dlh;
#endif
    return(ret);
}

void tgt_fatal_error(struct tgt_terminal * term,char *string)
{
    if(term)
    {
	tgt_chattr(term,TGT_TA_TXT,0,0);
	tgt_chattr(term,TGT_TA_NORMAL,0,0);
	tgt_chattr(term,TGT_TA_CLEAR,0,0);
    }
    else
	printf("\033[0m\x0f\033[H\033[J");
	
    fflush(stdout);
    fprintf(stderr,"tgt: Fatal error: %s\n",string);
    fflush(stderr);
    tgt_rawcon(0);
    exit(0);
}
