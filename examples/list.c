#include "tgt.h"
#include "tgt_app.h"
#include <stdio.h>
#include <string.h>
#define min(a,b) ((a) < (b) ? (a) : (b))

    struct tgt_object *desktop;
    struct tgt_terminal *myterm;
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
    tgt_get(string,TGTT_STRING_STRING,&p);
    li->item=strdup(p);
    li->next=l_head->next;
    l_head->next=li;
    tgt_refresh(nw);
}
int ldel(struct tgt_object *obj,int a)
{
    char *p;
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
	nw=tgt_createandlink(desktop,myterm,TGT_CLASS_WINDOW,(long[]) {TGTT_X,5,TGTT_Y,3,TGTT_XS,31,TGTT_YS,20,TGTT_WINDOW_TITLE,(long) "List management",TGTT_ID,112,TGTT_END,0});
	tgt_setfocus(nw);
	l_head=(struct listitem*) malloc(sizeof(struct listitem));
	l_head->next=NULL;
	tgt_set(tgt_createandlink(nw,myterm,TGT_CLASS_LIST,
		(long[]) {TGTT_X,3,TGTT_Y,3,TGTT_XS,26,TGTT_YS,10,
			  TGTT_CALLBACK,(long) ldel,TGTT_LIST_FRAMECOLOR,6,
			  TGTT_LIST_DATACALLBACK, (long) ldcb,
			  TGTT_END,0} ),TGTT_LIST_ITEMS,l_head);
	
	string=tgt_createandlink(nw,myterm,TGT_CLASS_STRING,(long[]) {TGTT_X,3,TGTT_Y,15,TGTT_XS,20,TGTT_STRING_MAX,26,TGTT_END,0} );
	tgt_createandlink(nw,myterm,TGT_CLASS_BUTTON,(long[]) {TGTT_X,24,TGTT_Y,15,TGTT_BUTTON_CAPTION,(long) "[Add]",TGTT_CALLBACK,(long) ladd,TGTT_END,0} );

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
    char sstring[256];
    int i;

    myterm=tgt_setscreen((void*)0);
    desktop=tgt_getdesktop(myterm);
    window=tgt_createandlink(desktop,myterm,TGT_CLASS_WINDOW,(long[]) {TGTT_X,10,TGTT_Y,5,TGTT_XS,60,TGTT_YS,15,TGTT_WINDOW_TITLE,(long) "List Class Test",TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_BUTTON,(long[]) {TGTT_X,50,TGTT_Y,12,TGTT_BUTTON_CAPTION,(long) "[Exit]",TGTT_CALLBACK,(long) shutdown,TGTT_END,0});
    blv=tgt_createandlink(window,myterm,TGT_CLASS_LIST,
	    (long[]) {TGTT_X,5,TGTT_Y,3,
		      TGTT_XS,20,TGTT_YS,6,
		      TGTT_LIST_ACTIVEBG,1,
		      TGTT_LIST_ITEMS,
     (long) (char* []){ "TGT: Authors","----","Thomas Sterna","smoku","smoku@linux-pl.com","----","Matthew Golicz","mteg","mtg@elsat.net.pl",NULL},
		      TGTT_END,0});
    tgt_createandlink(window,myterm,TGT_CLASS_LIST,
	    (long[]) {TGTT_X,26,TGTT_Y,3,
		      TGTT_XS,20,TGTT_YS,6,
		      TGTT_LIST_ITEMS,
     (long) (char* []){ "TGT: Authors","----","Thomas Sterna","smoku","smoku@linux-pl.com","----","Matthew Golicz","mteg","mtg@elsat.net.pl",NULL},
		      TGTT_LIST_FRAMECOLOR,6,
		      TGTT_CALLBACK,(long) smallcb,
		      TGTT_END,0});

    tgt_createandlink(window,myterm,TGT_CLASS_BUTTON,(long[]) {TGTT_X,2,TGTT_Y,12,TGTT_BUTTON_CAPTION,(long) "[Open/Close AddList]",TGTT_CALLBACK,(long) opennew,TGTT_END,0});

    tgt_refresh(desktop);
    tgt_chtimes(0,100000);
    tgt_waitkeys(desktop);
}
