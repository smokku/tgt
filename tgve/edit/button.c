#include "tgt.h"
#include "tgt_acmacros.h"
#include "tgve.h"
#include <stdio.h>
#include <string.h>

static struct tgt_object * create_button(void);
static void delete_button(struct tgt_object * obj);
static void update_button(struct tgt_object * win, struct tgt_object * ref);
static void change_button(struct tgt_object * win, struct tgt_object * ref);

struct tgve_editorinfo button_handler={create_button,update_button,change_button,delete_button,
			(struct attributes_list[]) {tgve_attr_std_stuff,
			"Background while active", ID_BASE, ATYP_SELECTION, g_available_colors,
			"Button caption", ID_BASE+1, ATYP_STRING, NULL,
			 NULL, 0, ATYP_END, NULL}};


static struct tgt_object * create_button(void)
{
    struct tgt_object * obj;
    if(!(obj = tgt_createobject(TGT_CLASS_BUTTON, NULL))) return(NULL);
    tgt_set(obj, TGTT_BUTTON_CAPTION, (tgtt) strdup("Button"));
    return(obj);
}
static void update_button(struct tgt_object * win, struct tgt_object * ref)
{
    tgt_set(tgt_haschild(win, ID_BASE), TGTT_SELECTBOX_SELECTED, tgt_direct_get(ref, TGTT_BUTTON_ACTIVEBG));
    tgt_set(tgt_haschild(win, ID_BASE+1), TGTT_STRING_STRING, tgt_direct_get(ref, TGTT_BUTTON_CAPTION));
}
static void change_button(struct tgt_object * win, struct tgt_object * ref)
{
    unsigned char * oldcaption = (unsigned char*) tgt_direct_get(ref, TGTT_BUTTON_CAPTION);
    tgt_set(ref, TGTT_BUTTON_CAPTION, (tgtt) strdup((char*) tgt_direct_get(tgt_haschild(win, ID_BASE+1), TGTT_STRING_STRING)));
    tgt_set(ref, TGTT_BUTTON_ACTIVEBG, (tgtt) tgt_direct_get(tgt_haschild(win, ID_BASE), TGTT_SELECTBOX_SELECTED));
    free(oldcaption);
}

static void delete_button(struct tgt_object * obj)
{
    free((void*) tgt_direct_get(obj, TGTT_BUTTON_CAPTION));
}
	

