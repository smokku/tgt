#include "tgt.h"
#include "tgve.h"
#include <stdio.h>

extern struct tgt_object * root;

static void generic_change(struct tgt_object * win, struct tgt_object * ref)
{
    ref->fg = tgt_direct_get(tgt_findchild(win, ID_COMMON), TGTT_SELECTBOX_SELECTED);
    ref->bg = tgt_direct_get(tgt_findchild(win, ID_COMMON+1), TGTT_SELECTBOX_SELECTED);
    snprintf(objinfo(ref)->objdata.virtual_id, 20, "%s", tgt_direct_get(tgt_findchild(win, ID_COMMON+2), TGTT_STRING_STRING));
    snprintf(objinfo(ref)->objdata.virtual_callback, 20, "%s", tgt_direct_get(tgt_findchild(win, ID_COMMON+3), TGTT_STRING_STRING));
}

static void generic_update(struct tgt_object * win, struct tgt_object * ref)
{
    tgt_set(tgt_findchild(win, ID_COMMON), TGTT_SELECTBOX_SELECTED,(tgtt) ref->fg);
    tgt_set(tgt_findchild(win, ID_COMMON+1), TGTT_SELECTBOX_SELECTED,(tgtt) ref->bg);
    tgt_set(tgt_findchild(win, ID_COMMON+2), TGTT_STRING_STRING,(tgtt) objinfo(ref)->objdata.virtual_id);
    tgt_set(tgt_findchild(win, ID_COMMON+3), TGTT_STRING_STRING,(tgtt) objinfo(ref)->objdata.virtual_callback);
}

int ed_list_datacb(void **data,int c,int n,char *buffer,int k,int a)
{
    int i;
    struct attributes_list * al = (struct attributes_list*) data;
    
    if(c == TGT_LISTREQ_GETMAX)
    {
	for(i = 0; al[i].type != ATYP_END; i++);
	return(i);
    }
    else
    {
	snprintf(buffer, k, "%s", al[n].name);
    }
}
void ed_list_changecb(struct tgt_object * obj, int nitem)
{
    struct attributes_list * al;
    struct tgt_object * eo;
    
    if(tgt_get(obj, TGTT_LIST_ITEMS, (tgtt*) &al))
    {
	tgt_hide(tgt_findchild(obj->ln.parent, al[obj->user_data2].id));
	tgt_unhide(eo = tgt_findchild(obj->ln.parent, al[obj->user_data2 = nitem].id));
	tgt_set(tgt_haschild(obj->ln.parent,ID_PROPLABEL), TGTT_LABEL_TEXT, (tgtt) al[nitem].name);
	tgt_refresh(tgt_haschild(obj->ln.parent,ID_PROPLABEL));
	tgt_refresh(eo);
    }
}

int ed_actioncb(struct tgt_object * obj, int code)
{
    struct tgt_object * win = obj->ln.parent;
    struct tgt_object * ref;
    
    tgt_unlink(win);
    tgt_refresh(root);
    unlock_object(objinfo(win)->editordata.reference);
    if(obj->id == ID_ACCEPT)
    {
	generic_change(win, ref = objinfo(win)->editordata.reference);
	(objinfo(win)->editordata.ei)->change(win, ref);
	tgt_refresh(ref);
    }
    tgt_destroyobject(win);
}

