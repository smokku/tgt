
#define TGT_MENUF_NONSELECTABLE 1
#define TGT_MENUF_HORIZONTAL 2

#ifndef TGT_MENU_PROTOS

struct tgt_object * tgt_findmparent(struct tgt_object *obj);
void * tgt_menu_context(int n);
void tgt_menu_add(void * ptr,char *name,short id,short flags,void *submenu);
void tgt_menu_insert(short aid,void * ptr,char *name,short id,short flags,void *submenu);
void tgt_menu_draw(struct tgt_terminal *term,void *menu,int fg,int bg,int ffg,int sbg,int x,int y,int mx);
void tgt_menu_chp(void * menu,int p);
void tgt_menu_delete(void * menu,int id);
void tgt_menu_destroycontext(void * menu);
int tgt_menu_enter(void **menu);
void * tgt_menu_parent(void *mh);
void tgt_menu_setactive(void * menu,int p);
int tgt_menu_getmaxsize(void *menu);
int tgt_menu_ishorizontal(void *menu);
#endif
