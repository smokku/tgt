/* ----------------------------------------------------------------------------
**      5.c    TGT Usage Example
** ----------------------------------------------------------------------------
**      TGT                                       TextGUIToolkit Library
**      Copyright (C)   2000-2001       Tomasz Sterna and Mateusz Golicz
**
**      LGPL license
**
** ----------------------------------------------------------------------------
**      MAINTAINER      Mateusz Golicz
**
**      Log:
**      
**
*/

/* so here it is */
/* this is same as 4.c, but uses a different object creation method */
/* by the way, you should REALLY read doc/en/AUTOCREATE before trying to
   understand this example :) */

#include "tgt.h"
#include "tgt_acmacros.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SEX_FEMALE 0
#define SEX_MALE 1

#define MAX_RECORDS 100

char * name[MAX_RECORDS];
char sex[MAX_RECORDS];
char * addr[MAX_RECORDS];
char * phone[MAX_RECORDS]; /* this'll hold our database records */

int rec_count = 0; /* # of records */

struct tgt_object * desktop; /* this'll store the root object */

int command(struct tgt_object *, int);
int add_command(struct tgt_object *, int);
int listcallback(void **,int,int,char *,int,int);
int chcallback(struct tgt_object *, int);
void add(void);

int main(int argc, char ** argv)
{
    struct tgt_object * window1;

    desktop = tgt_start(NULL); /* initialize TGT */

/* you do not have to type so much ... see macros usage below */
    window1 = tgt_ac_mkwindow("Sample no. 5", /* window title */
			      TGT_AC_HCENTER | TGT_AC_VCENTER, /* window alignment */
    /* this function creates a window using autocreat taglist */
		    (tgtt[]) {TGT_AC_SPACING, 1,
			      TGT_AC_CHILD, TGT_AC_HORIZONTALGROUP,
				    TGT_AC_CHILD, TGT_CLASS_LIST,
					TGTT_XS, 20,
					TGTT_LIST_FRAMECOLOR, TGT_COLOR_CYAN,
					TGTT_ID, 1000,
					TGTT_LIST_DATACALLBACK, (tgtt) listcallback,
					TGTT_LIST_CHANGECALLBACK, (tgtt) chcallback,
					TGTT_END, 0,
				    TGT_AC_CHILD, TGT_AC_VERTICALGROUP,
					TGT_AC_SPACING, 0,
					TGT_AC_CHILD, TGT_CLASS_BUTTON,
					    TGTT_BUTTON_CAPTION, (tgtt) "[ Add item    ]",
					    TGTT_ID, 100, TGTT_CALLBACK, (tgtt) command,
					    TGTT_END, 0,
					TGT_AC_CHILD, TGT_CLASS_BUTTON,
					    TGTT_BUTTON_CAPTION, (tgtt) "[ Remove item ]",
					    TGTT_ID, 101, TGTT_CALLBACK, (tgtt) command,
					    TGTT_END, 0,
					TGT_AC_CHILD, TGT_CLASS_BUTTON,
					    TGTT_BUTTON_CAPTION, (tgtt) "[ Change item ]",
					    TGTT_ID, 102, TGTT_CALLBACK, (tgtt) command,
					    TGTT_END, 0,
					TGT_AC_CHILD, TGT_CLASS_BUTTON,
					    TGTT_BUTTON_CAPTION, (tgtt) "[ Quit        ]",
					    TGTT_ID, 103, TGTT_CALLBACK, (tgtt) command,
					    TGTT_END, 0,
					TGT_AC_END, 0,
				    TGT_AC_END, 0,
				TGT_AC_CHILD, TGT_AC_HORIZONTALGROUP,
				    TGT_AC_CHILD, TGT_AC_VERTICALGROUP,
					TGT_AC_CHILD, TGT_CLASS_LABEL,
					    TGTT_LABEL_TEXT, (tgtt) "Name    :",
					    TGTT_END, 0,
					TGT_AC_CHILD, TGT_CLASS_LABEL,
					    TGTT_LABEL_TEXT, (tgtt) "Sex     :",
					    TGTT_END, 0,
					TGT_AC_CHILD, TGT_CLASS_LABEL,
					    TGTT_LABEL_TEXT, (tgtt) "Address :",
					    TGTT_END, 0,
					TGT_AC_CHILD, TGT_CLASS_LABEL,
					    TGTT_LABEL_TEXT, (tgtt) "Phone   :",
					    TGTT_END, 0,
					TGT_AC_END, 0,
				    TGT_AC_CHILD, TGT_AC_VERTICALGROUP,
					TGT_AC_CHILD, TGT_CLASS_LABEL,
					    TGTT_XS, 26,
					    TGTT_ID, 1001,
					    TGTT_END, 0,
					TGT_AC_CHILD, TGT_CLASS_LABEL,
					    TGTT_XS, 26,
					    TGTT_ID, 1002,
					    TGTT_END, 0,
					TGT_AC_CHILD, TGT_CLASS_LABEL,
					    TGTT_XS, 26,
					    TGTT_ID, 1003,
					    TGTT_END, 0,
					TGT_AC_CHILD, TGT_CLASS_LABEL,
					    TGTT_XS, 26,
					    TGTT_ID, 1004,
					    TGTT_END, 0,
					TGT_AC_END, 0,
				    TGT_AC_END,0,
				TGT_AC_END, 0});
    tgt_link(window1, desktop); /* make the first window visible on the desktop */

    tgt_refresh(desktop);
    tgt_waitkeys(desktop); /* wait for action */
    return(0);
}

