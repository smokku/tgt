/*Prototypy funkcji - gdyby nie one to kompilator nie wywalalby nam 
tych glupch warningow jak za malo argumentow dalismy ;)))) */


int tgt_builtin_desktop(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_window(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_button(struct tgt_object *obj,int type,int a,void * b);

// class.c

int tgt_refresh(struct tgt_object *obj);
void tgt_activatenext(struct tgt_object *obj);
void tgt_activateprev(struct tgt_object *obj);
int tgt_deliver_msg(struct tgt_object *obj,int type,int param,char* param2);
int tgt_waitkeys(struct tgt_object *obj);

// keyboard.c

void tgt_initconswitch(void);
long tgt_rawcon(void);
void tgt_normalcon(void);
int tgt_get_key(struct tgt_terminal *tm);

// management.c

long tgt_gettag(long *taglist,long stag,long defaultvalue);
void tgt_link(struct tgt_object *obj,struct tgt_object *parent);
void tgt_unlink(struct tgt_object *obj);
struct tgt_object * tgt_createobject(struct tgt_terminal *term,
                    int (*classf)(struct tgt_object*,int,int,void*),
		    long *taglist);

struct tgt_object * tgt_getdesktop(struct tgt_terminal * term);
void tgt_destroyobject(struct tgt_object *obj);
struct tgt_object *tgt_createandlinkobject(struct tgt_object *parent,struct tgt_terminal *term,
                                 int (*classf)(struct tgt_object*,int,int,void*),
				 long *taglist);
struct tgt_object * tgt_createobject_int(struct tgt_terminal *term,
                                 int classid,
				 long *taglist);
								  
struct tgt_object * tgt_createandlink_int(struct tgt_object *parent,struct tgt_terminal *term,
                                 int classid,long *taglist);
								  
								  


void tgt_activate(struct tgt_object *obj);
