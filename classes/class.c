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
    active=NULL;
    a+=obj->x; b+=obj->y;  /* Dodajemy nasze wspolrzedne-dzieci obiektu
    (np przyciski w oknie) w koncu maja ustawione wspolrzedne wzgledem
    naszych (czyli np . lewego gornego rogu okna) */
    for(children=obj->children;children!=NULL;children=children->next)
    {
	if(children->active==1)
	    active=children;	/* Aktywny obiekt na koncu */
	else
	    tgt_intrefresh(children,a,b);
    }
    if(active!=NULL) tgt_intrefresh(active,a,b); /* no i aktywny */
}
int tgt_refresh(struct tgt_object *obj)
{
    int x,y;
    struct tgt_object *parent;
    /* Znajdz absolutne wspolrzedne wzgledem ktorych ma zostac narysowany
       obiekt obj i poddaj go odswierzaniu */
    x=0; y=0;
    for(parent=obj->parent;parent!=NULL;parent=parent->parent)
    {
	x+=parent->x;
	y+=parent->y;
    }
    tgt_intrefresh(obj,x,y);
}
/* PRZYPOMINAM: obiekt aktywny-ma focus. nieaktywny-nie ma. To jedyna roznica :)*/

void tgt_activatenext(struct tgt_object *obj)
{
    struct tgt_object *child;
    struct tgt_object *act;
    /* Aktywuj nastepne dziecko obiektu obj... UGLYYY do rewrite... */
    child=obj->children;
    if(child==NULL) return;
    act=NULL;
    for(;child!=NULL;child=child->next) if(child->active==1) act=child;
    if(act==NULL) { obj->children->active=1; tgt_refresh(obj->children);return; }
    act->active=0; tgt_refresh(act);
    act=act->next; if(act==NULL) act=obj->children;
    act->active=1; tgt_refresh(act);
}
void tgt_activate(struct tgt_object *obj)
{
    struct tgt_object *parent;
    struct tgt_object *child;
/* aktywuj obiekt obj deaktywujac przy tym naturalnie pozostale dzieci
swojego rodzica */
    parent=obj->parent;
    if(parent==NULL) return;
    for(child=parent->children;child!=NULL;child=child->next) 
    {
	if(child->active==1)
	{
	    child->active=0;
	    tgt_refresh(child);
	}
    }
    obj->active=1;
    tgt_refresh(obj);
}

void tgt_activateprev(struct tgt_object *obj)
{
    /* Aktywuj poprzednie dziecko obiektu obj... UGLYYY do rewrite... */
    struct tgt_object *child;
    struct tgt_object *act;
    child=obj->children;
    if(child==NULL) return;
    act=NULL;
    for(;child!=NULL;child=child->next) if(child->active==1) act=child;
    if(act==NULL) { obj->children->active=1; tgt_refresh(obj->children); return; }
    act->active=0; tgt_refresh(act);
    act=act->prev; if(act==NULL) for(act=obj->children;act->next!=NULL;act=act->next);
    act->active=1; tgt_refresh(act);
}

int tgt_deliver_msg(struct tgt_object *obj,int type,int param,char* param2)
{
    struct tgt_object *children;
    /* dostarcz wiadomosc (type (w 99% bedzie to MSG_KEYHANDLE),param,param2)
       do obiektu obj. Jesli obiekt nie jest w stanie jej zrozumiec, dostarcz
       ja jego aktywnemu dziecku (wywolujac sama siebie) */
       
    if(obj->classf(obj,type,param,param2)==1) return(1);
    for(children=obj->children;children!=NULL;children=children->next)
	if(children->active==1) if(tgt_deliver_msg(children,type,param,param2)==1) return(1);
}

int tgt_waitkeys(struct tgt_object *obj)
{
    int c;
    /* Przelacz konsole w tryb 'raw' i czekaj na klawisze... jak cos
       to dostarcz wiadomosc obiektowi obj .. ( w 99% bedzie to obiekt klasy desktop )*/
    tgt_rawcon();
    for(;;)
    {
	c=tgt_get_key(obj->term);
	tgt_deliver_msg(obj,TGT_OBJECT_HANDLE,c,NULL);
    }
}
