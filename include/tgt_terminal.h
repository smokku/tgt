

#define TGT_TA_BGCOLOR 0
#define TGT_TA_FGCOLOR 1
#define TGT_TA_CLEAR 2
#define TGT_TA_BOLD 3
#define TGT_TA_NORMAL 4
#define TGT_TA_GFX 5
#define TGT_TA_TXT 6
#define TGT_TA_CURSOR 7
#define TGT_TA_DNBORDER 8
#define TGT_TA_TITLEBORDER 9
#define TGT_TA_VBORDER 10
#define TGT_TA_COLORS 11
#define TGT_TA_VISIBLECURSOR 12
#define TGT_TA_INVISIBLECURSOR 13
/* for usage with tgt_chattr() */


#define TGT_TC_HL 0
#define TGT_TC_VL 1
#define TGT_TC_TS 2
#define TGT_TC_TE 3
#define TGT_TC_UL 4
#define TGT_TC_UR 5
#define TGT_TC_LL 6
#define TGT_TC_LR 7

/* Indexes for semigraphic character mapping table, see below,
    HL- horizontal line, TS - title Start, UL- upper-left corner
    LR- lower right corner etc */

typedef unsigned short tgt_cell;

struct tgt_terminal
{
    char *termcapname;	/* Unused */
    short x_size;
    short y_size; 
    unsigned char color_fg,color_bg; /* _default_ object codes */
    char gfx_set[8]; /* semigraphic characters map, see indexes above&lowlevel.c*/
    char *c_clear;   /* Terminal clearing sequence */
    char *c_fgcolor; /* Sequence for changing frogeround color , %d gets replaced with code*/
    char *c_bgcolor; /* Sequence for changing background color , %d gets replaced with code*/
    char *c_active;  /* Bold on sequence */
    char *c_inactive;/* Bold off sequence */
    char *c_graphics;/* Switch to semigraphics characters set */
    char *c_text;    /* Switch to text characters set */
    char *c_move;    /* Move cursor to ...  */
    char *c_enablecurs; /* Show cursor */
    char *c_disablecurs;/* Hide cursor */
    char fg,bg;      /* Currently set colors */
    struct tgt_keynode * lookup_root; /* Keyboard lookup table */
    tgt_cell * terminal_buffer;
    tgt_cell * contents_buffer;
};

#define TGT_COLOR_BLACK 0
#define TGT_COLOR_RED 1
#define TGT_COLOR_GREEN 2
#define TGT_COLOR_YELLOW 3
#define TGT_COLOR_BLUE 4
#define TGT_COLOR_MAGNETA 5
#define TGT_COLOR_CYAN 6
#define TGT_COLOR_GREY 7

#define TGT_T_BOLD(cell,k) (((cell) & 0xf7ff) | ((k) << 11))
#define TGT_T_BG(cell,k)   (((cell) & 0xf8ff) | ((k) << 8 ))
#define TGT_T_FG(cell,k)   (((cell) & 0x8fff) | ((k) << 12))
#define TGT_T_CHAR(cell,k) (((cell) & 0xff00) |  (k) )
#define TGT_T_FCHAR(cell,k) ((cell) | (k))

#define TGT_T_GETBOLD(cell) ((cell) & 0x0800)
#define TGT_T_GETBG(cell)   (((cell) >> 8 ) & 7)
#define TGT_T_GETFG(cell)   (((cell) >> 12) & 7)
#define TGT_T_GETCHAR(cell) ((cell) & 0xff)

#define TGT_T_BUILDCELL(fg,bg,bold,gfx,c) ((c) | ((bg) << 8) | ((bold) << 11) | ((fg) << 12) | ((gfx) << 15))
