/* ----------------------------------------------------------------------------
**      parse.c TextGui Markup Language parser
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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include "tgmlp.h"

void tgml_destroyto(struct tgml_tagoption * to)
{
    if(to->next) tgml_destroyto(to->next);
    free(to->name);
    free(to->contents);
    free(to);
}

void * tgml_destroyti(struct tgml_taginfo * ti)
{
    if(ti->options) tgml_destroyto(ti->options);
    free(ti->tag_name);
    free(ti);
    return(NULL);
}


unsigned char * cutstr(unsigned char * str, int pos, int length)
{
    unsigned char * ret = (unsigned char*) malloc(length+1);
    snprintf(ret, length+1, "%s", str+pos);
    return(ret);
}

unsigned char * cuttolast(unsigned char * str, int len, int *klen)
{
    int i, c = *str;
    for(i = 1; i<len; i++) 
	if(str[i] == c) break;

    *klen=i+1;
    if(i>=len) 
	return(NULL);
    else
	return(cutstr(str, 1, i-1));
}

void tgml_addkey(struct tgml_taginfo * ti, char * key, char * value)
{
    struct tgml_tagoption * to = (struct tgml_tagoption*) malloc(sizeof(struct tgml_tagoption));
    to->name = key;
    to->contents = value;
    to->next = ti->options;
    ti->options = to;
}

char * tgml_lookup(struct tgml_taginfo * ti, char * key)
{
    struct tgml_tagoption * to;
    for(to = ti->options; to; to = to->next)
    {
	if(strccmp(to->name, key)) return(to->contents);
    }

    return(NULL);
}

struct tgml_taginfo * tgml_fetchtag(unsigned char * mem, int length)
{
    struct tgml_taginfo * ret;
    int pos, len=0;
    
    if(*mem == '<') /* skip eventual < */
    {
	mem++;
	if((--length) < 0) return(NULL);
	len++;
    }
    ret = (struct tgml_taginfo*) malloc(sizeof(struct tgml_taginfo));
    ret->tag_name = NULL;
    ret->negative = 0;
    ret->options = NULL;
    if(*mem == '/')
    {
	mem++;
	len++;
	if((--length) < 0) 
	{
	    free(ret);
	    return(NULL);
	}
	ret->negative = 1;
    }
    /* fetch name */
    for(pos = 0; pos<length && !isblank(mem[pos]) && mem[pos]>=' ' && mem[pos]!='>'; pos++) len++;
    if(pos>=length) { free(ret); return(NULL); }
    ret->tag_name = cutstr(mem, 0, pos);
    ret->length = len;
    if(mem[pos] == '>') return(ret);
    
    /* fetch parameters */
    for(mem += pos, length -= pos, pos=0; pos<length; )
    {
	char * key, * value;
	/* skip white characters */
	for(;;)
	{
	    if(isblank(*mem) || *mem < ' ')
	    {
		mem++; len++; length--;
		pos = 0;
		if(length < 0) return(tgml_destroyti(ret));
	    }
	    else
		break;
	}
	
	/* end of tag ? */
	if(*mem == '<') return(tgml_destroyti(ret));
	if(*mem == '>') { len++; break; }
	
	/* fetch option name ... */
	for(pos = 0; mem[pos] != '=' && !isblank(mem[pos]) && pos<length; pos++);
	if(pos>=length || pos == 0) return(tgml_destroyti(ret));
	if(isblank(mem[pos])) return(tgml_destroyti(ret));
	key = cutstr(mem, 0, pos);
	mem += pos+1; len+=pos+1;
	length -= pos+1;
	if(length < 0) return(tgml_destroyti(ret)); // mem leak, key
	/* fetch option value */
	if(*mem == '"' || *mem=='\'')
	{
	    int l;
	    if(!(value = cuttolast(mem, length, &l))) return(tgml_destroyti(ret)); // mem leak, key
	    len+=l; mem+=l; length-=l;
	}
	else
	{
	    /* scan to first whitechar, or to > */
	    for(pos = 0; pos<length; pos++)
		if(isblank(mem[pos]) || mem[pos]<' ' || mem[pos]=='>') break;
	    if(pos>=length) return(tgml_destroyti(ret)); // mem leak, key
	    value = cutstr(mem, 0, pos);
	    len += pos;
	    mem += pos;
	    length -= pos;
	}
//	printf("%s: key ,,%s'' value ,,%s''\n",ret->tag_name,key,value);	
	tgml_addkey(ret, key, value);
    }
    ret->length = len;
    return(ret);
}

int strcstr(char *s1, char *s2)
{
    char * ls1 = (char*) malloc(strlen(s1)+1);
    char * k;
    int i, l = strlen(s1);
    for(i = 0; i<l; i++) ls1[i]=tolower(s1[i]);
    ls1[i] = 0;
    k = strstr(ls1, s2);
    if(k) return(1); else return(0);
}

int strccmp(char *s1, char *s2)
{
    int l1 = strlen(s1), l2 = strlen(s2);
    int i;
    if(l1!=l2) return(0);
    for(i = 0; i<l1; i++) if(tolower(s1[i]) != tolower(s2[i])) return(0);
    return(1);
}

