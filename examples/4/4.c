/* ----------------------------------------------------------------------------
**      4.c    TGT Usage Example
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

/* now let's do something more fun */
/* 

 WARNING WARNING WARNING WARNING
 
 THERE IS A MUCH BETTER AND FASTER METHOD OF CREATING TGT OBJECTS
 THAN createobject(). IT IS EVEN POSSIBLE TO MAKE A WINDOW WITH MANY CHILDREN
 USING ONLY ONE CALL. SEE 4.c
 
 And BTW, this tool is incomplete :)

*/

#include "tgt.h"
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

    window1 = tgt_createobject(TGT_CLASS_WINDOW, (tgtt[])
		{TGTT_WINDOW_TITLE, (tgtt) "Sample no. 4",
		 TGTT_XS, 40,
		 TGTT_YS, 20,
		 TGTT_END,0}); /* create main window */
   
    tgt_set(window1, TGTT_X, (tgt_getterminal()->x_size - 40) / 2);
    tgt_set(window1, TGTT_Y, (tgt_getterminal()->y_size - 20) / 2);
    /* center it on the screen */

    /* this control holds the list of records */
    tgt_createandlink(window1, TGT_CLASS_LIST, (tgtt[])
		{TGTT_LIST_FRAMECOLOR, TGT_COLOR_CYAN,
		 TGTT_X, 2, TGTT_Y, 2,
		 TGTT_XS, 20, TGTT_YS, 10,
		 TGTT_ID, 1000,
    /* if we assign an object an unique identify number (but different than 0),
       we don't have to care about storing the object pointer, because
       we'll be able to fetch it from object's parent, grandparent, etc.,
       using tgt_findchild(p, child_id), where p is an object, that
       our one (or its parent, or its grandparent, etc) is linked to */
		 TGTT_LIST_DATACALLBACK, (tgtt) listcallback,
		 TGTT_LIST_CHANGECALLBACK, (tgtt) chcallback,
    /* see doc/en/reference/c.list for more details */
		 TGTT_END,0});
		 
    /* now, we'll create operation buttons */
    tgt_createandlink(window1, TGT_CLASS_BUTTON, (tgtt[])
		{TGTT_BUTTON_CAPTION, (tgtt) "[ Add item    ]",
		 TGTT_X, 23, TGTT_Y, 3,
		 TGTT_ID, 100, TGTT_CALLBACK, (tgtt) command,
    /* the ID will be also available in 'id' field of object structure,
       thus allowing us to use one callback for several controls
       (see command() below) */
		 TGTT_END,0});
    tgt_createandlink(window1, TGT_CLASS_BUTTON, (tgtt[])
		{TGTT_BUTTON_CAPTION, (tgtt) "[ Remove item ]",
		 TGTT_X, 23, TGTT_Y, 4,
		 TGTT_ID, 101, TGTT_CALLBACK, (tgtt) command,
		 TGTT_END,0});
    tgt_createandlink(window1, TGT_CLASS_BUTTON, (tgtt[])
		{TGTT_BUTTON_CAPTION, (tgtt) "[ Change item ]",
		 TGTT_X, 23, TGTT_Y, 5,
		 TGTT_ID, 102, TGTT_CALLBACK, (tgtt) command,
		 TGTT_END,0});
    tgt_createandlink(window1, TGT_CLASS_BUTTON, (tgtt[])
		{TGTT_BUTTON_CAPTION, (tgtt) "[ Quit        ]",
		 TGTT_X, 23, TGTT_Y, 7,
		 TGTT_ID, 103, TGTT_CALLBACK, (tgtt) command,
		 TGTT_END,0});

    /* those 'll be descriptions of fields below */
    tgt_createandlink(window1, TGT_CLASS_LABEL, (tgtt[])
		{TGTT_LABEL_TEXT, (tgtt) "Name    :",
		 TGTT_X, 2, TGTT_Y, 13,
		 TGTT_END,0});
    tgt_createandlink(window1, TGT_CLASS_LABEL, (tgtt[])
		{TGTT_LABEL_TEXT, (tgtt) "Sex     :",
		 TGTT_X, 2, TGTT_Y, 14,
		 TGTT_END,0});
    tgt_createandlink(window1, TGT_CLASS_LABEL, (tgtt[])
		{TGTT_LABEL_TEXT, (tgtt) "Address :",
		 TGTT_X, 2, TGTT_Y, 15,
		 TGTT_END,0});
    tgt_createandlink(window1, TGT_CLASS_LABEL, (tgtt[])
		{TGTT_LABEL_TEXT, (tgtt) "Phone no:",
		 TGTT_X, 2, TGTT_Y, 16,
		 TGTT_END,0});

    /* and these'll be used, when the user highlights a position to display information
       about the record*/
    tgt_createandlink(window1, TGT_CLASS_LABEL, (tgtt[])
		{TGTT_XS, 26,
		 TGTT_X, 12, TGTT_Y, 13,
		 TGTT_ID, 1001, /* name */
		 TGTT_END,0}); 
    tgt_createandlink(window1, TGT_CLASS_LABEL, (tgtt[])
		{TGTT_XS, 26,
		 TGTT_X, 12, TGTT_Y, 14,
		 TGTT_ID, 1002, /* sex */
		 TGTT_END,0});
    tgt_createandlink(window1, TGT_CLASS_LABEL, (tgtt[])
		{TGTT_XS, 26,
		 TGTT_X, 12, TGTT_Y, 15,
		 TGTT_ID, 1003, /* address */
		 TGTT_END,0});
    tgt_createandlink(window1, TGT_CLASS_LABEL, (tgtt[])
		{TGTT_XS, 26,
		 TGTT_X, 12, TGTT_Y, 16,
		 TGTT_ID, 1004, /* phone # */
		 TGTT_END,0});


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
	addobjwindow = tgt_createobject(TGT_CLASS_WINDOW,(tgtt[])
		    {TGTT_XS, 40, TGTT_YS, 10,
		     TGTT_X, 0, TGTT_Y, 0,
		     TGTT_BG, TGT_COLOR_RED,
		     TGTT_WINDOW_TITLE, (tgtt) "Add an item",
		     TGTT_ID, 10000,
		     TGTT_END,0});

	tgt_createandlink(addobjwindow, TGT_CLASS_LABEL, (tgtt[])
		{TGTT_LABEL_TEXT, (tgtt) "Name    :",
		 TGTT_X, 2, TGTT_Y, 2,
		 TGTT_END,0});
	tgt_createandlink(addobjwindow, TGT_CLASS_LABEL, (tgtt[])
		{TGTT_LABEL_TEXT, (tgtt) "Sex     :",
		 TGTT_X, 2, TGTT_Y, 3,
		 TGTT_END,0});
	tgt_createandlink(addobjwindow, TGT_CLASS_LABEL, (tgtt[])
		{TGTT_LABEL_TEXT, (tgtt) "Address :",
		 TGTT_X, 2, TGTT_Y, 4,
		 TGTT_END,0});
        tgt_createandlink(addobjwindow, TGT_CLASS_LABEL, (tgtt[])
		{TGTT_LABEL_TEXT, (tgtt) "Phone no:",
		 TGTT_X, 2, TGTT_Y, 5,
		 TGTT_END,0});


	tgt_createandlink(addobjwindow, TGT_CLASS_STRING, (tgtt[])
		{TGTT_XS, 26, TGTT_STRING_MAX, 30,
		 TGTT_ID, 10001, /* name */
		 TGTT_X, 12, TGTT_Y, 2,
		 TGTT_END,0});
	tgt_createandlink(addobjwindow, TGT_CLASS_SELECTBOX, (tgtt[])
		{TGTT_XS, 26,
		 TGTT_SELECTBOX_ITEMS, 
		     (tgtt) (char*[]) {"Female", "Male", NULL},
		 TGTT_ID, 10002, /* sex */
		 TGTT_X, 12, TGTT_Y, 3,
		 TGTT_END,0});
	tgt_createandlink(addobjwindow, TGT_CLASS_STRING, (tgtt[])
		{TGTT_XS, 26, TGTT_STRING_MAX, 30,
		 TGTT_ID, 10003, /* address */
		 TGTT_X, 12, TGTT_Y, 4,
		 TGTT_END,0});
	tgt_createandlink(addobjwindow, TGT_CLASS_STRING, (tgtt[])
		{TGTT_XS, 26, TGTT_STRING_MAX, 30,
		 TGTT_ID, 10004, /* phone no */
		 TGTT_X, 12, TGTT_Y, 5,
		 TGTT_END,0});

	tgt_createandlink(addobjwindow, TGT_CLASS_BUTTON, (tgtt[])
		{TGTT_ID, 10005,
		 TGTT_X, 18, TGTT_Y, 7,
		 TGTT_BUTTON_CAPTION, (tgtt) "[ Add  ]",
		 TGTT_CALLBACK, (tgtt) add_command,
		 TGTT_END,0});
	tgt_createandlink(addobjwindow, TGT_CLASS_BUTTON, (tgtt[])
		{TGTT_ID, 10006,
		 TGTT_X, 2, TGTT_Y, 7,
		 TGTT_BUTTON_CAPTION, (tgtt) "[Cancel]",
		 TGTT_CALLBACK, (tgtt) add_command,
		 TGTT_END,0});

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
/* tgt_get attempts to fetch the tag, which code was passed as a second
   argument from the object, that a pointer to was passed as first one.
   if this suceeds, tag value is written to a variable, which a pointer to
   was passed as the last argument ;) */
	name[rec_count] = strdup(str);

	tgt_get(tgt_findchild(desktop, 10002),TGTT_SELECTBOX_SELECTED,(tgtt*) &s);
	    /* address */
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

/* this is going to be call whenever the selected item in list changes */
int chcallback(struct tgt_object *obj, int p)
{
    if(p>=0)
    {
	tgt_set(tgt_findchild(desktop,1001), TGTT_LABEL_TEXT, (tgtt) name[p]);
	tgt_set(tgt_findchild(desktop,1002), TGTT_LABEL_TEXT, (tgtt) (sex[p] ? male : female));
	tgt_set(tgt_findchild(desktop,1003), TGTT_LABEL_TEXT, (tgtt) addr[p]);
	tgt_set(tgt_findchild(desktop,1004), TGTT_LABEL_TEXT, (tgtt) phone[p]);
	tgt_refresh(desktop);
    }
    return(1);
}
