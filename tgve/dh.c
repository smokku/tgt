#include "tgt.h"
#include "tgt_acmacros.h"
#include "tgve.h"
#include <stdio.h>
#include <unistd.h>

extern struct tgve_status state;
extern struct tgt_object * root;

int deliveryhandler(struct tgt_object * obj, int message, int a, char *b)
{
    
    if(! objinfo(obj))
    {
	state.mode = MODE_IDLE;
	return(0);
    }
    if(objinfo(obj)->type == WINTYPE_EDITOR) return(0);
    
    switch(message)
    {
	case TGT_OBJECT_HANDLE:
	    switch(a)
	    {
		case TGT_KEY_DELETE:
		case TGT_KEY_BKSPC:
		case ' ':
		    if(state.drag_mode!=DM_DELETE) return(0);
		    tgt_unlink(obj);
		    tgt_refresh(obj->ln.parent);
		    object_handlers[objinfo(obj)->objdata.class]->delete(obj);
		    tgt_destroyobject(obj);
		    break;
		case TGT_KEY_MOUSEDOWN:
		    if(tgt_mouse_get_button() == TGT_MOUSE_LMB)
		    {
			state.mode = MODE_DOWN;
			state.last_x = tgt_mouse_get_x();
			state.last_y = tgt_mouse_get_y();
			state.last_clicked = obj;
			if(objinfo(obj)->type != WINTYPE_DESKTOP)
			    set_status("%s at (%d, %d) size %d x %d", object_names[objinfo(obj)->objdata.class], obj->x, obj->y, obj->xs, obj->ys);
		    }
		    else if(tgt_mouse_get_button() == TGT_MOUSE_RMB)
		    {
			if(objinfo(obj)->type == WINTYPE_OBJECT)
			    open_editor(obj, root);
			else
			    return(0);
		    }
		    else return(0);
		    break;
		case TGT_KEY_MOUSEUP:
		    set_status("");
		    if(state.mode == MODE_DRAG && (state.drag_x >= state.last_x && state.drag_y >= state.last_y))
		    {
			if(state.drag_mode == DM_CREATE)
			{
				if(objinfo(state.last_clicked)->type == WINTYPE_DESKTOP)
				{
				    if(state.drag_x > state.last_x && state.drag_y > state.last_y)
					new_window(root, state.last_clicked, state.last_x, state.last_y, state.drag_x, state.drag_y);
				}
				else
				{
				    if(state.drag_x > state.last_x || state.drag_y > state.last_y)
					new_object(state.last_clicked, root, state.last_clicked, state.last_x, state.last_y, state.drag_x, state.drag_y);
				}
			}
		    }
		    state.mode = MODE_IDLE;
		    break;
		default:
		    return(0);
	    }
	    break;
	case TGT_OBJECT_MOUSEDRAG:
	    if(! state.last_clicked) break;
	    if(state.mode == MODE_DOWN) 
	    {
		state.mode = MODE_DRAG;
		if(state.drag_mode == DM_RESIZE)
		{
		    struct tgt_object * child = state.last_clicked->ln.child;
		    state.min_x = 1; state.min_y = 1;
		    
		    if(child)
		    {
			struct tgt_object * nc = child->ln.next;
			for(;;)
			{
			    int nxs, nys;
			    nxs = child->x + child->xs; 
			    nys = child->y + child->ys;
			    if(state.min_x < nxs) state.min_x = nxs;
			    if(state.min_y < nys) state.min_y = nys;
			    if(nc == child) break;
			    nc = nc->ln.next;
			}
		    }
		}
	    }
	    else
		if(state.mode != MODE_DRAG) break;

	    switch(state.drag_mode)
	    {
		case DM_CREATE:
		    state.drag_x = tgt_mouse_get_x();
		    state.drag_y = tgt_mouse_get_y();

		    if(state.drag_x > state.last_clicked->xs) state.drag_x = state.last_clicked->xs;
	    	    if(state.drag_y > state.last_clicked->ys) state.drag_y = state.last_clicked->ys;

	    	    tgt_refresh(state.last_clicked);
		    framedraw(state.last_clicked->visual_buffer, state.last_clicked->xs, 
			state.last_x, state.last_y, state.drag_x, state.drag_y);
	    	    tgt_rebuild_parent(state.last_clicked);
		    tgt_update_screen();
		    set_status("Create %s (%d, %d) (%d x %d)", 
			(objinfo(state.last_clicked)->type == WINTYPE_DESKTOP) ? "window" :
			object_names[tgt_direct_get(tgt_findchild(root, ID_NEWOBJTYPE) , TGTT_SELECTBOX_SELECTED)],
			state.last_x, state.last_y, state.drag_x - state.last_x, state.drag_y - state.last_y);
		    break;
		case DM_MOVE:			/* taken from window.c */
		    if(objinfo(state.last_clicked)->type == WINTYPE_DESKTOP) break;
	            state.last_clicked->x += a;
	            state.last_clicked->y += (int) b;
	            if(state.last_clicked->x < 0) state.last_clicked->x = 0;
	            if(state.last_clicked->y < 0) state.last_clicked->y = 0;
	            if((state.last_clicked->x + state.last_clicked->xs) > state.last_clicked->ln.parent->xs) state.last_clicked->x = state.last_clicked->ln.parent->xs - state.last_clicked->xs;
	            if((state.last_clicked->y + state.last_clicked->ys) > state.last_clicked->ln.parent->ys) state.last_clicked->y = state.last_clicked->ln.parent->ys - state.last_clicked->ys;
	            tgt_single_refresh(state.last_clicked->ln.parent);
		    set_status("Move: (%d, %d)", state.last_clicked->x, state.last_clicked->y);
		    break;
		case DM_RESIZE:
		    if(objinfo(state.last_clicked)->type == WINTYPE_DESKTOP) break;
		    tgt_set(state.last_clicked, TGTT_XS, state.last_clicked->xs + a);
		    tgt_set(state.last_clicked, TGTT_YS, state.last_clicked->ys + (int) b);
		    if(state.last_clicked->xs < state.min_x) state.last_clicked->xs = state.min_x;
		    if(state.last_clicked->ys < state.min_y) state.last_clicked->ys = state.min_y;
	            if((state.last_clicked->x + state.last_clicked->xs) > state.last_clicked->ln.parent->xs) tgt_set(state.last_clicked, TGTT_XS, state.last_clicked->ln.parent->xs - state.last_clicked->x);
	            if((state.last_clicked->y + state.last_clicked->ys) > state.last_clicked->ln.parent->ys) tgt_set(state.last_clicked, TGTT_YS, state.last_clicked->ln.parent->ys - state.last_clicked->y);
		    tgt_realloc_buffer(state.last_clicked);
		    tgt_refresh(state.last_clicked->ln.parent);
		    set_status("Resize: (%d x %d)", state.last_clicked->xs, state.last_clicked->ys);
		    break;
	    }
	    break;
    }
    
    return(1);
}

