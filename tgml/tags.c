/* ----------------------------------------------------------------------------
**      tags.c TextGui Markup Language parser
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "tgmlp.h"
#include "tagprotos.h"

extern struct tgml_parserdata td;

void fatal(char *s)
{
    fprintf(stderr,"%s",s);
    exit(1);
}

void justify(void)
{
    int i,j;
    for(i=0, j=td.recursion_level*2;i<j;i++) printf(" ");
}

void jprintf(char *format, ...)
{
    va_list va;
    va_start(va, format);
    justify();
    vprintf(format, va);
    va_end(va);
}

char fbuffer[200];

char * color2const(char * str)
{
    sprintf(fbuffer,"%s",str);
    if(strcstr(str,"black")) sprintf(fbuffer,"TGT_COLOR_BLACK");
    if(strcstr(str,"red")) sprintf(fbuffer,"TGT_COLOR_RED");
    if(strcstr(str,"green")) sprintf(fbuffer,"TGT_COLOR_GREEN");
    if(strcstr(str,"yellow")) sprintf(fbuffer,"TGT_COLOR_YELLOW");
    if(strcstr(str,"blue")) sprintf(fbuffer,"TGT_COLOR_BLUE");
    if(strcstr(str,"magneta")) sprintf(fbuffer,"TGT_COLOR_MAGNETA");
    if(strcstr(str,"cyan")) sprintf(fbuffer,"TGT_COLOR_CYAN");
    if(strcstr(str,"grey")) sprintf(fbuffer,"TGT_COLOR_GREY");
    return(fbuffer);
}

char * align2const(char * str)
{
    fbuffer[0]=0;
    if(strcstr(str,"left")) 
	sprintf(fbuffer,"TGT_AC_LEFT");
    else if(strcstr(str,"right"))
	sprintf(fbuffer,"TGT_AC_RIGHT");
    if(strcstr(str,"top")) 
	sprintf(fbuffer,"%s%sTGT_AC_TOP", fbuffer, strlen(fbuffer)?"|":"");
    else if(strcstr(str,"bottom"))
	sprintf(fbuffer,"%s%sTGT_AC_BOTTOM", fbuffer, strlen(fbuffer)?"|":"");
    if(strcstr(str,"hcenter")) 
	sprintf(fbuffer,"%s%sTGT_AC_HCENTER", fbuffer, strlen(fbuffer)?"|":"");
    if(strcstr(str,"vcenter"))
	sprintf(fbuffer,"%s%sTGT_AC_VCENTER", fbuffer, strlen(fbuffer)?"|":"");
    return(fbuffer);
}

void p_genericobject(struct tgml_taginfo *ti, char * classname)
{
    char *id, *xs, *ys, *fgcolor, *bgcolor, *cb, *align;
    if((align = tgml_lookup(ti, "align"))) jprintf("TGT_AC_ALIGNMENT, %s,\n",align2const(align));
    jprintf("TGT_AC_CHILD, %s, \n",classname);
    td.recursion_level++;
    if((xs = tgml_lookup(ti, "width"))) jprintf("TGTT_XS, %s,\n",xs);
    if((ys = tgml_lookup(ti, "height"))) jprintf("TGTT_YS, %s,\n",ys);
    if((fgcolor = tgml_lookup(ti, "fgcolor"))) jprintf("TGTT_FG, %s,\n",color2const(fgcolor));
    if((bgcolor = tgml_lookup(ti, "bgcolor"))) jprintf("TGTT_BG, %s,\n",color2const(bgcolor));
    if((cb = tgml_lookup(ti, "callback"))) jprintf("TGTT_CALLBACK, (tgtt) (%s),\n",cb);
    if((id = tgml_lookup(ti, "id"))) jprintf("TGTT_ID, %s,\n",id);
}





void p_genericparse(struct tgml_taginfo * ti, void (*fn)(struct tgml_taginfo *ti), char * classname, char * tagname)
{
    if(ti->negative) 
    {
	fprintf(stderr,"</%s>\n",tagname);
	fatal("PARSE ERROR: this tag does not exist or can not be used in that context\n");
    }
    p_genericobject(ti, classname);
    if(fn) fn(ti);
    jprintf("TGTT_END, 0, \n");
    td.recursion_level --;
}

void q_itemtag_tp(struct tgml_taginfo *ti)
{
    char * str;
    
    if(strccmp(ti->tag_name,"list") || strccmp(ti->tag_name,"selectbox"))
	if(ti->negative)
	{
	    td.tagparser = NULL;
	    if(td.internal > 0)
	    {
		jprintf(", NULL},\n");
		td.recursion_level --;
	    }
	    jprintf("TGTT_END,0,\n");
	    td.recursion_level --;
	    return;
	}
    if(! strccmp(ti->tag_name,"item"))
	fatal("PARSE ERROR: No other tags than <item> allowed inside <list> or <selectbox>\n");

    if(ti->negative) 
	fatal("PARSE ERROR: </item> tag does not exist\n");
    if((str = tgml_lookup(ti,"string")))
    {
	if(td.internal < 0)
	{
	    jprintf("TGTT_%s_ITEMS, (tgtt) (char*[]) {",td.internal==-1?"LIST":"SELECTBOX");
	    td.internal = 1;
	    td.recursion_level ++; 
	}
	else
	    printf(",\n");

	jprintf("\"%s\"",str);
    }
    else
	fatal("PARSE ERROR: <item> tag requires 'string' parameter\n");
}
void parsetag(struct tgml_taginfo * ti)
{
    char * name = ti->tag_name;
    if(td.tagparser) 
    {
	td.tagparser(ti);
	return;
    }
    if(strccmp(name, "window")) p_wintag(ti);
    if(strccmp(name, "group")) p_grptag(ti);
    if(strccmp(name, "add")) p_addtag(ti);
    if(strccmp(name, "menu")) p_menutag(ti);
    
    if(td.recursion_level < 1) return;
    if(strccmp(name, "button")) p_genericparse(ti,q_buttontag,"TGT_CLASS_BUTTON","button");
    if(strccmp(name, "checkbox")) p_genericparse(ti,q_cbtag,"TGT_CLASS_CHECKBOX","checkbox");
    if(strccmp(name, "direct")) p_genericparse(ti,NULL,"TGT_CLASS_DIRECT","direct");
    if(strccmp(name, "label")) p_genericparse(ti,q_labeltag,"TGT_CLASS_LABEL","label");
    if(strccmp(name, "slider")) p_genericparse(ti,q_slidertag,"TGT_CLASS_SLIDER","slider");
    if(strccmp(name, "string")) p_genericparse(ti,q_stringtag,"TGT_CLASS_STRING","string");
    if(strccmp(name, "progress")) p_genericparse(ti,q_progresstag,"TGT_CLASS_PROGRESS","progress");
    if(strccmp(name, "textbuffer")) p_genericparse(ti,q_tbtag,"TGT_CLASS_TEXTBUFFER","textbuffer");
    if(strccmp(name, "termemu")) p_genericparse(ti,NULL,"TGT_CLASS_TERMEMU","termemu");
    if(strccmp(name, "list")) p_listtag(ti);
    if(strccmp(name, "selectbox")) p_sboxtag(ti);
    if(strccmp(name, "space")) p_spacetag(ti);
    if(strccmp(name, "spacing")) p_spacingtag(ti);
    if(strccmp(name, "minsize")) p_minsizetag(ti);
    if(strccmp(name, "xfix")) jprintf("TGT_AC_FIX_X, 0,\n");
    if(strccmp(name, "yfix")) jprintf("TGT_AC_FIX_Y, 0,\n");
}
