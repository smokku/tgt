#include "tgt.h"
#include "tgt_app.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define min(a,b) ((a) < (b) ? (a) : (b))

    struct tgt_object *desktop;
    struct tgt_object *blv;
    struct tgt_object *nw;
    struct tgt_object *string;
/* Callbacks */
void shutdown()
{
    tgt_destroyobject(desktop);
    _exit(0);
}
int smallcb(struct tgt_object *list,int pos)
{
    tgt_set(blv,TGTT_LIST_SELECTED,pos);
    tgt_refresh(blv);
    return(1);
}

struct listitem 
{
    char *item;
    struct listitem *next;
};
struct listitem *l_head;

int ladd(struct tgt_object *obj,int a)
{
    char *p;
    struct listitem *li;
    li=(struct listitem*) malloc(sizeof(struct listitem));
    tgt_get(string,TGTT_STRING_STRING,(tgtt*) &p);
    li->item=strdup(p);
    li->next=l_head->next;
    l_head->next=li;
    tgt_refresh(nw);
    return(1);
}

int ldel(struct tgt_object *obj,int a)
{
    struct listitem *li;
    struct listitem *lp;
    int i;
    li=l_head->next; lp=l_head;
    for(i=0;i<a && (li);i++,lp=li,li=li->next);
    if(li)
    {
	lp->next=li->next;
	free(li);
	tgt_refresh(nw);
    }
    return(1);
}

int ldcb(struct listitem *head,int req,int param,char *buffer,int max,int act)
{
    int i;
    struct listitem *q;
    if(req==TGT_LISTREQ_GETITEM)
    {
	q=head->next;
	for(i=0;q!=NULL && i<param;i++,q=q->next);
	if(q)
	{
	    memset(buffer,' ',max);
	    memcpy(buffer,q->item,min(strlen(q->item),max));
	    buffer[max]=0;
	}
	else
	    memset(buffer,' ',max);

	return(1);
    }
    else
    {
	q=head->next;
	for(i=0;q!=NULL;q=q->next,i++);
	return(i);
    }
}

void opennew(struct tgt_object *button)
{
    struct listitem *itm;
    if(tgt_haschild(desktop,112)==NULL)
    {
	nw = tgt_createandlink(desktop,TGT_CLASS_WINDOW,
		(tgtt[]) {TGTT_X,3, TGTT_Y,1,
			TGTT_XS,31, TGTT_YS,18,
			TGTT_WINDOW_TITLE, (tgtt) "List management",
			TGTT_ID, 112,
			TGTT_END,0});
	
	string = tgt_createandlink(nw,TGT_CLASS_STRING,
		(tgtt[]) {TGTT_X,3, TGTT_Y,15,
			TGTT_XS, 20,
			TGTT_STRING_MAX,26,
			TGTT_END,0} );
	tgt_createandlink(nw,TGT_CLASS_BUTTON,
		(tgtt[]) {TGTT_X,24, TGTT_Y,15,
			TGTT_BUTTON_CAPTION, (tgtt) "[Add]",
			TGTT_CALLBACK, (tgtt) ladd,
			TGTT_END,0} );

	l_head=(struct listitem*) malloc(sizeof(struct listitem));
	l_head->next=NULL;
	tgt_set(tgt_createandlink(nw,TGT_CLASS_LIST,
			(tgtt[]) {TGTT_X,2, TGTT_Y,2,
				TGTT_XS,27, TGTT_YS,13,
				TGTT_CALLBACK, (tgtt) ldel,
				TGTT_LIST_FRAMECOLOR, 6,
				TGTT_LIST_DATACALLBACK, (tgtt) ldcb,
				TGTT_END,0}),
		TGTT_LIST_ITEMS, (tgtt) l_head);

	tgt_setfocus(string);
	tgt_refresh(desktop);
    }
    else
    {
	tgt_unlink(nw);
	tgt_refresh(desktop);
	for(itm=l_head;l_head!=NULL;l_head=itm) { itm=l_head->next; free(l_head); }
	tgt_destroyobject(nw);
    }
}
int main()
{
	struct tgt_object *window;
	static char * list[] = {
			"TGT: Authors",
			"----",
			"Tomasz Sterna",
			"smoku",
			"smoku@jaszczur.org",
			"----",
			"Mateusz Golicz",
			"mteg",
			"mtg@elsat.net.pl",
			NULL
		    };

	desktop=tgt_start(NULL);

	window = tgt_createandlink(desktop,TGT_CLASS_WINDOW,
	 (tgtt[]) {TGTT_WINDOW_TITLE, (tgtt) "List Class Test",
		   TGTT_XS,49,TGTT_YS,13,
		   TGTT_END,0});
	tgt_set(window,TGTT_X,(tgt_getterminal()->x_size-49)/2);
	tgt_set(window,TGTT_Y,(tgt_getterminal()->y_size-13)/2);
	
	tgt_createandlink(window,TGT_CLASS_BUTTON,
	 (tgtt[]) {TGTT_X,40,TGTT_Y,10,
	 	   TGTT_BUTTON_CAPTION, (tgtt) "[Exit]",
		   TGTT_CALLBACK, (tgtt) shutdown,
		   TGTT_END,0});

	blv = tgt_createandlink(window,TGT_CLASS_LIST,
		(tgtt[]) {TGTT_X,4,TGTT_Y,2,
			TGTT_XS,20,TGTT_YS,6,
			TGTT_LIST_ACTIVEBG,1,
			TGTT_LIST_ITEMS, (tgtt) list,
			TGTT_END,0});
	tgt_createandlink(window,TGT_CLASS_LIST,
		(tgtt[]) {TGTT_X,26,TGTT_Y,2,
			TGTT_XS,20,TGTT_YS,6,
			TGTT_LIST_ITEMS, (tgtt) list,
			TGTT_LIST_FRAMECOLOR,6,
			TGTT_CALLBACK,(tgtt) smallcb,
			TGTT_END,0});

	tgt_createandlink(window,TGT_CLASS_LABEL,
		(tgtt[]) {TGTT_X,28,TGTT_Y,8,
			TGTT_LABEL_TEXT,(tgtt) "Try <ENTER> here",
			TGTT_END,9});

	tgt_createandlink(window,TGT_CLASS_BUTTON,
		(tgtt[]) {TGTT_X,2,TGTT_Y,10,
			TGTT_BUTTON_CAPTION, (tgtt) "[Open/Close AddList]",
			TGTT_CALLBACK, (tgtt) opennew,
			TGTT_END,0});

	tgt_refresh(desktop);
	tgt_chtimes(0,100000);
	tgt_waitkeys(desktop);
	return(1);
}
