/* Tagi (atrybuty w tagliscie) */

/* Do ogolnego uzytku */
#define TGTT_END 0	/* koniec taglisty */

#define TGTT_X 1
/* pozycja X - parsowane przez createobject(), nie musi tego robic handler
   klasy (oczywiscie nie ma przymusu podawania tego, jak nie podamy w tagliscie
   to bedzie tam domyslna wartosc 0 (zreszta klasa wcale nie musi wykorzystywac
   tego pola */

#define TGTT_Y 2	/* pozycja Y - jw */

#define TGTT_XS 3	/* szerokosc - jw */

#define TGTT_YS 4	/* wysokosc - jw */

#define TGTT_FG 5	/* kolory obiektu - jw */
#define TGTT_BG 6

/* callback-zaleznie od klasy... UWAGA: createobject() NIE zczytuje tej wartosci
jesli klasa uzywa callbackow musi SAMA wywolac odpowiednie tgt_gettag() */
#define TGTT_CALLBACK 7

/* parsowane przez createobject, pelna swoboda klasy, patrz tgt_classes.h*/
#define TGTT_ID 8

/* tabele klawiszy przelaczania do poprzedniego/nastepnego dziecka */
#define TGTT_NEXT_KEYS 9
#define TGTT_PREV_KEYS 10



#define TGTT_GENERIC_CAPTION 2000
#define TGTT_GENERIC_ACTIVEBG 2001
#define TGTT_GENERIC_FRAMECOLOR 2002
#define TGTT_GENERIC_MIN 2003
#define TGTT_GENERIC_MAX 2004
#define TGTT_GENERIC_VALUE 2005
