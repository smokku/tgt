
/* Kody "klawiszy specjalnych" */

#define TGT_KEY_UP -1
#define TGT_KEY_DOWN -2
#define TGT_KEY_LEFT -3
#define TGT_KEY_RIGHT -4
#define TGT_KEY_BKSPC -5
#define TGT_KEY_DELETE -6
#define TGT_KEY_INSERT -7
#define TGT_KEY_HOME -8
#define TGT_KEY_END -9


/* hihi magiczna liczba ;)) */
#define TGT_KEY_NONE -65536

struct tgt_keynode
{
    int type;
    void *value;
};

/* Element key-lookup table... Tablica sklada sie z 256 elementow, osobny dla
   kazdego znaku... Jesli po wprowadzeniu danego znaku ma byc bezposrednio
   przekazywana jakas wiadomosc o klawiszu do obiektu, to type dla
   odpowiedniego elementu tablicy wynosi TGT_KEYN_KEY, a value to kod
   tego znaku przekazywany do obiektu (np. 'a' albo TGT_KEY_DOWN) ...
   Jesli po wprowadzeniu tegoz znaku tgt_waitkeys() ma pobrac kolejny znak to
   type=TGT_KEYN_LOOKUPTABLE a value wskazuje na lookup-table ktora
   bedzie sluzyc do analizy tego nastepnego znaku.... Jesli wprowadzona
   kombinacja (znak) jest niepoprawna to type=TGT_KEYN_NONE.. Wtedy
   tgt_keywait() przesyla po kolei kody ascii wszystkich wprowadzonych
   znakow od poczatku lookup-u ... */

#define TGT_KEYN_LOOKUPTABLE 0
#define TGT_KEYN_KEY 1
#define TGT_KEYN_NONE 2

#define TGT_MAX_SEQ 20
