#include "tgt.h"
#include "tgt_acmacros.h"
#include "tgve.h"
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

struct tgt_object * root;
struct tgve_status state = {NULL, 0, 0, 0, 0, MODE_IDLE, 0, DM_CREATE, 0, 0};

void set_status(char * format, ...)
{
    static char status_string[100];
    va_list al;
    va_start(al, format);
    vsnprintf(status_string, 100, format, al);
    tgt_set(tgt_findchild(root, ID_STATUS), TGTT_STATUS_TEXT, (tgtt) status_string);
    tgt_refresh(tgt_findchild(root, ID_STATUS));
    va_end(al);
}

int main(int argc, char **argv)
{
    /* basic application setup */
    struct tgt_object * opwin;    
    
    root = tgt_start(NULL);
    root->user_data = create_objdatainfo(0,0);
    ((struct tgve_objinfo*) root->user_data)->type = WINTYPE_DESKTOP;
    
    tgt_createandlink(root, TGT_CLASS_STATUS, (tgtt[])
		{TGTT_BG, TGT_COLOR_RED, TGTT_FG, TGT_COLOR_WHITE, TGTT_ID, ID_STATUS, TGTT_END, 0});

    opwin = tgt_ac_mkwindow("Operations", TGT_AC_CUSTOM, (tgtt[]) 
		{AcChild, AcHorizGroup,
		     AcChild, SimpleLabel("Drag mode: "),
		     AcAlign(TGT_AC_HCENTER),
		     AcChild, Selectbox, 
		         SelectboxItems(((char*[]) {"Create", "Move", "Resize", "Delete", NULL})),
			 Callback(change_mode),
			 Width(15),
			 End,
		     AcEnd,
		AcChild, AcHorizGroup,
		     AcChild, SimpleLabelI("Create   : ", ID_CREATLABEL),
		     AcChild, Selectbox, 
			 SelectboxItems(object_names),
			 Id(ID_NEWOBJTYPE),
			 Width(15),
			 End,
		     AcEnd,
		AcSpace(1),
		AcChild, SimpleCheckboxIC("Creating inside window children allowed", 0, ID_ICCB, operations),
		AcChild, Label, Color(TGT_COLOR_GREEN), Text("    Use only if you know what are you doing"), End,
		AcSpace(1),
		AcChild, SimpleButtonIC("[ Quit             ]", ID_QUIT, operations),
		AcEnd}, 20, 70);


    tgt_link(opwin, root);

    tgt_refresh(root);
    set_status("Click & drag on the desktop to create windows");
    tgt_setdhandler(deliveryhandler);
    tgt_waitkeys(root);
}
