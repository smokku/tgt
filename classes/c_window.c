#include <stdio.h>
#include "tgt.h"

void tgt_wint_drawtitleb(struct tgt_terminal *term,int a,char *b,int fg,int bg,int tit)
{
    int i,n,p;
    /* Rysuje 'belke tytulowa' okna */
    printf(term->c_graphics); /* Tryb graficzny ... */
    printf(term->c_fgcolor,fg); printf(term->c_bgcolor,bg); /* Kolorki ... */
    putchar(term->gfx_set[TGT_TC_UL]); p=term->gfx_set[TGT_TC_HL]; 
    /* naroznik i znaczek "przekreconego T" do rozpoczecia tytulu... */
    
    putchar(term->gfx_set[TGT_TC_TS]); printf(term->c_text);
    printf(term->c_fgcolor,tit);
    printf(" %s ",b);
    /* Tytul... */
    
    printf(term->c_fgcolor,fg);
    printf(term->c_graphics); putchar(term->gfx_set[TGT_TC_TE]); /* znak "przekreconego w druga strone T :)) */
    for(i=0,n=a-6-strlen(b);i<n;i++) putchar(p); /* cala reszta oprocz 2 znakow na narozniki + 2 na 'T' + 2 na odstepy i dlugosci
    stringa tytulowego to poziome kreski */
    putchar(term->gfx_set[TGT_TC_UR]); /* naroznik */
    term->fg=fg; term->bg=bg;
    return;
}
void tgt_wint_drawsideb(struct tgt_terminal *term,int x,int y,int l,int lxs)
{
    char *tmpbuff;
    int i;
    /* Rysuje boczna ramke okna poczynajac od pozycji x,y o ilosci
       kolumn lxs i wierszy l */
    tmpbuff=(char*) malloc(term->x_size+1);
    memset(tmpbuff,' ',term->x_size);
    tmpbuff[0]=term->gfx_set[TGT_TC_VL];  /* pionowa linia */
    tmpbuff[lxs-1]=term->gfx_set[TGT_TC_VL];
    tmpbuff[lxs]=0;
    /* Tworzymy sobie string pomocniczy , cos a'la "|         |" */
    
    
    for(i=0;i<l;i++)
    {
	/* No i teraz przemieszczamy kursor corazto nizej i wypluwamy
	   nasz string */
	printf(term->c_move,0,y+i,x);
	printf(tmpbuff);
    }
    /* Zwalniamy pomocniczy string */
    free(tmpbuff);
}

    static char defaulttitle[]="Window";
    int wswitches_next[]={'\t',TGT_KEY_DOWN,TGT_KEY_RIGHT,0};
    int wswitches_prev[]={TGT_KEY_NONE,TGT_KEY_UP,TGT_KEY_LEFT,0};

struct tgt_int_window
{
    int borderfg;
    int titlefg;
    char *title;
};

    static char defb=6,deftit=7;

int tgt_builtin_window(struct tgt_object *obj,int type,int a,void *b)
{
    int i,n;
    int act;
    struct tgt_int_window *iw;
    switch(type)
    {
	case TGT_OBJECT_CREATE:
	    iw=(struct tgt_int_window*) malloc(sizeof(struct tgt_int_window));
	    obj->class_data=iw;
	    obj->objectf=(int(*)()) tgt_getptrtag(b,TGTT_CALLBACK,NULL);
	    iw->title=(char*) tgt_getptrtag(b,TGTT_WINDOW_TITLE,defaulttitle);
	    iw->borderfg=tgt_getnumtag(b,TGTT_WINDOW_BORDERCOLOR,defb);
	    iw->titlefg=tgt_getnumtag(b,TGTT_WINDOW_TITLECOLOR,deftit);
            if(!(obj->next_keys)) obj->next_keys=wswitches_next;
	    if(!(obj->prev_keys)) obj->prev_keys=wswitches_prev;		
	    return(1);
	case TGT_OBJECT_DESTROY:
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    act=tgt_isactive(obj);
	    if(act==1) tgt_chattr(obj->term,TGT_TA_BOLD,0,0);
	    iw=obj->class_data;
	    tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a,(void*) ((int) (obj->y+(int) b)));
	    tgt_wint_drawtitleb(obj->term,obj->xs,iw->title,iw->borderfg,obj->bg,iw->titlefg);
	    
	    tgt_wint_drawsideb(obj->term,obj->x+a,obj->y+1+(int) b,obj->ys-2,obj->xs);
	    tgt_chattr(obj->term,TGT_TA_CURSOR,obj->x+a,obj->y+(int) b+obj->ys-1);
	    tgt_int_lowerb(obj->term,obj->xs);
	    if(act==1) tgt_chattr(obj->term,TGT_TA_NORMAL,0,0);
	    tgt_chattr(obj->term,TGT_TA_TXT,0,0);
	    fflush(stdout);
	    return(1);
	case TGT_OBJECT_HANDLE:
	    n=tgt_shalliswitch(obj,a,0);
	    if(n<0) { tgt_activateprev(obj); return(1); }
	    if(n>0) { tgt_activatenext(obj); return(1); }
// in future eventually window moving (for instance) functions by keys
// like alt + -> 
            if(obj->objectf) return(obj->objectf(obj,a));
	    return(0);
	    break;
	case TGT_OBJECT_SETTAG:
	    iw=obj->class_data;
	    switch(a)
	    {
		case TGTT_WINDOW_TITLE:	iw->title=(char*) b; return(1);
		case TGTT_WINDOW_BORDERCOLOR: iw->borderfg=(int) b; return(1);
		case TGTT_WINDOW_TITLECOLOR: iw->titlefg=(int) b; return(1);
	    }
	    return(0);
	case TGT_OBJECT_SETDEFAULTS:
	    defb=atoi(tgt_getprefs(b,"window","framecolor","6"));
	    deftit=atoi(tgt_getprefs(b,"window","titlecolor","7"));
	    return(1);
	default: return(0);
    }
}
