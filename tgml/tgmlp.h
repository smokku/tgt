/* ----------------------------------------------------------------------------
**      tgmlp.h TextGui Markup Language parser
** ----------------------------------------------------------------------------
**      TGT                                       TextGUIToolkit Library
**      Copyright (C)   2000-2001       Mateusz Golicz and Tomasz Sterna
**
**      LGPL license
**
** ----------------------------------------------------------------------------
**      MAINTAINER      Mateusz Golicz
**
**      Log:
**
**
*/

struct tgml_tagoption
{
    struct tgml_tagoption * next;
    char * name;
    char * contents;
};

struct tgml_taginfo
{
    int negative;
    int length;
    char * tag_name;
    struct tgml_tagoption * options;
};

struct tgml_taginfo * tgml_fetchtag(unsigned char * mem, int length);
char * tgml_lookup(struct tgml_taginfo * ti, char * key);
void * tgml_destroyti(struct tgml_taginfo * ti);

struct tgml_parserdata
{
    int parsing_window;
    int recursion_level;
    int first_window;
    void (*tagparser)(struct tgml_taginfo * ti);
    int internal;
};
void parsetag(struct tgml_taginfo * ti);

int strcstr(char *s1, char *s2);
int strccmp(char *s1, char *s2);
