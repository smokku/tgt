
//#define TGT_CLASS_MENU 6

#define TGTT_MENU_ITEMS 1000
#define TGTT_MENU_OPEN 1001
#define TGTT_MENU_FRAMECOLOR 1002
#define TGTT_MENU_SELCOLOR 1003
#define TGTT_MENU_HORIZONTAL 1006

#define TGTA_MENU_ADDITEM 1004
#define TGTA_MENU_ASSIGNSUBMENU 1005
#define TGTA_MENU_SWITCHSELECTABILITY 1007
#define TGTA_MENU_SWITCHSELECTABILITYLAST 1008
#define TGTA_MENU_SELECT 1009

#define TGTA_MENU_DELETEITEM 1010
#define TGTA_MENU_INSERTITEM 1011
#define TGTA_MENU_CHSUBMENU 1012
#define TGTA_MENU_CHANGEITEM 1013


struct tgt_int_menu
{
    int nitems;
    int active;
    struct tgt_int_menuitem
    {
	char *name;
	struct tgt_object *child;
	int selectable;
    } *items;
    int selcolor;
    int framecolor;
    int fdpos;
    struct tgt_object * parent;
};

struct tgt_int_menu_exchange
{
    int n;
    int q;
    void *p;
};
