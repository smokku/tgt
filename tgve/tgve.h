
struct tgve_status
{
    struct tgt_object * last_clicked;
    unsigned short last_x, last_y;
    unsigned short drag_x, drag_y;
    int mode;
    int modearg;
    int drag_mode;
    unsigned short min_x, min_y;
};

struct tgve_objinfo
{
    int type;
    union
    {
	struct
	{
	    int class;
	    char virtual_id[20];
	    char virtual_callback[20];
	    int locked;
	    void * edowndata;
	} objdata;
	struct
	{
	    struct tgve_editorinfo * ei;
	    struct tgt_object * reference;
	} editordata;
	struct tgt_object * current_item_reference;
    };
};

#define WINTYPE_DESKTOP 0
#define WINTYPE_EDITOR 1
#define WINTYPE_OBJECT 2
#define WINTYPE_EDLIST 3

struct tgve_editorinfo
{
    struct tgt_object * (*create)(void);
    void (*update)(struct tgt_object *, struct tgt_object *);
    void (*change)(struct tgt_object *, struct tgt_object *);
    void (*delete)(struct tgt_object *);
    struct attributes_list * attrs;
};

struct attributes_list
{
    char *name;
    int id;
    int type;
    void * data;
};

#define ATYP_END -1
#define ATYP_BOOLEAN 0
#define ATYP_STRING 1
#define ATYP_SELECTION 2

#define MODE_IDLE 0
#define MODE_DRAG 1
#define MODE_DOWN 2

#define COLOR_DRAGFRAME1 TGT_COLOR_MAGNETA
#define COLOR_DRAGFRAME2 TGT_COLOR_CYAN

#define ID_STATUS 100
#define ID_QUIT 101
#define ID_DRAGMODE 103
#define ID_NEWOBJTYPE 104
#define ID_CREATLABEL 105
#define ID_ICCB 106

#define ID_ACCEPT 200
#define ID_CANCEL 201
#define ID_PROPLABEL 202

#define ID_COMMON 250

#define ID_BASE 300


#define DM_CREATE 0
#define DM_MOVE 1
#define DM_RESIZE 2
#define DM_DELETE 3
#define objinfo(obj) ((struct tgve_objinfo*) ((obj)->user_data))

void framedraw(tgt_cell *buff, int xsize, int sx, int sy, int dx, int dy);
void change_mode(struct tgt_object * obj, int m);
void new_object(struct tgt_object * p, struct tgt_object * r, struct tgt_object *o, int x1, int y1, int x2, int y2);
void new_window(struct tgt_object * r, struct tgt_object *o, int x1, int y1, int x2, int y2);
void operations(struct tgt_object * obj, int param);
int deliveryhandler(struct tgt_object * obj, int message, int a, char *b);
void set_status(char * format, ...);
struct tgve_objinfo * create_objdatainfo(int class, int vid);
struct tgve_objinfo * create_editordatainfo(struct tgve_editorinfo * ei, struct tgt_object *ref);
int lock_object(struct tgt_object * obj);
void unlock_object(struct tgt_object * obj);
void open_editor(struct tgt_object * obj, struct tgt_object * root);

#define tgve_attr_std_stuff g_fc_label, ID_COMMON,    ATYP_SELECTION, g_available_colors,\
			    g_bc_label, ID_COMMON+1,  ATYP_SELECTION, g_available_colors,\
			    g_id_label, ID_COMMON+2,  ATYP_STRING,    NULL,\
			    g_cb_label, ID_COMMON+3,  ATYP_STRING,    NULL

#ifndef DATA_FILE
extern char * object_names[];
extern tgtt* sample_taglists[];
extern int object_types[];
extern struct tgve_editorinfo* object_handlers[];
extern char * g_available_colors[];
extern char g_fc_label[];
extern char g_bc_label[];
extern char g_id_label[];
extern char g_cb_label[];
#endif

