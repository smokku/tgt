#include "tgt.h"
#include "tgt_acmacros.h"
#include "tgve.h"
#include <stdio.h>
#include <unistd.h>


extern struct tgve_status state;
extern struct tgt_object * root;

int winonly_protection = 0;

void change_mode(struct tgt_object * obj, int m)
{
    state.drag_mode = m;
    switch(m)
    {
	case DM_MOVE: set_status("Click & drag to move objects"); tgt_hide(tgt_findchild(root, ID_NEWOBJTYPE)); tgt_hide(tgt_findchild(root, ID_CREATLABEL)); break;
	case DM_RESIZE: set_status("Click & drag to scale objects"); tgt_hide(tgt_findchild(root, ID_NEWOBJTYPE)); tgt_hide(tgt_findchild(root, ID_CREATLABEL)); break;
	case DM_CREATE: set_status("Click & drag to create objects"); tgt_unhide(tgt_findchild(root, ID_NEWOBJTYPE)); tgt_unhide(tgt_findchild(root, ID_CREATLABEL)); break;
    }
}

void new_object(struct tgt_object *p, struct tgt_object * r, struct tgt_object *o, int x1, int y1, int x2, int y2)
{
    struct tgt_object * obj;
    int type;
    
    if(x2 >= p->xs || y2 >= p->ys) return;

    if(! (obj = tgt_findchild(r, ID_NEWOBJTYPE))) return;
    type = tgt_direct_get(obj, TGTT_SELECTBOX_SELECTED);
    
    if( objinfo(p)->objdata.class != 13) 
	if(! winonly_protection)
	{
	    set_status("Creation inside windows' children is disabled!");
	    return;
	}
    
    obj = object_handlers[type]->create();

    if(obj)
    {
	obj->user_data = create_objdatainfo(type, 0);

	tgt_set(obj, TGTT_X, x1);
	tgt_set(obj, TGTT_Y, y1);
	tgt_set(obj, TGTT_XS, x2 - x1);
	tgt_set(obj, TGTT_YS, y2 - y1);

	tgt_link(obj, p);
	tgt_setfocus(obj);
	set_status("Created a %s", object_names[type]);
    }
    else
	tgt_easyrequest(r, "Error", "Unable to create such an object", "Cancel");
}

void new_window(struct tgt_object * r, struct tgt_object *o, int x1, int y1, int x2, int y2)
{
    struct tgt_object * obj;

    if(x2 >= r->xs || y2 >= r->ys) return;
    
    obj = tgt_createobject(TGT_CLASS_WINDOW, NULL);    
    obj->user_data = create_objdatainfo(13, 0);


    tgt_set(obj, TGTT_X, x1);
    tgt_set(obj, TGTT_Y, y1);
    tgt_set(obj, TGTT_XS, x2 - x1);
    tgt_set(obj, TGTT_YS, y2 - y1);
    tgt_link(obj, r);
    tgt_setfocus(obj);
    set_status("Created a window");
}

void operations(struct tgt_object * obj, int param)
{
    switch(obj->id)
    {
	case ID_QUIT:
	    tgt_destroyobject(root);
	    exit(0);
	    break;
	case ID_ICCB:
	    winonly_protection = tgt_direct_get(obj, TGTT_CHECKBOX_ACTIVE);
	    break;
    }
}

