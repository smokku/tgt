#include "tgt.h"
#include <stdio.h>

void tgt_mvcladdstr(struct tgt_terminal *term, int x, int y, int fg, int bg, const char *str)
{
    tgt_chattr(term,TGT_TA_CURSOR,x,y);
    if(fg & TGT_COLOR_HIGHBRIGHT) tgt_chattr(term,TGT_TA_BOLD,0,0); else tgt_chattr(term,TGT_TA_NORMAL,0,0);
    fg=fg & TGT_COLOR_HIGHBRIGHTMASK;
    tgt_chattr(term,TGT_TA_COLORS,fg,bg);
    printf("%s",str);
    fflush(stdout);
}

void tgt_mvcladdch(struct tgt_terminal *term, int x, int y, int fg, int bg, const char chr)
{
    tgt_chattr(term,TGT_TA_CURSOR,x,y);
    if(fg & TGT_COLOR_HIGHBRIGHT) tgt_chattr(term,TGT_TA_BOLD,0,0); else tgt_chattr(term,TGT_TA_NORMAL,0,0);
    fg=fg & TGT_COLOR_HIGHBRIGHTMASK;
    tgt_chattr(term,TGT_TA_COLORS,fg,bg);
    putchar(chr);
    fflush(stdout);
}

void tgt_claddstr(struct tgt_terminal *term, int fg, int bg, const char *str)
{
    if(fg & TGT_COLOR_HIGHBRIGHT) tgt_chattr(term,TGT_TA_BOLD,0,0); else tgt_chattr(term,TGT_TA_NORMAL,0,0);
    fg=fg & TGT_COLOR_HIGHBRIGHTMASK;
    tgt_chattr(term,TGT_TA_COLORS,fg,bg);
    printf("%s",str);
    fflush(stdout);
}

void tgt_claddch(struct tgt_terminal *term, int fg, int bg, const char chr)
{
    if(fg & TGT_COLOR_HIGHBRIGHT) tgt_chattr(term,TGT_TA_BOLD,0, 0); else tgt_chattr(term,TGT_TA_NORMAL,0, 0);
    fg=fg & TGT_COLOR_HIGHBRIGHTMASK;
    tgt_chattr(term,TGT_TA_COLORS,fg, bg);
    putchar(chr);
    fflush(stdout);
}

void tgt_chclr(struct tgt_terminal *term, int fg, int bg)
{
    if(fg & TGT_COLOR_HIGHBRIGHT) tgt_chattr(term,TGT_TA_BOLD,0, 0); else tgt_chattr(term,TGT_TA_NORMAL,0, 0);
    fg=fg & TGT_COLOR_HIGHBRIGHTMASK;
    tgt_chattr(term,TGT_TA_COLORS,fg, bg);
}

void tgt_chfg(struct tgt_terminal *term, int fg)
{
    if(fg & TGT_COLOR_HIGHBRIGHT) tgt_chattr(term,TGT_TA_BOLD,0, 0); else tgt_chattr(term,TGT_TA_NORMAL,0, 0);
    fg=fg & TGT_COLOR_HIGHBRIGHTMASK;
    tgt_chattr(term,TGT_TA_FGCOLOR,fg, 0);
}
