/* Tagi (atrybuty w tagliscie) */

/* Do ogolnego uzytku */
#define TGTT_END 0
/* koniec taglisty */
#define TGTT_X 1
/* pozycja X - parsowane przez createobject(), nie musi tego robic handler
   klasy (oczywiscie nie ma przymusu podawania tego, jak nie podamy w tagliscie
   to bedzie tam domyslna wartosc 0 (zreszta klasa wcale nie musi wykorzystywac
   tego pola */
#define TGTT_Y 2
/* pozycja Y - jw */
#define TGTT_XS 3
/* szerokosc - jw */
#define TGTT_YS 4
/* wysokosc - jw */
#define TGTT_FG 5
#define TGTT_BG 6
/* kolory obiektu - jw */
#define TGTT_CALLBACK 7
/* callback-zaleznie od klasy... UWAGA: createobject() NIE zczytuje tej wartosci
jesli klasa uzywa callbackow musi SAMA wywolac odpowiednie tgt_gettag() */
#define TGTT_ID 8
/* parsowane przez createobject, pelna swoboda klasy, patrz tgt_classes.h*/

#define TGTT_NEXT_KEYS 9
#define TGTT_PREV_KEYS 10

/* tabele klawiszy przelaczania do poprzedniego/nastepnego dziecka */


/* Wlasne tagi klasy window .. nazwy chyba mowia same za siebie ...
   oczywiscie nie ma obowiazku podawania */
#define TGTT_WINDOW_TITLE 1000
#define TGTT_WINDOW_BORDERCOLOR 1001
#define TGTT_WINDOW_TITLECOLOR 1002

/* Wlasne tagi button. MUSI zostac podany TTGT_BUTTON_CAPTION */
#define TGTT_BUTTON_CAPTION 1000
#define TGTT_BUTTON_ACTIVEBG 1001
/*kolor tla w momencie kiedy przycisk otrzymuje fokus */

#define TGTT_LABEL_TEXT 1000



/* Metody wlasne klas */


