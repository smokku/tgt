/*Prototypy funkcji - gdyby nie one to kompilator nie wywalalby nam 
tych glupch warningow jak za malo argumentow dalismy ;)))) */


int tgt_builtin_desktop(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_window(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_button(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_label(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_string(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_list(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_menu(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_cycle(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_checkbox(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_slider(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_progress(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_status(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_buffer(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_direct(struct tgt_object *obj,int type,int a,void * b);


// class.c

int tgt_refresh(struct tgt_object *obj);
void tgt_activatenext_child(struct tgt_object *obj);
void tgt_activateprev_child(struct tgt_object *obj);
int tgt_deliver_msg(struct tgt_object *obj,int type,int param,char* param2);

// keyboard.c

long tgt_rawcon(int);
int tgt_waitkeys(struct tgt_object *obj);

// management.c

tagdata tgt_gettag(tagitem *taglist,codetype stag,tagdata defaultvalue);
void tgt_link(struct tgt_object *obj,struct tgt_object *parent);
void tgt_unlink(struct tgt_object *obj);
struct tgt_object * tgt_createobject_int(struct tgt_terminal *term,
                    int (*classf)(struct tgt_object*,int,int,void*),
		    tagitem *taglist);

struct tgt_object * tgt_getdesktop(struct tgt_terminal * term);
void tgt_destroyobject(struct tgt_object *obj);
struct tgt_object * tgt_createobject(struct tgt_terminal *term,
                                 int classid,
				 tagitem *taglist);
								  
struct tgt_object * tgt_createandlink(struct tgt_object *parent,struct tgt_terminal *term,
                                 int classid,tagitem *taglist);
								  
								  


void tgt_activate(struct tgt_object *obj);


#ifdef TGT_QUEUES
#include "tgt_app.h"
struct tgt_queue * tgt_app_createqueue(void);
void tgt_app_queue(struct tgt_queue *q,int code,long d1,void *d2);
int tgt_app_waitmsg(struct tgt_queue *q,struct tgt_queue_msg *m);
int tgt_app_checkmsg(struct tgt_queue *q,struct tgt_queue_msg *m,unsigned int s,unsigned int us);
#endif

struct tgt_keynode * tgt_initroottable(void);
void tgt_addkeyseq(struct tgt_keynode *l_main_table,char *seq,int c);

struct tgt_terminal * tgt_setscreen(char *name);
struct tgt_object * tgt_haschild(struct tgt_object *parent,int id);

#ifndef PREFS_FILE
char * tgt_getprefs(void* prfs, char *section, char *key,char *def);
#endif

void tgt_addclass(int id,int (*)(struct tgt_object *,int,int,void*));
void * tgt_loadclass(int id,char *name);
void tgt_unloadclass(void *dlh);

struct tgt_object * tgt_start(char *tname,struct tgt_terminal **ptr);