void open_editor(struct tgt_object * obj, struct tgt_object * root)
{
    if(lock_object(obj))
    {
	struct tgt_object * eo; /* Editor object */
	struct tgt_object * co; /* Current object */
	struct tgt_object * lo; /* List object */
	
	struct tgve_editorinfo * ei = object_handlers[objinfo(obj)->objdata.class];
	struct attributes_list * al;
	int xs, ys, xp, yp, i;

	xs = tgt_getterminal()->x_size/2;
	ys = tgt_getterminal()->y_size/2;
	if(xs < 50) xs = 50;
	if(ys < 10) ys = 10;

	eo = tgt_createobject(TGT_CLASS_WINDOW, (tgtt[]) {TGTT_WINDOW_TITLE, (tgtt) "Object attributes", TGTT_END, 0});
	if(!eo) { unlock_object(obj); return; }
	
	tgt_set(eo, TGTT_XS, xs);
	tgt_set(eo, TGTT_YS, ys);
	ys -= 2; xs -= 2; /* substract frame sizes */
	
	lo = co = tgt_createobject(TGT_CLASS_LIST, (tgtt[]) {TGTT_X, 2, TGTT_Y, 2, TGTT_LIST_FRAMECOLOR, TGT_COLOR_CYAN, TGTT_END, 0});
	tgt_set(co, TGTT_XS, xs - 2);
	tgt_set(co, TGTT_YS, ys - 4);
	tgt_set(co, TGTT_LIST_ITEMS, (tgtt) ei->attrs);
	tgt_set(co, TGTT_LIST_DATACALLBACK, (tgtt) ed_list_datacb);
	tgt_set(co, TGTT_LIST_CHANGECALLBACK, (tgtt) ed_list_changecb);
	tgt_set(co, TGTT_LIST_SELECTED, 0);
	tgt_link(co, eo);
	
	co = tgt_createobject(TGT_CLASS_BUTTON, (tgtt[]) {TGTT_BUTTON_CAPTION, (tgtt) "[Accept]", TGTT_ID, ID_ACCEPT, TGTT_X, 2, TGTT_CALLBACK, (tgtt) ed_actioncb, TGTT_END, 0});
	tgt_set(co, TGTT_Y, ys); tgt_link(co, eo);
	
	co = tgt_createobject(TGT_CLASS_BUTTON, (tgtt[]) {TGTT_BUTTON_CAPTION, (tgtt) "[Cancel]", TGTT_ID, ID_CANCEL, TGTT_X, 11, TGTT_CALLBACK, (tgtt) ed_actioncb, TGTT_END, 0});
	tgt_set(co, TGTT_Y, ys); tgt_link(co, eo);

	co = tgt_createobject(TGT_CLASS_LABEL, (tgtt[]) {TGTT_X, 2, TGTT_ID, ID_PROPLABEL, TGTT_LABEL_TEXT, (tgtt) "", TGTT_END, 0});
	tgt_set(co, TGTT_Y, ys - 2); tgt_set(co, TGTT_XS, (xs - 2) / 2);
	tgt_set(co, TGTT_LABEL_TEXT, (tgtt) ei->attrs[0].name);
	tgt_link(co, eo);
	
	for(al = ei->attrs, i = 0; al[i].type != ATYP_END; i++)
	{
	    switch(al[i].type)
	    {
		case ATYP_BOOLEAN:  co = tgt_createobject(TGT_CLASS_CHECKBOX, NULL); break;
		case ATYP_STRING:
		    co = tgt_createobject(TGT_CLASS_STRING, NULL); 
		    tgt_set(co, TGTT_STRING_MAX, 20);
		    break;
		case ATYP_SELECTION:
		    co = tgt_createobject(TGT_CLASS_SELECTBOX, NULL); 
		    tgt_set(co, TGTT_SELECTBOX_ITEMS, (tgtt) al[i].data);
		    break;
	    }
	    tgt_set(co, TGTT_Y, ys - 2); tgt_set(co, TGTT_X, xs/2 + 1);
	    tgt_set(co, TGTT_XS, (xs - 2) / 2);
	    tgt_set(co, TGTT_ID, al[i].id);
	    tgt_link(co, eo);
	    if(i) tgt_hide(co);
	}
	lo->user_data2 = 0;
	generic_update(eo, obj);
	ei->update(eo, obj);
	eo->user_data = create_editordatainfo(ei, obj);
	tgt_link(eo, root);
	tgt_refresh(root);
	tgt_setfocus(eo);
    }
}
