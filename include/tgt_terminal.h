
#define TGT_TA_BGCOLOR 0
#define TGT_TA_FGCOLOR 1
#define TGT_TA_CLEAR 2
#define TGT_TA_BOLD 3
#define TGT_TA_NORMAL 4
#define TGT_TA_GFX 5
#define TGT_TA_TXT 6
#define TGT_TA_CURSOR 7
#define TGT_TA_DNBORDER 8
#define TGT_TA_TITLEBORDER 9
#define TGT_TA_VBORDER 10
/* operacje mozliwe do wykonania przez funkcje z lowlevel (tgt_chattr())
   na terminalu */


#define TGT_TC_HL 0
#define TGT_TC_VL 1
#define TGT_TC_TS 2
#define TGT_TC_TE 3
#define TGT_TC_UL 4
#define TGT_TC_UR 5
#define TGT_TC_LL 6
#define TGT_TC_LR 7
/* indeksy dla tablicy w strukturze tgt_terminal ktora mapuje charset 
   semigraficzny do charsetu tekstowego HL- horizontal line,
    TS - title Start, UL- upper-left corner LR- lower right corner etc */


struct tgt_terminal
{
    char *termcapname;	/* w praktyce nieuzywane */
    short x_size;
    short y_size; /* jak widac... */
    unsigned char color_fg,color_bg; /* _domyslne_ kolory dla obiektow */
    char gfx_set[8]; /* tablica mapujaca znaki semigraficzne - patrz wyzej i lowlevel.c*/
    char *c_clear;
    char *c_fgcolor;
    char *c_bgcolor;
    char *c_active;
    char *c_inactive;
    char *c_graphics;
    char *c_text;
    char *c_move;   /* stringi do wykonywania operacji na terminalu. na WYLACZNY uzytek tgt_chattr() */
    char *c_cursu;
    char *c_cursd;
    char *c_cursl;
    char *c_cursr;
    char *c_bkspc;
    char *c_del;   /* Stringi do czytania klawiatury - na WYLACZNY uzytek keyboard.c */
    char gchars[10]; /* kolejka nie zanalizowanych jeszcze znakow- patrz keyboard.c*/
    int num; /* ilosc znakow pozostalych w kolejce */
    int pos; /* pozycja w kolejce modulo 10 */
};
