
struct tgt_listnode
{
    struct tgt_object *parent;
    /* Rodzic. Wypelniany przy linkowaniu */
    struct tgt_object *next;
    struct tgt_object *prev;
    /* Pointery do polaczenia z innymi dziecmi rodzica */
    struct tgt_object *child;	/* (Aktywne) dziecko, lub NULL */
};

struct tgt_object
{
    struct tgt_listnode ln;
    struct tgt_terminal *term;
    /* Terminal w jakim 'istnieje' obiekt */
    int (*classf) (struct tgt_object *,int,int,void*);
    /* Funkcja obslugi obiektu, vel 'handler klasy' 
       parametry: classf(obiekt, metoda (zadanie)->patrz nizej(stale), argument1, argument2)*/
    int (*objectf) ();
    /* Callback-argumenty i momenty w ktorych jest on wywolywany-zaleznie od
       klasy */
    short x,y,xs,ys; /* rozmiary obiektu */
    unsigned char fg,bg; 
    /* ... i kolory -> w tych 6 polach rowniez prawie pelna dowolnosc klasy .. 
	WYJATTEK: x i y sa uzywane przy przeliczaniu wspolrzednych dzieci
	(podaje sie je wzgledem wspolrzednych rodzica) - MUSZA miec sensowne
	wartosci o ile obiekt ma dzieci */

    int id;	/* Pelna dowolnosc klasy. Typowe zastosowanie: przekazanie
    callbackowi tak aby 1 callback mogl obsluzyc >1 obiekt */
    void *class_data;
    /* Totalna dowolnosc klasy */
    void *user_data;
    /* Totalna dowolnosc klasy */
/*    int active; */
/* Nie istnieje od v 0.07 */
    unsigned char objflags;
/* Pole bitowe... patrz nizej... */
    int *prev_keys;	/* Tablica klawiszy przelaczajacych do nastepnego dziecka */
    int *next_keys;	/* Tablica klawiszy przelaczajacych do poprzedniego dziecka */
};


/*
Metody klas (rzadania (wiem wiem ze przez z z kropka ale wtedy nie 
widac sensu :) 

zakladajac ze handler klasy to
int class(struct object * obj, int req, int arg1, void* arg2);
to jesli parametr req to odpowiednio:

*/
#define TGT_OBJECT_CREATE 1
/* Wypelnij pola ktore masz zamiar uzywac w strukturze obj. Jesli
   Ci to potrzebne-jako arg 2 masz dostarczona tagliste ktora mozesz
   odczytac np. przez tgt_gettag()*/

#define TGT_OBJECT_DESTROY 2
/* Zwolnij wszelkie zasoby systemowe jakie przydzieliles przy
   Wypelnaniu wlasnych pol (TGT_OBJECT_CREATE) */
   
#define TGT_OBJECT_REFRESH 3
/* Odswierz obiekt. Za punkt odniesienia przyjmij (arg1, (int) arg2) */

#define TGT_OBJECT_HANDLE 4
/* Potrafisz zorzumiec klawisz arg1? 
"Szur. Zrobilem co trzeba (np. dodalem kolejny znak do bufora, wywolalem
 callback uzytkownika,etc." = 1
"eee niezbyt, zapytaj sie moich dzieci" = 0
*/

#define TGT_OBJECT_SETTAG 5
#define TGT_OBJECT_GETTAG 6

#define TGT_OBJECT_SETDEFAULTS 7

/* Pole objflags */
#define TGT_OBJFLAGS_NONSELECTABLE 1
/* aktywna -> obiekt nie moze zostac zaznaczony */
#define TGT_OBJFLAGS_INFORMFIRST 2
/* Jesli aktywna, to obiekt informowany jest _przed_ rodzicami o zdarzeniach
(np. context-menu) , jesli zero to informacja przekazywana
jest _po_ odpytaniu rodzicow o to czy rozumieja
    wiadomosc */
/* UWAGA!!! Ta flaga moze byc ustawiona JEDYNIE w obiektach ktore nie beda 
   mialy dzieci (ktore moglyby zostac zaktywizowane) ... inaczej zostanie
   zignorowana */



/* Identyfikatory klas wewnetrznych */
#define TGT_CLASS_DESKTOP 0
#define TGT_CLASS_WINDOW 1
#define TGT_CLASS_BUTTON 2
#define TGT_CLASS_LABEL 3
#define TGT_CLASS_STRING 4
#define TGT_CLASS_LIST 5
#define TGT_CLASS_CHECKBOX 6
#define TGT_CLASS_SLIDER 7
#define TGT_CLASS_PROGRESS 8

#define TGT_CLASS_MENU 29
#define TGT_CLASS_CYCLE 30
