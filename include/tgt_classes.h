
struct tgt_listnode
{
    struct tgt_object *parent;
    /* Filled when linked */
    struct tgt_object *next;
    struct tgt_object *prev;
    /* Pointers for connecting in a linked list with other children */
    struct tgt_object *child;	/* (Active) child or NULL */
};

struct tgt_object
{
    struct tgt_listnode ln;
//    struct tgt_terminal *term; 
    /* Terminal description to use when rendering object */
    int (*classf) (struct tgt_object *,int,int,void*);
    /* The class functions, takes a pointer to object structure,
       task code (see below) and up to two task parameters, depending
       on kind of task to do */
    int (*objectf) ();
    /* Callback-class-dependant */
    short x,y,xs,ys; /* position&dimensions */
    unsigned char fg,bg; /* colors */

    int id;	/* Object numeric ID; might be passed by class to its
		    callbacks */
    void *class_data;
    /* Depends on classf */
    void *classlink_data;
    /* Depends on classf */
    void *user_data;
    unsigned char user_data2;
    /* whatever user wants */
/*    int active; */
/* Nie istnieje od v 0.07 */
    unsigned char objflags;
/* Bitfield, see below  */
    int *prev_keys;	/* Keys switching to previous child. */
    int *next_keys;	/* Keys switching to next child. */
    tgt_cell * visual_buffer;
    int socket;
};


/*

Class' methods (requests) - second argument values for class functions

classf(struct tgt_object * object,int request,int a,void *b)
                                  ^^^^^^^^^^^

*/
#define TGT_OBJECT_CREATE 1
/* Fill all fields you need in object structure. If you need - you can use
object creation taglist, passed as b */

#define TGT_OBJECT_DESTROY 2

/* Free all resources that were allocated during TGT_OBJECT_CREATE */
   
#define TGT_OBJECT_REFRESH 3
/* Refresh object; as a reference point use (a,(int) b) */

#define TGT_OBJECT_HANDLE 4
/* Can you understand keycode a? */

#define TGT_OBJECT_SETTAG 5
/* Can you set tag of code a to value (tgtt) b ? */
#define TGT_OBJECT_GETTAG 6
/* Can you save tag of code a to *((tgtt*) b) ? */

#define TGT_OBJECT_SETDEFAULTS 7
/* Set your default values eg. get them from rcfile structure; called
   upon tgt initialization  */

#define TGT_OBJECT_GETSIZES 8

#define TGT_OBJECT_LINK 9
#define TGT_OBJECT_UNLINK 10

/* New parent ! */

#define TGT_OBJECT_MOUSEDRAG 11

#define TGT_OBJECT_VISUALINIT 12

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

#define TGT_OBJFLAGS_INVISIBLE 8
#define TGT_OBJFLAGS_DONOTKILL 16
#define TGT_OBJFLAGS_RECURSIVEKILL 32

#define TGT_OBJFLAGS_NETLINKED 64



#define TGT_OBJFLAGS_REFRESHBASE 4


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
#define TGT_CLASS_STATUS 9

#define TGT_CLASS_MENU 29
#define TGT_CLASS_SELECTBOX 30
#define TGT_CLASS_MULTILINE 31
#define TGT_CLASS_TEXTBUFFER 32
#define TGT_CLASS_DIRECTSURFACE 33
#define TGT_CLASS_TERMEMU 34


#define TGT_CLASS_USERBASE 1000

#define TGTT_DESKTOP_HIDECURSOR 1000

#define TGT_CLASS_ACBASE 1000000

#define tgt_refresh(x) tgt_do_refresh(x,1)
#define tgt_single_refresh(x) tgt_do_refresh(x,0)
#define tgt_realloc_buffer(x) x->visual_buffer=(tgt_cell*) realloc(x->visual_buffer,x->xs*x->ys*sizeof(tgt_cell))
#define tgt_parent(x) ((x)->ln.parent)

