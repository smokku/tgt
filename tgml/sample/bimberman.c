/**
 ** ripped from a project attempt
 ** a nice example of tgt + tgml
 **/

#include <tgt/tgt.h>
#include <stdio.h>

extern struct tgt_object * create_connect();
extern struct tgt_object * create_players();

/* Prototypes of functions, that'll be provided by TGML */

struct tgt_object * desk;

int command(struct tgt_object * obj, int p)
{
    if(obj->id==100)
    {
	tgt_destroyobject(desk);
	exit(0);
    }
}

int main(int argc, char ** argv)
{
    struct tgt_object * window[2];
    tgtt colors, heads;
    
    desk = tgt_start(NULL); /* Enable TGT */
    window[0] = create_connect(); /* Create form */
    window[1] = create_players(); /* Create form */
    
    tgt_set(window[0], TGTT_X, 0); /* Position windows on desktop */
    tgt_set(window[0], TGTT_Y, 0);
    tgt_set(window[1], TGTT_X, 5);
    tgt_set(window[1], TGTT_Y, 4);

    tgt_link(window[1], desk); /* Place window on the desktop */
    tgt_link(window[0], desk); /* Place window on the desktop */

    tgt_get(tgt_findchild(desk, 911), TGTT_SELECTBOX_ITEMS, &colors); /* get objects properties */
    tgt_get(tgt_findchild(desk, 912), TGTT_SELECTBOX_ITEMS, &heads); /* finding objects by id */

    tgt_set(tgt_findchild(desk, 921), TGTT_SELECTBOX_ITEMS, colors); /* and apply them */
    tgt_set(tgt_findchild(desk, 922), TGTT_SELECTBOX_ITEMS, heads);
    tgt_set(tgt_findchild(desk, 931), TGTT_SELECTBOX_ITEMS, colors);
    tgt_set(tgt_findchild(desk, 932), TGTT_SELECTBOX_ITEMS, heads);
    tgt_set(tgt_findchild(desk, 941), TGTT_SELECTBOX_ITEMS, colors);
    tgt_set(tgt_findchild(desk, 942), TGTT_SELECTBOX_ITEMS, heads);
    
    tgt_findchild(desk, 900)->objflags |= TGT_OBJFLAGS_NONSELECTABLE; /* set messages-list nonselectable */

    tgt_refresh(desk); /* Update desktop  */
    tgt_waitkeys(desk); /* Wait for input */
}
