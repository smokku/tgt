/* API Functions */

int tgt_hasfocus(struct tgt_object *obj);

int tgt_set(struct tgt_object * obj,tgtt tag,tgtt value);
int tgt_get(struct tgt_object * obj,tgtt tag,tgtt *value);
tgtt tgt_direct_get(struct tgt_object * obj,tgtt tag);
void tgt_getkey(struct tgt_object *obj);
void tgt_setfocus(struct tgt_object *obj);
void tgt_chtimes(int a,int b);


struct tgt_object * tgt_findchild(struct tgt_object *obj,int id);
void tgt_recursive_set(struct tgt_object * obj, tgtt tag, tgtt value);
void tgt_buffer_abort(long *ptr);
int tgt_easyrequest(struct tgt_object *ref,char *title,char *body,char *buttons);
int tgt_askstring(struct tgt_object *ref,char *title,char *body,char *buttons,char* buffer,int maxlen);
void * tgt_launch_launcher(void);
void tgt_halt_launcher(void * lnch);
void * tgt_launcher_system(void *li,char *name);
void tgt_detach_launcher(void*);
void tgt_attach_launcher(void*,struct tgt_object *obj);
void tgt_launcher_kill(void *li);
int tgt_launcher_getchildpid(void *li);

void tgt_menu_enable(struct tgt_object *obj,int x,int y);
struct tgt_object * tgt_menu_getcontext(void);
void tgt_menu_selectability(struct tgt_object * menu,int pos,int sel);
void tgt_menu_add(struct tgt_object * obj,char * item,int nonselectable,struct tgt_object *sub);
void tgt_menu_insert(struct tgt_object * obj,int as, char * item,int nonselectable,struct tgt_object * submenu);
void tgt_menu_delete(struct tgt_object * obj, int n);
void tgt_menu_modify(struct tgt_object * obj, int num, char * text, int sel, struct tgt_object * submenu);

int tgt_isactive(struct tgt_object *obj);
int tgt_do_refresh(struct tgt_object *obj,int q);
int tgt_waitkeys(struct tgt_object *obj);
void tgt_link(struct tgt_object *obj,struct tgt_object *parent);
void tgt_unlink(struct tgt_object *obj);
struct tgt_object * tgt_getdesktop(void);
void tgt_destroyobject(struct tgt_object *obj);
struct tgt_object * tgt_createobject(int classid,
				 tagitem *taglist);
								  
struct tgt_object * tgt_createandlink(struct tgt_object *parent,
                                 int classid,tagitem *taglist);
								  
								  


void tgt_activate(struct tgt_object *obj);

struct tgt_queue * tgt_app_createqueue(void);
void tgt_app_queue(struct tgt_queue *q,int code,long d1,void *d2);
int tgt_app_waitmsg(struct tgt_queue *q,struct tgt_queue_msg *m);
int tgt_app_checkmsg(struct tgt_queue *q,struct tgt_queue_msg *m,unsigned int s,unsigned int us);

#ifndef TGT_PREFS_FILE
void * tgt_load_conf(char *);
char * tgt_get_conf(void *,char *,char *,char*);
int tgt_set_conf(void *,char *,char *,char *);
void tgt_unload_conf(void *);
int tgt_save_conf(void *,char *);
#endif

void tgt_addclass(int id,int (*)(struct tgt_object *,int,int,void*));
void * tgt_loadclass(int id,char *name);
void tgt_unloadclass(void *dlh);

struct tgt_object * tgt_start(char *tname);

/* Internal functions */


