#include "tgt.h"
#include <stdio.h>

struct tgt_object *desktop;

int main()
{
    int k;
    char buffer[100];

    desktop=tgt_start(NULL);
    tgt_refresh(desktop);
    buffer[0]=0;
    k=tgt_easyrequest(desktop,"This is a test","The thing you are currently watching\nis a tgt_easyrequest() result","[Ok]|[Cancel]");
    tgt_askstring(desktop,"Stringfield","Type something\nhere:","[Ok]",buffer,100);

    tgt_destroyobject(desktop);
    printf("%d\n%s\n",k,buffer);
    return(0);
}
