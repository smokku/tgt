#include <stdio.h>
#include "tgt.h"
#ifdef TGT_DLCLASSES
#include <dlfcn.h>
#endif
    static int int_initialized=0; /*Inicjowalismy juz tablice wewnetrznych klas?*/
    void *internal_classes[100]; /* owa tablica... */
    extern void *g_prefs;

struct tgt_object * tgt_start(char *tname,struct tgt_terminal **ptr)
{
    struct tgt_object *desktop;
    struct tgt_terminal *terminal;
    tgt_init();
    if(!(terminal=tgt_setscreen(NULL))) return(NULL);
    if(!(desktop=tgt_getdesktop(terminal)))
    {
	tgt_destroyterminal(terminal);
	return(NULL);
    }
    if(ptr) *ptr=terminal;
    return(desktop);
}

void tgt_addclass(int id,int (*classf)(struct tgt_object *,int,int,void*))
{
    if(g_prefs) classf(NULL,TGT_OBJECT_SETDEFAULTS,0,g_prefs);
    internal_classes[id]=(void*) classf;
}
#ifdef TGT_DLCLASSES
void * tgt_loadclass(int id,char *name)
{
    void *dlh;
    int (*classf)(struct tgt_object *,int,int,void*);
    if(dlh=dlopen(name,RTLD_LAZY))
    {
	classf=dlsym(dlh,"classmain");
	if(classf)
	{
	    tgt_addclass(id,classf);
	    return(dlh);
	}
	else
	{
	    dlclose(dlh);
	    return(NULL);
	}
    }
    else
	return(NULL);
}
void tgt_unloadclass(void *dlh)
{
    dlclose(dlh);
}
#endif
void tgt_init_intclasses(void)
{
    /* ehh.. narazie cosik malo ... ;) */
    tgt_addclass(TGT_CLASS_DESKTOP,tgt_builtin_desktop);
    tgt_addclass(TGT_CLASS_WINDOW,tgt_builtin_window);
    tgt_addclass(TGT_CLASS_BUTTON,tgt_builtin_button);
    tgt_addclass(TGT_CLASS_LABEL,tgt_builtin_label);
    tgt_addclass(TGT_CLASS_STRING,tgt_builtin_string);
    tgt_addclass(TGT_CLASS_LIST,tgt_builtin_list);
    tgt_addclass(TGT_CLASS_MENU,tgt_builtin_menu);
    tgt_addclass(TGT_CLASS_CYCLE,tgt_builtin_cycle);
    tgt_addclass(TGT_CLASS_CHECKBOX,tgt_builtin_checkbox);
    tgt_addclass(TGT_CLASS_SLIDER,tgt_builtin_slider);
    tgt_addclass(TGT_CLASS_PROGRESS,tgt_builtin_progress);
    tgt_addclass(TGT_CLASS_STATUS,tgt_builtin_status);
    tgt_addclass(TGT_CLASS_TEXTBUFFER,tgt_builtin_buffer);
    tgt_addclass(TGT_CLASS_DIRECTSURFACE,tgt_builtin_direct);
    int_initialized=1;    
}

	    

tagdata tgt_gettag(tagitem *taglist,codetype stag,tagdata defaultvalue)
{
    long tag;
    tagdata value;
    /* Przeszukuje tagliste taglist w poszukiwaniu tagu stag i zwraca
      jego wartosc. Jesli tag nie zostanie znaleziony, funkcja zwraca
      defaultvalue */
      
    if(taglist==NULL) return(defaultvalue);
    for(;;)
    {
	tag=taglist->tcode; value=taglist->tdata; taglist++;
	if(tag==TGTT_END) return(defaultvalue); /* koniec taglisty */
	if(stag==tag) return(value); /* tag ktorego szukamy */
    }
}

void tgt_link(struct tgt_object *obj,struct tgt_object *parent)
{
    /* wlacza obiekt obj do listy dzeci obiektu parent */
    obj->ln.parent=parent;
    /* naszym rodzicem jest od tej chwili parent ... */
    if(parent->ln.child==NULL){
	obj->ln.next=obj;
	obj->ln.prev=obj;
	parent->ln.child=obj;
    }else{
	obj->ln.prev=parent->ln.child->ln.prev;
	obj->ln.next=parent->ln.child;
	parent->ln.child->ln.prev->ln.next=obj;
	parent->ln.child->ln.prev=obj;
	/* wstawiamy sie w srodek listy */
	if(parent->ln.child->objflags & TGT_OBJFLAGS_NONSELECTABLE)
	    parent->ln.child=obj;
	/* jesli obiekt nonselectable to wstawiamy sie w jego miejsce */
    }
}
void tgt_unlink(struct tgt_object *obj)
{
    struct tgt_object *next;
    struct tgt_object *parent;
    /* odlacza obiekt obj */
    if(obj->ln.parent!=NULL){
	if(obj->ln.parent->ln.child==obj) tgt_activatenext(obj);
	/* jesli jestesmy aktywni przestawmy rodzica na nastepne SELECTABLE dziecko */
	if(obj->ln.parent->ln.child==obj) obj->ln.parent->ln.child=obj->ln.next;
	/* jesli to znow my, to przestawmy na nastepne DOWOLNE dziecko */
	if(obj->ln.parent->ln.child==obj){
	    obj->ln.parent->ln.child=NULL;
	    /* jesli to znow my, to zamykamy liste dzieci */
	}else{
	    obj->ln.next->ln.prev=obj->ln.prev;
	    obj->ln.prev->ln.next=obj->ln.next;
	    /* jesli nie, to ja po prostu zwijamy */
	}
    }
}


