#include <stdio.h>
#include "tgt.h"

int tgt_shalliswitch(struct tgt_object *obj,int key,int pri)
{
    struct tgt_object *parent;
    int *nk;
    int *pk;
    int i;
    /* Czy klawisz key nie zawiera sie aby w jednej z tablic klawiszy
      przelaczajacych dzieci rodzica obiektu obj (lub w pierwszych ich
      elementach jesli pri == 1) ? -1 : tak, w prev_keys,
      1: tak, w next_keys 0 : nie
    */
    if(!(parent=obj->ln.parent)) return(0);
    if(pri)
    {
	if(nk=parent->next_keys) if(nk[0]==key) return(1);
	if(pk=parent->next_keys) if(pk[0]==key) return(-1);
    }
    else
    {
	if(nk=parent->next_keys) for(i=0;nk[i]!=0;i++) if(nk[i]==key) return(1);
	if(pk=parent->prev_keys) for(i=0;pk[i]!=0;i++) if(pk[i]==key) return(-1);
    }
    return(0);
}

int tgt_intrefresh(struct tgt_object *obj,int a,int b)
{
    struct tgt_object *children;
    struct tgt_object *active;
    /* Wewnetrzna procedura odswierzania: odswierz obiekt obj,
       przeliczajac jego wspolrzedne wzgledem a i b (prosciej:
       po prostu dodajac je do nich :) po czym wywolaj siebie
       dla dzieci obiektu*/
    /* Oczywiscie 'brudna robote' wykonuje handler klasy... */
    obj->classf(obj,TGT_OBJECT_REFRESH,a,(void*) b);
    active=obj->ln.active_child;
    a+=obj->x; b+=obj->y;  /* Dodajemy nasze wspolrzedne-dzieci obiektu
    (np przyciski w oknie) w koncu maja ustawione wspolrzedne wzgledem
    naszych (czyli np . lewego gornego rogu okna) */
    for(children=obj->ln.first_child;children!=NULL;children=children->ln.next)
	if(active!=children) tgt_intrefresh(children,a,b);

    if(active!=NULL) tgt_intrefresh(active,a,b); /* no i aktywny */
}
int tgt_refresh(struct tgt_object *obj)
{
    int x,y;
    struct tgt_object *parent;
    /* Znajdz absolutne wspolrzedne wzgledem ktorych ma zostac narysowany
       obiekt obj i poddaj go odswierzaniu */
    x=0; y=0;
    for(parent=obj->ln.parent;parent!=NULL;parent=parent->ln.parent)
    {
	x+=parent->x;
	y+=parent->y;
    }
    tgt_intrefresh(obj,x,y);
}
/* obiekt aktywny to ten ktory ma focus. */
void tgt_activate(struct tgt_object *obj)
{
    struct tgt_object *parent;
    struct tgt_object *old_active;
    /* Aktywuj obiekt obj */
    parent=obj->ln.parent;
    if(parent!=NULL)
    {
	old_active=parent->ln.active_child;
	parent->ln.active_child=obj;
	if(old_active!=NULL) tgt_refresh(old_active);
	tgt_refresh(obj);
    }
}

struct tgt_object * tgt_findnext_selectable(struct tgt_object * obj)
{
    struct tgt_object *next;
    if(obj==NULL) return(NULL);
    next=obj;
    for(;;)
    {
	next=next->ln.next;
	if(next==NULL) next=obj->ln.parent->ln.first_child;
	if(next==obj) return(obj);
	if(!(next->objflags & TGT_OBJFLAGS_NONSELECTABLE)) return(next);
    }
}
struct tgt_object * tgt_findprev_selectable(struct tgt_object * obj)
{
    struct tgt_object *next;
    if(obj==NULL) return(NULL);
    next=obj;
    for(;;)
    {
	next=next->ln.prev;
	if(next==NULL) next=obj->ln.parent->ln.last_child;
	if(next==obj) return(NULL);
	if(!(next->objflags & TGT_OBJFLAGS_NONSELECTABLE)) return(next);
    }
}

void tgt_activatenext(struct tgt_object *obj)
{
    struct tgt_object *parent;
    /* Aktywuj nastepny obiekt po obj...*/
    if(!(parent=obj->ln.parent)) return;
    parent->ln.active_child=tgt_findnext_selectable(obj);
    tgt_refresh(obj);
    tgt_refresh(parent->ln.active_child);
}
void tgt_activateprev(struct tgt_object *obj)
{
    struct tgt_object *parent;
    /* Aktywuj poprzedni obiekt, przed obj...*/
    if(!(parent=obj->ln.parent)) return;
    parent->ln.active_child=tgt_findprev_selectable(obj);
    tgt_refresh(obj);
    tgt_refresh(parent->ln.active_child);
}


void tgt_activatenext_child(struct tgt_object *obj)
{
    struct tgt_object *active;
    struct tgt_object *old_active;
    /* Aktywuj nastepne dziecko obiektu obj...*/
    old_active=obj->ln.active_child;
    if(old_active==NULL)
	obj->ln.active_child=tgt_findnext_selectable(obj->ln.first_child);
    else
    {
	obj->ln.active_child=tgt_findnext_selectable(obj->ln.active_child);
	tgt_refresh(old_active);
    }
    if(obj->ln.active_child) tgt_refresh(obj->ln.active_child);
}
void tgt_activateprev_child(struct tgt_object *obj)
{
    struct tgt_object *active;
    struct tgt_object *old_active;
    /* Aktywuj poprzednie dziecko obiektu obj...*/
    old_active=obj->ln.active_child;
    if(old_active==NULL)
	obj->ln.active_child=tgt_findprev_selectable(obj->ln.last_child);
    else
    {
	obj->ln.active_child=tgt_findprev_selectable(obj->ln.active_child);
	tgt_refresh(old_active);
    }
    if(obj->ln.active_child) tgt_refresh(obj->ln.active_child);
}

int tgt_is_active(struct tgt_object *obj)
{
    if( ! obj->ln.parent) return(0);
    if(obj->ln.parent->ln.active_child == obj) return(1); else return(0);
}
int tgt_deliver_msg(struct tgt_object *obj,int type,int param,char* param2)
{
    struct tgt_object *active;
    /* dostarcz wiadomosc (type (w 99% bedzie to MSG_KEYHANDLE),param,param2)
       do ostatniego aktywnego dziecka obiektu obj. Jesli nie jest w stanie
       jej zrozumiec, dostarcz ja jego rodzicowi (wywolujac sama siebie) */
    for(active=obj;active->ln.active_child!=NULL;active=active->ln.active_child);
    while(active->classf(active,type,param,param2) != 1) {
	if((active=active->ln.parent)==NULL) return(1);
    }
}