int tgt_builtin_desktop(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_window(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_button(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_label(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_string(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_list(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_menu(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_selectbox(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_checkbox(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_slider(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_progress(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_status(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_buffer(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_direct(struct tgt_object *obj,int type,int a,void * b);
int tgt_builtin_termemu(struct tgt_object *obj,int type,int a,void * b);


// class.c

void tgt_activatenext_child(struct tgt_object *obj);
void tgt_activateprev_child(struct tgt_object *obj);
int tgt_deliver_msg(struct tgt_object *obj,int type,int param,char* param2);

// keyboard.c

void tgt_rawcon(int);

// management.c

tgtt tgt_gettag(tgtt *taglist,tgtt stag,tgtt defaultvalue);
int (*tgt_int_getclass(int classid))(struct tgt_object*,int,int,void*);
struct tgt_object * tgt_createobject_int(int (*classf)(struct tgt_object*,int,int,void*),
		    tagitem *taglist,int,int,int,int);




struct tgt_keynode * tgt_initroottable(void);
void tgt_addkeyseq(struct tgt_keynode *l_main_table,char *seq,int c);

struct tgt_terminal * tgt_setscreen(char *name);
struct tgt_object * tgt_haschild(struct tgt_object *parent,int id);




/* autocreat.c */

int tgt_ac_minimalsize_g(tgtt *tl,int type,struct tgt_ac_objectinfo * ret,int spc);
struct tgt_ac_objectinfo * tgt_ac_minimalsize(int type,tagitem *tl,int spc);
void tgt_ac_positionself(struct tgt_ac_objectinfo * oi,int xp,int yp,int xs,int ys);
int tgt_ac_createself(struct tgt_object *linkto,struct tgt_ac_objectinfo * oi);

struct tgt_object * tgt_ac_mkwindow(char *title,
                                    int alignment,tagitem * description,...);

void tgt_destroyterminal(struct tgt_terminal *tterm);

struct tgt_keynode * tgt_allockeylookuptable(void);
struct tgt_keynode * tgt_initroottable(void);
void tgt_destroylookuptable(struct tgt_keynode * lt);
void tgt_addkeyseq(struct tgt_keynode *l_main_table,char *seq,int c);

void tgt_init(void);
void tgt_activatenext(struct tgt_object *obj);
void tgt_chattr(int request,int a,int b);
void tgt_initconswitch(void);
void tgt_activateprev(struct tgt_object *obj);
int tgt_shalliswitch(struct tgt_object *obj,int key,int pri);
void tgt_int_lowerb(tgt_cell * buffer,int l,tgt_cell attr);
void tgt_int_upperb(tgt_cell * buffer,int l,tgt_cell attr);



void tgt_init_mouse(void);
void tgt_reset_mouse(void);
void tgt_lock_mouse(struct tgt_object *obj);
void tgt_unlock_mouse(void);
int tgt_mouse_get_x(void);
int tgt_mouse_get_y(void);
int tgt_mouse_get_button(void);
void tgt_handle_mouse(struct tgt_object * obj);
void tgt_fatal_error(char *string);
void tgt_blit(tgt_cell * dest,int dxs,tgt_cell *src,int xs,int ys,int xp,int yp);
void tgt_fill_buffer(tgt_cell * buffer,int xs,int ys,tgt_cell cell);
void tgt_change_colors(tgt_cell * buffer,int length,int fg,int bg,int bold);
int tgt_printf(tgt_cell *buffer,int max,tgt_cell attr,char *format,...);
void tgt_flprintf(tgt_cell *buffer,int max,tgt_cell attr,char *format,...);
void tgt_update_screen(void);
tgtt tgt_getsizetag(tgtt * taglist,tgtt stag,tgtt defaultvalue);
void tgt_menu_rerender(tgt_cell * buffer,int xs,int ys,
                       struct tgt_int_menuitem * items,
		      int fg,int bg,int framecolor);

void tgt_menu_chcolors(tgt_cell * buffer,int n,int xs,int ys,int fg,int bg);
void tgt_rebuild_parent(struct tgt_object * obj);					      


struct tgt_terminal * tgt_getterminal(void);
void tgt_setdhandler(int (*dh)(struct tgt_object*, int, int, char*));
int tgt_notified_send(struct tgt_object * obj, int type, int p1, char *p2);
void tgt_hide(struct tgt_object * obj);
void tgt_unhide(struct tgt_object * obj);
void tgt_make_selectable(struct tgt_object *obj);
void tgt_make_nonselectable(struct tgt_object *obj);
