#include <stdio.h>
#include "tgt.h"

struct tgt_object * tgt_haschild(struct tgt_object *obj,int id)
{
    struct tgt_object *children;
    struct tgt_object *cc;
    struct tgt_object *ret;
    cc=obj->ln.child;
    if(!cc) return(NULL);
    for(children=cc->ln.next;children!=cc;children=children->ln.next)
	if(children->id==id) return(children);

    return(NULL);
}

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
    struct tgt_object *firstch;
    struct tgt_object *prevch;
    struct tgt_object *tmpch;
    /* funkcja przerysowuje aktualny obiekt i wszystkie dzieci */
    /* Wewnetrzna procedura odswierzania: odswierz obiekt obj,
       przeliczajac jego wspolrzedne wzgledem a i b (prosciej:
       po prostu dodajac je do nich :) po czym wywolaj siebie
       dla dzieci obiektu (wstecz listy)*/
    /* Oczywiscie 'brudna robote' wykonuje handler klasy... */
    obj->classf(obj,TGT_OBJECT_REFRESH,a,(void*) b);

    if(obj->ln.child)		/* Zapomniales (!!!!) */
    {    
	firstch=obj->ln.child->ln.prev;
	a+=obj->x; b+=obj->y;   /* Zapomniales... czy raczej moze ja zapomnialem powiedziec... */
	if((prevch=obj->ln.child->ln.prev)!=NULL)
    	    do{
		tmpch=prevch->ln.prev;
		tgt_intrefresh(prevch,a, b);
		prevch=tmpch;
	    }while(prevch!=firstch);
    }
}
#ifdef SEMAPHORED_REFRESH
    static int ref_semaphore=0;
#endif

int tgt_refresh(struct tgt_object *obj)
{
    int x,y;
    struct tgt_object *parent;
    struct tgt_object *ref;
#ifdef SEMAPHORED_REFRESH
    while(ref_semaphore) usleep(SEMAPHORED_REFRESH_DELAY);
    ref_semaphore=1;
#endif
    /* Znajdz absolutne wspolrzedne wzgledem ktorych ma zostac narysowany
       obiekt obj i poddaj go odswiezaniu */
    x=0; y=0; ref=obj;
    for(parent=obj->ln.parent;parent!=NULL;parent=parent->ln.parent)
    {
	x+=parent->x;
	y+=parent->y;
	ref=parent;
    }
    if(ref->objflags & TGT_OBJFLAGS_REFRESHBASE) tgt_intrefresh(obj,x,y);
#ifdef SEMAPHORED_REFRESH
    ref_semaphore=0;
#endif
}
int tgt_isactive(struct tgt_object *obj)
{
    if( ! obj->ln.parent) return(0);
    if(obj->ln.parent->ln.child == obj) return(1); else return(0);
}
int tgt_hasfocus(struct tgt_object *obj)
{
    struct tgt_object *parent;
    struct tgt_object *current;
    for(parent=obj->ln.parent,current=obj;parent!=NULL;){
        if(parent->ln.child!=current) return(0);
        current=parent;
        parent=parent->ln.parent;
    }
    return(1);
}
void tgt_setfocus(struct tgt_object *obj)
{
    struct tgt_object *parent;
    struct tgt_object *current;
    /* ustaw focus na obiekt obj */
    for(parent=obj->ln.parent,current=obj;parent!=NULL;){
	parent->ln.child=current;
	current=parent;
	parent=parent->ln.parent;
    }
    if(obj->ln.parent!=NULL)
	tgt_refresh(obj->ln.parent);
    else
	tgt_refresh(obj);
}

struct tgt_object * tgt_findnext_selectable(struct tgt_object * obj)
{
    struct tgt_object *next;
    if(obj==NULL) return(NULL);
    next=obj;
    for(;;)
    {
	next=next->ln.next;
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
	if(next==obj) return(obj);
	if(!(next->objflags & TGT_OBJFLAGS_NONSELECTABLE)) return(next);
    }
}

void tgt_activatenext(struct tgt_object *obj)
{
    struct tgt_object *parent;
    /* Aktywuj nastepny obiekt po obj...*/
    if(!(parent=obj->ln.parent)) return;
    parent->ln.child=tgt_findnext_selectable(obj);
    tgt_refresh(obj);
    if(parent->ln.child!=obj) tgt_refresh(parent->ln.child);
}
void tgt_activateprev(struct tgt_object *obj)
{
    struct tgt_object *parent;
    /* Aktywuj poprzedni obiekt, przed obj...*/
    if(!(parent=obj->ln.parent)) return;
    parent->ln.child=tgt_findprev_selectable(obj);
    tgt_refresh(obj);
    if(parent->ln.child!=obj) tgt_refresh(parent->ln.child);
}

// *** OBSOLETE ***
//void tgt_activatenext_child(struct tgt_object *obj)
//{
//    struct tgt_object *active;
//    struct tgt_object *old_active;
//    /* Aktywuj nastepne dziecko obiektu obj...*/
//    old_active=obj->ln.active_child;
//    if(old_active==NULL)
//	obj->ln.active_child=tgt_findnext_selectable(obj->ln.first_child);
//    else
//    {
//	obj->ln.active_child=tgt_findnext_selectable(obj->ln.active_child);
//	tgt_refresh(old_active);
//    }
//    if(obj->ln.active_child) tgt_refresh(obj->ln.active_child);
//}
//void tgt_activateprev_child(struct tgt_object *obj)
//{
//    struct tgt_object *active;
//    struct tgt_object *old_active;
//    /* Aktywuj poprzednie dziecko obiektu obj...*/
//    old_active=obj->ln.active_child;
//    if(old_active==NULL)
//	obj->ln.active_child=tgt_findprev_selectable(obj->ln.last_child);
//    else
//    {
//	obj->ln.active_child=tgt_findprev_selectable(obj->ln.active_child);
//	tgt_refresh(old_active);
//    }
//    if(obj->ln.active_child) tgt_refresh(obj->ln.active_child);
//}

int tgt_deliver_msg(struct tgt_object *obj,int type,int param,char* param2)
{
    struct tgt_object *active;
    /* dostarcz wiadomosc (type (w 99% bedzie to MSG_KEYHANDLE),param,param2)
       do ostatniego aktywnego dziecka obiektu obj. Jesli nie jest w stanie
       jej zrozumiec, dostarcz ja jego rodzicowi (wywolujac sama siebie) */
    for(active=obj;active->ln.child!=NULL;active=active->ln.child);

    while(active->classf(active,type,param,param2) != 1)
    {
	if(active==obj) return(0);
	if(!(active=active->ln.parent)) return(0);
    }
    return(1);
}

