#include "tgt.h"
#include <stdio.h>

int main(int argc,char ** argv)
{
    struct tgt_object * desktop;
    desktop = tgt_start(NULL, NULL);
    tgt_easyrequest(desktop,"TGT Test",
			"  This is a TGT test program.\n"
			"It's designed to demonstrate you,\n"
			"possible TGT controls (objects) and\n"
			"to learn you how to navigate between objects\n\n",
			"[Continue]");
    tgt_easyrequest(desktop,"TGT Test",
			"  You'll be presented a menu of possible\n"
			"tests now. \n\n"
			"Switch between buttons using <Tab> or arrow keys;\n"
			"select an option using ENTER or SPACE\n\n",
			"[Continue]");

    tgt_destroyobject(desktop);
}
