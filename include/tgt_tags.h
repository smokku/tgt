/* Tagi (atrybuty w tagliscie) */

/* Do ogolnego uzytku */
#define TTGT_END 0
/* koniec taglisty */
#define TTGT_X 1
/* pozycja X - parsowane przez createobject(), nie musi tego robic handler
   klasy (oczywiscie nie ma przymusu podawania tego, jak nie podamy w tagliscie
   to bedzie tam domyslna wartosc 0 (zreszta klasa wcale nie musi wykorzystywac
   tego pola */
#define TTGT_Y 2
/* pozycja Y - jw */
#define TTGT_XS 3
/* szerokosc - jw */
#define TTGT_YS 4
/* wysokosc - jw */
#define TTGT_FG 5
#define TTGT_BG 6
/* kolory obiektu - jw */
#define TTGT_CALLBACK 7
/* callback-zaleznie od klasy... UWAGA: createobject() NIE zczytuje tej wartosci
jesli klasa uzywa callbackow musi SAMA wywolac odpowiednie tgt_gettag() */
#define TTGT_ID 8
/* parsowane przez createobject, pelna swoboda klasy, patrz tgt_classes.h*/

#define TTGT_NEXT_KEYS 9
#define TTGT_PREV_KEYS 10

/* tabele klawiszy przelaczania do poprzedniego/nastepnego dziecka */


/* Wlasne tagi klasy window .. nazwy chyba mowia same za siebie ...
   oczywiscie nie ma obowiazku podawania */
#define TTGT_WINDOW_TITLE 1000
#define TTGT_WINDOW_BORDERCOLOR 1001
#define TTGT_WINDOW_TITLECOLOR 1002

/* Wlasne tagi button. MUSI zostac podany TTGT_BUTTON_CAPTION */
#define TTGT_BUTTON_CAPTION 1000
#define TTGT_BUTTON_ACTIVEBG 1001
/*kolor tla w momencie kiedy przycisk otrzymuje fokus */

#define TTGT_LABEL_TEXT 1000



/* Metody wlasne klas */