struct tgt_object * tgt_createobject_int(struct tgt_terminal *term,
				 int (*classf)(struct tgt_object*,int,int,void*),
				 tagitem *taglist)
{
    struct tgt_object *ret;
    /* Tworzy obiekt dla klasy o handlerze classf istniejacy w terminalu term,
       z atrybutami takimi jak podane w tagliscie */

    /* Przydzielamy pamiec... */    
    ret=(struct tgt_object*) malloc(sizeof(struct tgt_object));

    ret->term=term; ret->classf=classf;
    /* Ustawiamy sobie pare najczesciej uzywanych i majacych wlasne pola w 
       strukturze tagow */
    ret->x=tgt_getnumtag(taglist,TGTT_X,0); ret->y=tgt_getnumtag(taglist,TGTT_Y,0);
    ret->xs=tgt_getnumtag(taglist,TGTT_XS,1); ret->ys=tgt_getnumtag(taglist,TGTT_YS,1);
    ret->fg=tgt_getnumtag(taglist,TGTT_FG,term->color_fg); ret->bg=tgt_getnumtag(taglist,TGTT_BG,term->color_bg);
    ret->id=tgt_getnumtag(taglist,TGTT_ID,0); 
    ret->next_keys=(int*) tgt_getptrtag(taglist,TGTT_NEXT_KEYS,0);
    ret->prev_keys=(int*) tgt_getptrtag(taglist,TGTT_PREV_KEYS,0);
/* W sumie to moglaby sobie to zalatwic klasa sama ale ze dazymy do maksymalnej
  idiotoodpornosci... ;)) */

    ret->ln.parent=NULL; ret->ln.child=NULL;
    ret->ln.next=NULL; ret->ln.prev=NULL;
    ret->objflags=0;

    /* no i teraz dajemy klasie mozliwosc stworzenia swoich wlasnych
       strktur opisujacych obiekt-np. tak jak jest to opisane w tagliscie.
       Zwrocenie zera oznacza brak jakiegos niezbednego tagu czy tam
       jakies inne niepowodzenie ... */
    if(classf!=NULL) if(classf(ret,TGT_OBJECT_CREATE,0,taglist)==0) { free(ret); return(NULL); }
    return(ret);
}

struct tgt_object * tgt_getdesktop(struct tgt_terminal * term)
{
    struct tgt_object *desk;
    /* Front-end dla tgt_createobject() tworzacy nadrzedny obiekt desktopu dla terminala term */
    desk=tgt_createobject_int(term,tgt_builtin_desktop,(tagitem[]) { TGTT_X,1, TGTT_Y,1, TGTT_END,0} );
    //! a tu z kolei powinno byc TGTT_X,0, TGTT_Y,0   bo wspolzedne ekranowe liczymy od zera..
    desk->xs = term->x_size;
    desk->ys = term->y_size;
    return(desk);
}
void tgt_destroyobject(struct tgt_object *obj)
{
    struct tgt_object *firstch;
    struct tgt_object *nextch;
    struct tgt_object *tmpch;
    /* Likwiduje obiekt wywolujac odpowiednia metode z klasy po czym zwalniajac
       przeznaczona dla niego pamiec. Procedura wywoluje siebie rekursywnie w
       celu usuniecia rowniez dzieci obiektu*/
    
    firstch=obj->ln.child;
    if((nextch=obj->ln.child)!=NULL)
        do{
	    tmpch=nextch->ln.next;
	    tgt_destroyobject(nextch);
	    nextch=tmpch;
	}while(nextch!=firstch);
    obj->classf(obj,TGT_OBJECT_DESTROY,0,0);
    free(obj);
    return;
}
struct tgt_object * tgt_createobject(struct tgt_terminal *term,
				 int classid,
				 tagitem *taglist)
{
    /* jw, front-end, tworzy obiekt wykorzystujac jako handler klasy
       jedna z wbudowanych w system klas */
    if(int_initialized==0) tgt_init_intclasses();

    return(tgt_createobject_int(term,internal_classes[classid],taglist));
}

struct tgt_object * tgt_createandlink(struct tgt_object *parent,struct tgt_terminal *term,
				 int classid,
				 tagitem *taglist)
{
    /* Kombinacja obu powyzszych */
    struct tgt_object *ret;
    if(int_initialized==0) tgt_init_intclasses();
    ret=tgt_createobject_int(term,internal_classes[classid],taglist);
    if(ret==NULL) return(NULL);
    tgt_link(ret,parent);
    return(ret);
}