#include <stdio.h>
#include "tgt.h"


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
	if((next->objflags & TGT_OBJFLAGS_NONSELECTABLE) == 0) return(next);
	next=next->ln.next;
	if(next==NULL) next=obj->ln.parent->ln.first_child;
	if(next==obj) return(NULL);
    }
}
struct tgt_object * tgt_findprev_selectable(struct tgt_object * obj)
{
    struct tgt_object *next;
    if(obj==NULL) return(NULL);
    next=obj;
    for(;;)
    {
	if((next->objflags & TGT_OBJFLAGS_NONSELECTABLE) == 0) return(next);
	next=next->ln.prev;
	if(next==NULL) next=obj->ln.parent->ln.last_child;
	if(next==obj) return(NULL);

    }
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
	active=old_active->ln.next;
	if(active!=NULL)
	    obj->ln.active_child=tgt_findnext_selectable(active);
	else
	    obj->ln.active_child=tgt_findnext_selectable(obj->ln.first_child);
	
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
	active=old_active->ln.prev;
	if(active!=NULL)
	    obj->ln.active_child=tgt_findprev_selectable(active);
	else
	    obj->ln.active_child=tgt_findprev_selectable(obj->ln.last_child);

	tgt_refresh(old_active);
    }
    if(obj->ln.active_child) tgt_refresh(obj->ln.active_child);
}

int tgt_is_active(struct tgt_object *obj)
{
    struct tgt_object *parent;
    parent=obj->ln.parent;
    if(parent==NULL) return(0);
    if(parent->ln.active_child==obj) return(1); else return(0);
}
int tgt_deliver_msg(struct tgt_object *obj,int type,int param,char* param2)
{
    struct tgt_object *active;
    /* dostarcz wiadomosc (type (w 99% bedzie to MSG_KEYHANDLE),param,param2)
       do obiektu obj. Jesli obiekt nie jest w stanie jej zrozumiec, dostarcz
       ja jego aktywnemu dziecku (wywolujac sama siebie) */
#ifdef TGT_POSSIBLE_INFORMFIRST
/* Zobacz czy obiekt koncowy nie ma ustawionej flagi inform_first ... jesli tak
poinformuj go jako pierwszy */
    for(active=obj;active->ln.active_child!=NULL;active=active->ln.active_child);
    if(active->objflags & TGT_OBJFLAGS_INFORMFIRST) if(active->classf(active,type,param,param2)==1) return(1);
#endif
    for(active=obj;active!=NULL;active=active->ln.active_child)
	if(active->classf(active,type,param,param2)==1) return(1);

}

/*int tgt_waitkeys(struct tgt_object *obj)
{
    int c;
    tgt_rawcon();
    for(;;)
    {
	c=tgt_get_key(obj->term);
	tgt_deliver_msg(obj,TGT_OBJECT_HANDLE,c,NULL);
    }
}*/