/* this'll be called, when one of the buttons is pressed */

int command(struct tgt_object *obj, int p)
{
    switch(obj->id)
    {
	case 103: /* Quit button */
	    tgt_destroyobject(desktop);
	    exit(0);
	case 100: /* Add item button */
	    add();
	    break;
    }
    return(1);
}


void add(void)
{
    /* check, whether our window is already opened */
    if(! tgt_findchild(desktop, 10000))
    {
	/* it is not */
	struct tgt_object * addobjwindow;
	/* MACROS USAGE IN AUTOCREATE ENGINE:
		Macros save your fingers, but remember, that they're  not
		as flexible as constants. Fortunately, it's possible to mix
		macros & constants */
	addobjwindow = tgt_ac_mkwindow("Add an item", TGT_AC_LEFT | TGT_AC_TOP,
			(tgtt[]) {AcSetSpacing(1),
				    AcChild, AcHorizGroup,
					AcChild, AcVertGroup,
					    AcChild, SimpleLabel("Name     :"),
					    AcChild, SimpleLabel("Sex      :"),
					    AcChild, SimpleLabel("Address  :"),
					    AcChild, SimpleLabel("Phone No.:"),
					    AcEnd,
					AcChild, AcVertGroup,
					    AcMinWidth(25),
					    AcChild, SimpleStringI(26,10001),
					    AcChild, SimpleSelectboxI(((char*[]) {"Female", "Male", NULL}),10002),
					    AcChild, SimpleStringI(26,10003),
					    AcChild, SimpleStringI(26,10004),
					    AcEnd,
					AcEnd,
				    AcChild, AcHorizGroup,
					AcAlign(TGT_AC_LEFT),
					AcChild, SimpleButtonCI("[ Add  ]",add_command, 10005),
					AcAlign(TGT_AC_RIGHT),
					AcChild, SimpleButtonCI("[Cancel]",add_command, 10006),
				    AcEnd});

	tgt_set(addobjwindow, TGTT_ID, 10000);
	tgt_recursive_set(addobjwindow, TGTT_BG, TGT_COLOR_RED);
	/* this affects also object's children */
	
	tgt_link(addobjwindow, desktop);
	tgt_setfocus(addobjwindow);
    }
}

/* this'll be called, when one of the buttons in <add item> window is pressed */

int add_command(struct tgt_object *obj, int p)
{
    struct tgt_object * win = tgt_findchild(desktop, 10000);
    if(obj->id == 10005) /* add an item */
    {
	char * str;
	int s;
	tgt_get(tgt_findchild(desktop, 10001), /* 'name' string */
		TGTT_STRING_STRING,
		(tgtt*) &str);
/* TGT GET attempts to fetch the tag, which code was passed as a second
   argument from the object, that a pointer to was passed as first one.
   if this suceeds, tag value is written to a variable, which a pointer to
   was passed as the last argument ;) */
	name[rec_count] = strdup(str);

	tgt_get(tgt_findchild(desktop, 10002),TGTT_SELECTBOX_SELECTED,(tgtt*) &s);
	    /* sex */
	sex[rec_count] = s;

	tgt_get(tgt_findchild(desktop, 10003),TGTT_STRING_STRING,(tgtt*) &str);
	    /* address */
	addr[rec_count] = strdup(str);

	tgt_get(tgt_findchild(desktop, 10004),TGTT_STRING_STRING,(tgtt*) &str);
	    /* phone no  */
	phone[rec_count] = strdup(str);
	rec_count ++;

    }
/* In both cases, close the window */
    tgt_unlink(win);
    tgt_destroyobject(win);
    tgt_refresh(desktop);
    return(1);
}



/* this is used by TGT to read our list records, see doc/en/reference/c.list
   for more details */
int listcallback(void ** d,int c,int n,char *buff,int k,int a)
{
    if(c == TGT_LISTREQ_GETMAX)
    {
	/* tgt wants to know, how many items do we have */
	return(rec_count);
    }
    else
    {
	char fstr[20];
	/* tgt wants to fetch record number n */
	snprintf(fstr,20,"%%-%ds",k);
	snprintf(buff, k, fstr, name[n]);
    }
    return(1);
}

char male[]="Male";
char female[]="Female";

/* this is going to be called whenever the selected item in list changes */
int chcallback(struct tgt_object *obj, int p)
{
    if(p>=0 && p<rec_count)
    {
	tgt_set(tgt_findchild(desktop,1001), TGTT_LABEL_TEXT, (tgtt) name[p]);
	tgt_set(tgt_findchild(desktop,1002), TGTT_LABEL_TEXT, (tgtt) (sex[p] ? male : female));
	tgt_set(tgt_findchild(desktop,1003), TGTT_LABEL_TEXT, (tgtt) addr[p]);
	tgt_set(tgt_findchild(desktop,1004), TGTT_LABEL_TEXT, (tgtt) phone[p]);
	tgt_refresh(desktop);
    }
    return(1);
}
