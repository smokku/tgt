#define DATA_FILE

#include "tgt.h"
#include "tgve.h"
#include <stdio.h>

char * object_names[]={"Button", "Checkbox", "DSurface", "Label", "List", "Progress",
                       "Selectbox", "Slider", "Status", "String", "Termemu", "Textbuffer",NULL,"Window"};

int object_types[]={TGT_CLASS_BUTTON, TGT_CLASS_CHECKBOX, TGT_CLASS_DIRECTSURFACE, TGT_CLASS_LABEL,
			    TGT_CLASS_LIST, TGT_CLASS_PROGRESS, TGT_CLASS_SELECTBOX, TGT_CLASS_SLIDER,
			    TGT_CLASS_STATUS, TGT_CLASS_STRING, TGT_CLASS_TERMEMU, TGT_CLASS_TEXTBUFFER,0,
			    TGT_CLASS_WINDOW};


tgtt* sample_taglists[]={	(tgtt[]) {TGTT_BUTTON_CAPTION,(tgtt) "Button", TGTT_END,0},
				(tgtt[]) {TGTT_CHECKBOX_CAPTION, (tgtt) "Checkbox", TGTT_END,0},
				(tgtt[]) {TGTT_BG, TGT_COLOR_BLACK, TGTT_END, 0},
				(tgtt[]) {TGTT_LABEL_TEXT, (tgtt) "Label", TGTT_END,0},
				(tgtt[]) {TGTT_LIST_ITEMS, (tgtt) (char*[]) {"Item1", "Item2", "Item3", NULL}, TGTT_LIST_FRAMECOLOR, TGT_COLOR_CYAN, TGTT_END,0},
				(tgtt[]) {TGTT_PROGRESS_CAPTION, (tgtt) "Pgrc", TGTT_PROGRESS_VALUE, 30, TGTT_END,0},
				(tgtt[]) {TGTT_SELECTBOX_ITEMS, (tgtt) (char*[]) {"Item1", "Item2", "Item3", NULL}, TGTT_END,0},
				(tgtt[]) {TGTT_SLIDER_CAPTION, (tgtt) "Slider", TGTT_SLIDER_VALUE, 30, TGTT_END,0},
				(tgtt[]) {TGTT_STATUS_TEXT, (tgtt) "Status", TGTT_END,0},
				(tgtt[]) {TGTT_STRING_STRING, (tgtt) "String field", TGTT_END,0},
				(tgtt[]) {TGTT_BG, TGT_COLOR_BLACK, TGTT_END, 0},
				(tgtt[]) {TGTT_BG, TGT_COLOR_BLACK, TGTT_END, 0},
				NULL,
				NULL};


extern struct tgve_editorinfo button_handler;

struct tgve_editorinfo* object_handlers[]={&button_handler,&button_handler,&button_handler,&button_handler,&button_handler,&button_handler,&button_handler,&button_handler,&button_handler,&button_handler,&button_handler,&button_handler,
				&button_handler,&button_handler};

char * g_available_colors[]={"Black","Red","Green","Yellow","Blue","Magneta","Cyan","Gray",NULL};
char g_fc_label[]="Foreground color";
char g_bc_label[]="Background color";
char g_id_label[]="Object identifier";
char g_cb_label[]="Object callback";
