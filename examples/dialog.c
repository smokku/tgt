#include "tgt.h"
#include <stdio.h>

struct tgt_object *desktop;

main()
{
    struct tgt_terminal *term;
    struct tgt_object *window;
    int k;

    desktop=tgt_start(NULL,&term); 
    tgt_refresh(desktop);    
    k=tgt_easyrequest(desktop,"This is a test","The thing you are currently watching\nis a tgt_easyrequest() result","[Ok]|[Cancel]",0);
    tgt_destroyobject(desktop);
    printf("%d\n",k);
}