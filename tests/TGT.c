#include "tgt.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

struct tgt_object * labelptr;
struct tgt_object * desktop;
struct tgt_object * button_window;
struct tgt_object * checkbox_window;


char * items_table[] = {"Yes","No","I've no idea",NULL};
void create_important_windows(void);

char label_buffer[30];

int buttons_cb(struct tgt_object * obj, int id)
{
    snprintf(label_buffer, 30, "You have pressed button no. %d", obj->id);
    tgt_set(labelptr, TGTT_LABEL_TEXT, (tgtt) label_buffer);
    tgt_refresh(labelptr);
    return(1);
}

int cbox_cb(struct tgt_object * obj, int id)
{
    tgt_set(tgt_findchild(tgt_parent(obj),100), TGTT_CHECKBOX_ACTIVE, 0);
    tgt_set(tgt_findchild(tgt_parent(obj),101), TGTT_CHECKBOX_ACTIVE, 0);
    tgt_set(tgt_findchild(tgt_parent(obj),102), TGTT_CHECKBOX_ACTIVE, 0);
    tgt_set(obj, TGTT_CHECKBOX_ACTIVE, id);
    tgt_set(tgt_findchild(tgt_parent(obj),200), TGTT_SELECTBOX_SELECTED, obj->id-100);
    tgt_refresh(tgt_parent(obj));
    return(1);
}

int sbox_cb(struct tgt_object * obj, int id)
{
    tgt_set(tgt_findchild(tgt_parent(obj),100), TGTT_CHECKBOX_ACTIVE, 0);
    tgt_set(tgt_findchild(tgt_parent(obj),101), TGTT_CHECKBOX_ACTIVE, 0);
    tgt_set(tgt_findchild(tgt_parent(obj),102), TGTT_CHECKBOX_ACTIVE, 0);
    tgt_set(tgt_findchild(tgt_parent(obj),id+100), TGTT_CHECKBOX_ACTIVE, 1);
    tgt_refresh(tgt_parent(obj));
    return(1);
}

int button_window_cb(struct tgt_object *obj, int key)
{
    if(key != 'q' && key != 'Q') return(0);
    switch(obj->id)
    {
	case 1000:
	    tgt_unlink(button_window);
	    tgt_easyrequest(desktop,"TGT Test",
			"  Now, you're going to get familiar with Checkboxes and Selectboxes; "
			"of course they do not have to be always linked as in this example\n"
			"Navigation between these objects is quite intuitive;\n\n"
			"Switch by <tab> or cursor keys, select using ENTER or SPACE\n"
			"Press q to end this test\n\n",
			"[Continue]");
	    tgt_link(checkbox_window,desktop);
	    tgt_refresh(desktop);
	    break;
	default:
    	    tgt_destroyobject(desktop);
	    exit(0);
    }
    return(1);
}


int main(int argc,char ** argv)
{
    
    desktop = tgt_start(NULL);
    tgt_easyrequest(desktop,"TGT Test",
			"  This is a TGT test program.\n\n"
			"It's designed to demonstrate you "
			"possible TGT controls (objects) and "
			"to teach you how to navigate between objects\n\n"
			"If you want to check, how mouse handling works in "
			"TGT, remember to run this program on a REAL console "
			"(ie. not under mc or programs using similar technique "
			"to run 'background' shell)\n\n",
			"[Continue]");
    tgt_easyrequest(desktop,"TGT Test",
			"  The simplest TGT controls are buttons and labels\n"
			"Switch between buttons using <Tab> or arrow keys; "
			"select using ENTER or SPACE\n"
			"Press q to end the test\n\n",
			"[Continue]");

    create_important_windows();

    tgt_link(button_window, desktop);
    tgt_refresh(desktop);
    tgt_waitkeys(desktop);
    return 1;
}

void create_important_windows(void)
{
    button_window = tgt_ac_mkwindow("Buttons'n'labels",TGT_AC_HCENTER|TGT_AC_VCENTER,(tagitem[])
                  {TGT_AC_SPACE,1,
		   TGT_AC_CHILD,TGT_CLASS_BUTTON,
			TGTT_BUTTON_CAPTION, (tgtt) "Button number one",
            		TGTT_CALLBACK,(tgtt) buttons_cb,
			TGTT_ID,1,
			TGTT_END,0,
		   TGT_AC_CHILD,TGT_CLASS_BUTTON,
			TGTT_BUTTON_CAPTION, (tgtt) "Button number two",
            		TGTT_CALLBACK,(tgtt) buttons_cb,
			TGTT_ID,2,
			TGTT_END,0,
		   TGT_AC_CHILD,TGT_CLASS_BUTTON,
			TGTT_BUTTON_CAPTION, (tgtt) "Button number three",
            		TGTT_CALLBACK,(tgtt) buttons_cb,
			TGTT_ID,3,
			TGTT_END,0,
		   TGT_AC_SPACE,1,
		   TGT_AC_SAVEAS, (tgtt) &labelptr,
		   TGT_AC_CHILD,TGT_CLASS_LABEL,
			TGTT_XS,30,
			TGTT_END,0,
		   TGT_AC_END});

    checkbox_window = tgt_ac_mkwindow("Checkboxes & Selectboxes",TGT_AC_HCENTER|TGT_AC_VCENTER,(tagitem[])
                  {TGT_AC_SPACE, 1,
		   TGT_AC_CHILD, TGT_AC_HORIZONTALGROUP,
		      TGT_AC_SPACE, 1,
		      TGT_AC_CHILD, TGT_AC_VERTICALGROUP,
		          TGT_AC_CHILD, TGT_CLASS_CHECKBOX,
			      TGTT_CHECKBOX_CAPTION, (tgtt) "Yes",
			      TGTT_CHECKBOX_ACTIVE, 1,
			      TGTT_ID, 100,
			      TGTT_CALLBACK, (tgtt) cbox_cb,
			      TGTT_END, 0,
		          TGT_AC_CHILD, TGT_CLASS_CHECKBOX,
			      TGTT_CHECKBOX_CAPTION, (tgtt) "No",
			      TGTT_ID, 101,
			      TGTT_CALLBACK, (tgtt) cbox_cb,
			      TGTT_END, 0,
		          TGT_AC_CHILD, TGT_CLASS_CHECKBOX,
			      TGTT_CHECKBOX_CAPTION, (tgtt) "I have no idea",
			      TGTT_ID, 102,
			      TGTT_CALLBACK, (tgtt) cbox_cb,
			      TGTT_END, 0,
			  TGT_AC_END, 0,
		       TGT_AC_SPACE, 1,
		       TGT_AC_CHILD, TGT_CLASS_SELECTBOX,
		          TGTT_SELECTBOX_ITEMS, (tgtt) items_table,
			  TGTT_CALLBACK, (tgtt) sbox_cb,
			  TGTT_ID, 200,
			  TGTT_END,0,
		       TGT_AC_END,0,
		  TGT_AC_SPACE, 1,
		  TGT_AC_END,0});

    tgt_set(button_window, TGTT_ID, 1000);
    tgt_set(checkbox_window, TGTT_ID, 1001);

    tgt_set(button_window, TGTT_CALLBACK, (tgtt) button_window_cb);
    tgt_set(checkbox_window, TGTT_CALLBACK, (tgtt) button_window_cb);
}
