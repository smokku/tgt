#define PREFS_FILE
#include "tgt.h"
#include <pwd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#define MAX_LINE 500
#define MAX_SECTION 20

struct tgt_confsection
{
    struct tgt_confsection *next;
    char *name;
    char *ptr;
};

struct tgt_prefs
{
    struct tgt_confsection *sections;
    char *file;
    int size;
    char *ibuffer;
};

char * tgt_skipto(char *str,char c) 
{
    for(;*str!=0 && *str!=c;str++);
    if(*str==0) 
	return(NULL);
    else
	return(str);
}
struct tgt_prefs * tgt_int_loadconfig(void)
{
    FILE *fh;
    struct tgt_prefs *ret;
    struct tgt_confsection *section;
    long size;
    char *file=NULL,*ts;
    char sbuffer[MAX_SECTION];
    char *fbuffer,*hdir;
    int i;
    struct passwd *pentry;
    
    if(!(fh=fopen(".tgtrc","r")))
    {
	if(pentry=getpwuid(getuid()))
	    if(hdir=pentry->pw_dir)
	    {
		file=(char*) malloc(strlen(hdir)+8);
		sprintf(file,"%s/.tgtrc",hdir);
		fh=fopen(file,"r");
		free(file);
	    }
	if(!fh) if(!(fh=fopen("/etc/tgtrc","r"))) return(NULL);
    }
    ret=(struct tgt_prefs*) malloc(sizeof(struct tgt_prefs));
    ret->ibuffer=(char*) malloc(MAX_LINE);
    fseek(fh,0,SEEK_END);
    size=ftell(fh);
    rewind(fh);
    file=(char*) malloc(size+1);
    file[size]=0;
    fread(file,size,1,fh);
    ret->file=file;
    ret->size=size;
    fclose(fh);
    section=(struct tgt_confsection*) ret;
    section->next=NULL;
    
    for(ts=file;*ts!=0;ts++)
    {
//	fprintf(stderr,"%s\n",ts);
	if(*ts=='#') if(!(ts=tgt_skipto(ts,'\n'))) break;
	if(*ts=='[')
	{
	    section->next=(struct tgt_confsection*) malloc(sizeof(struct tgt_confsection));
	    section=section->next;
	    section->next=NULL;
	    *(ts++)=0;
	    for(i=0;i<(MAX_SECTION-1) && ts[i]!=']' && ts[i]!=0;i++)
		sbuffer[i]=ts[i];
	    sbuffer[i]=0;
	    section->name=strdup(sbuffer);
	    section->ptr=ts;
	}
	if(*ts==0) break;
	if(!(ts=tgt_skipto(ts,'\n'))) break;
//	fprintf(stderr,"done");
    }
    return(ret);
}

char * tgt_getprefs(struct tgt_prefs* prfs, char *section, char *key,char *def)
{
    struct tgt_confsection *cs;
    int ns,nk,i,l;
    char *ptr;
    char *n,*c;
//    fprintf(stderr,"getting\n");

    if(!prfs) return(def);
//    fprintf(stderr,"getting\n");
//    fprintf(stderr,"Getting %s : %s",section,key);
    for(cs=prfs->sections;cs!=NULL;cs=cs->next) 
	if(strcmp(cs->name,section)==0) break;

    if(cs)
    {
	for(;;)
	{
	    ptr=cs->ptr;
	    if(strlen(ptr)<2) return(def);
	    if(n=strstr(ptr,key))
	    {
		n+=strlen(key);
		for(i=0;n[i]!=0 && (n[i]==' '|| n[i]=='\t');i++);

		if(n[i]=='=') 
		{
		    n+=i+1;
		    c=prfs->ibuffer;
		    for(i=0;n[i]!=0 && n[i]!='\n' && i<(MAX_LINE-1);i++)
			c[i]=n[i];
		    c[i]=0;
		    return(c);
		}
		if(n[i]==0) return(def);
		    
		ptr=n+1;
	    }
	    else
		break;

	}
    }
    return(def);
}


void * g_prefs;

extern int d_seconds,d_micros;
void tgt_init(void)
{
    g_prefs=(void*) tgt_int_loadconfig();
    d_seconds=atoi(tgt_getprefs((struct tgt_prefs*) g_prefs,"keyboard","esc_sec","0"));
    d_micros=atoi(tgt_getprefs((struct tgt_prefs*) g_prefs,"keyboard","esc_usec","200000"));
}

