#include <stdio.h>
#include "tgt.h"

    static int int_initialized=0; /*Inicjowalismy juz tablice wewnetrznych klas?*/
    void *internal_classes[100]; /* owa tablica... */

void tgt_init_intclasses()
{
    /* ehh.. narazie cosik malo ... ;) */
    internal_classes[TGT_CLASS_DESKTOP]=tgt_builtin_desktop;
    internal_classes[TGT_CLASS_WINDOW]=tgt_builtin_window;
    internal_classes[TGT_CLASS_BUTTON]=tgt_builtin_button;
    int_initialized=1;    
}

long tgt_gettag(long *taglist,long stag,long defaultvalue)
{
    long tag,value;
    /* Przeszukuje tagliste taglist w poszukiwaniu tagu stag i zwraca
      jego wartosc. Jesli tag nie zostanie znaleziony, funkcja zwraca
      defaultvalue */
      
    if(taglist==NULL) return(defaultvalue);
    for(;;)
    {
	tag=*(taglist++); value=*(taglist++);
	if(tag==0) return(defaultvalue); /* koniec taglisty */
	if(stag==tag) return(value); /* tag ktorego szukamy */
    }
}

void tgt_link(struct tgt_object *obj,struct tgt_object *parent)
{
    struct tgt_object *oldch;
    struct tgt_object *ach;
    /* laczy podrzednie obiekt obj z obiektem parent */
    obj->parent=parent;  /* naszym rodzicem jest od tej chwili parent ... */
    oldch=parent->children; /* stare dziecko bedace pierwsze na liscie */
    obj->next=oldch; /* ... teraz bedzie drugie :) */
    obj->prev=NULL;  /* ... a my pierwsi */
    if(oldch!=NULL) oldch->prev=obj; 
    parent->children=obj; /* no i wpisujemy sie na hamca jako poczatek listy */
}
void tgt_unlink(struct tgt_object *obj)
{
    struct tgt_object *next;
    /* odlacza obiekt obj */
    next=obj->next;
    if(next!=NULL) next->prev=obj->prev;
    if(obj->prev!=NULL) obj->prev->next=obj->next;
    if(obj->parent!=NULL)
    {
	if(obj->parent->children==obj) /* czy aby nie bylismy poczatkiem listy ?? */
	    obj->parent->children=next;
    }
}


struct tgt_object * tgt_createobject(struct tgt_terminal *term,
				 int (*classf)(struct tgt_object*,int,int,void*),
				 long *taglist)
{
    struct tgt_object *ret;
    /* Tworzy obiekt dla klasy o handlerze classf istniejacy w terminalu term,
       z atrybutami takimi jak podane w tagliscie */

    /* Przydzielamy pamiec... */    
    ret=(struct tgt_object*) malloc(sizeof(struct tgt_object));

    ret->term=term; ret->classf=classf;
    /* Ustawiamy sobie pare najczesciej uzywanych i majacych wlasne pola w 
       strukturze tagow */
    ret->x=tgt_gettag(taglist,TTGT_X,0); ret->y=tgt_gettag(taglist,TTGT_Y,0);
    ret->xs=tgt_gettag(taglist,TTGT_XS,0); ret->ys=tgt_gettag(taglist,TTGT_YS,0);
    ret->fg=tgt_gettag(taglist,TTGT_FG,term->color_fg); ret->bg=tgt_gettag(taglist,TTGT_BG,term->color_bg);
    ret->id=tgt_gettag(taglist,TTGT_ID,0);

    ret->parent=NULL; ret->children=NULL;
    ret->next=NULL; ret->prev=NULL;
    ret->active=0; /* Obiekt nie ma na poczatku fokusa */
    /* no i teraz dajemy klasie mozliwosc stworzenia swoich wlasnych
       strktur opisujacych obiekt-np. tak jak jest to opisane w tagliscie.
       Zwrocenie zera oznacza brak jakiegos niezbednego tagu czy tam
       jakies inne niepowodzenie ... */
    if(classf!=NULL) if(classf(ret,TGT_OBJECT_CREATE,0,taglist)==0) { free(ret); return(NULL); }
    return(ret);
}

struct tgt_object * tgt_getdesktop(struct tgt_terminal * term)
{
    /* Front-end dla tgt_createobject() tworzacy nadrzedny obiekt desktopu dla terminala term */
    return(tgt_createobject(term,tgt_builtin_desktop,(long[]) { TTGT_X,1, TTGT_Y,1, TTGT_END,0} ));
}
void tgt_destroyobject(struct tgt_object *obj)
{
    struct tgt_object *children;
    struct tgt_object *newch;
    /* Likwiduje obiekt wywolujac odpowiednia metode z klasy po czym zwalniajac
       przeznaczona dla niego pamiec. Procedura wywoluje siebie rekursywnie w
       celu usuniecia rowniez dzieci obiektu*/
       
    for(children=obj->children;children!=NULL;)
    {
	newch=children->next;
	tgt_destroyobject(children);
	children=newch;
    }
    obj->classf(obj,TGT_OBJECT_DESTROY,0,0);
    free(obj);
    return;
}
struct tgt_object *tgt_createandlinkobject(struct tgt_object *parent,struct tgt_terminal *term,
				 int (*classf)(struct tgt_object*,int,int,void*),
				 long *taglist)
{
/* Front-end dla tgt_createobject() -> tworzy obiekt i linkuje go od razu
   do obiektu parent */

    struct tgt_object *ret;
    ret=tgt_createobject(term,classf,taglist);
    if(ret==NULL) return(NULL);
    tgt_link(ret,parent);
    return(ret);
}
struct tgt_object * tgt_createobject_int(struct tgt_terminal *term,
				 int classid,
				 long *taglist)
{
    /* jw, front-end, tworzy obiekt wykorzystujac jako handler klasy
       jedna z wbudowanych w system klas */
    if(int_initialized==0) tgt_init_intclasses();
    return(tgt_createobject(term,internal_classes[classid],taglist));
}

struct tgt_object * tgt_createandlink_int(struct tgt_object *parent,struct tgt_terminal *term,
				 int classid,
				 long *taglist)
{
    /* Kombinacja obu powyzszych */
    struct tgt_object *ret;
    if(int_initialized==0) tgt_init_intclasses();
    ret=tgt_createobject(term,internal_classes[classid],taglist);
    if(ret==NULL) return(NULL);
    tgt_link(ret,parent);
    return(ret);
}