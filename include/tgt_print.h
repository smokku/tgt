#define TGT_COLOR_HIGHBRIGHT 128
#define TGT_COLOR_HIGHBRIGHTMASK 127
#define TGT_COLOR_DARKGREY TGT_COLOR_BLACK | TGT_COLOR_HIGHBRIGHT
#define TGT_COLOR_BRIGHTRED TGT_COLOR_RED | TGT_COLOR_HIGHBRIGHT
#define TGT_COLOR_BRIGHTGREEN TGT_COLOR_GREEN | TGT_COLOR_HIGHBRIGHT
#define TGT_COLOR_BRIGHTYELLOW TGT_COLOR_YELLOW | TGT_COLOR_HIGHBRIGHT
#define TGT_COLOR_BRIGHTBLUE TGT_COLOR_BLUE | TGT_COLOR_HIGHBRIGHT
#define TGT_COLOR_BRIGHTMAGENTA TGT_COLOR_MAGNETA | TGT_COLOR_HIGHBRIGHT
#define TGT_COLOR_BRIGHTCYAN TGT_COLOR_CYAN | TGT_COLOR_HIGHBRIGHT
#define TGT_COLOR_WHITE TGT_COLOR_GREY | TGT_COLOR_HIGHBRIGHT

void tgt_mvcladdstr(int x, int y, int fg, int bg, const char *str);
void tgt_mvcladdch(int x, int y, int fg, int bg, const char chr);
void tgt_claddstr(int fg, int bg, const char *str);
void tgt_claddch(int fg, int bg, const char chr);
void tgt_chclr(int fg, int bg);
void tgt_chfg(int fg);
