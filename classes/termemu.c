#ifndef TGT_NOPTHREAD
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <limits.h>
#include <pty.h>
#include <sys/poll.h>
#include <ctype.h>
#include "tgt.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <paths.h>
#define DEBUG_STDERR
#undef DEBUG_STDERR
#define DEBUG_PIPE
#undef DEBUG_PIPE

    extern int	tgt_preferred_x,tgt_preferred_y;

struct tgt_launcher_info
{
    int master;
    int slave;
    int pipe;
    pid_t pid;
    struct tgt_object *obj;
    pthread_t thid;
    pid_t childpid;
};
struct tgt_int_termemuinfo
{
    struct tgt_object * obj;
    int cx,cy;
    int pos;
    unsigned char firsttime;
    tgt_cell attr;
    unsigned char xs,ys;
    unsigned char srs,sre;
    struct tgt_launcher_info *li;
};

static struct tgt_int_termemu_ldata
{
    int (*interpreter)(char*,int,struct tgt_int_termemuinfo*);
} 
termemu_sequence_codes[128];

static char already_initialized_sequence_codes=0;
void tgt_termemu_initsequencecodes(void);

static inline void tgt_termemu_write(int fd,int key)
{
    unsigned char c;
    switch(key)
    {
	case TGT_KEY_UP:    write(fd,"\033[A",3); break;
	case TGT_KEY_DOWN:  write(fd,"\033[B",3); break;
	case TGT_KEY_RIGHT: write(fd,"\033[C",3); break;
	case TGT_KEY_LEFT:  write(fd,"\033[D",3); break;
	case TGT_KEY_BKSPC: write(fd,"\010",1);   break;
	case TGT_KEY_DELETE:write(fd,"\177",1);   break;
	default:
	    c=key;
	    write(fd,&c,1);
	    break;
    }
}

int tgt_builtin_termemu(struct tgt_object *obj,int type,int a,void *b)
{
    struct tgt_int_termemuinfo * id;
    int n;
    switch(type)
    {
	case TGT_OBJECT_GETSIZES:
	    ((struct tgt_ac_objectinfo*) b)->xsize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_XS,1);
	    ((struct tgt_ac_objectinfo*) b)->ysize=tgt_getsizetag((tagitem*)(((struct tgt_ac_objectinfo*) b)->ctl),TGTT_YS,1);
	    return(1);				 
	case TGT_OBJECT_CREATE:
	    obj->class_data=(void*) id=(struct tgt_int_termemuinfo*) malloc(sizeof(struct tgt_int_termemuinfo));
	    id->obj=obj;
	    id->cx=0; id->cy=0; id->attr=TGT_T_BUILDCELL(obj->fg,obj->bg,0,0,0); id->pos=0; id->li=NULL;
	    id->xs=obj->xs; id->ys=obj->ys; id->srs=0; id->sre=obj->ys-1;
	    id->firsttime=1;
	    return(1);
	case TGT_OBJECT_DESTROY:
	    free(obj->class_data);
	    return(1);
	case TGT_OBJECT_REFRESH:
	    id=(struct tgt_int_termemuinfo*) obj->class_data;
	    if(id->firsttime)
	    {
		tgt_fill_buffer(obj->visual_buffer,obj->xs,obj->ys,TGT_T_FCHAR(id->attr,' '));
		id->firsttime=0;
	    }
	    if(tgt_hasfocus(obj)) 
	    {
		tgt_preferred_x=((struct tgt_int_termemuinfo*)obj->class_data)->cx + obj->x + a;
		tgt_preferred_y=((struct tgt_int_termemuinfo*)obj->class_data)->cy + obj->y + (int) b;
	    }
	    return(1);
        case TGT_OBJECT_HANDLE:
	    id=obj->class_data;
	    n=tgt_shalliswitch(obj,a,1);
	    if(n<0) { tgt_activateprev(obj); return(1); }
	    if(n>0) { tgt_activatenext(obj); return(1); }
	    if(id->li) 
	    {
		tgt_termemu_write(id->li->master,a);
		return(1);	    
	    }
	default: return(0);
    }
}


void tgt_int_chldhandler(int i)
{
    while(wait3(NULL,WNOHANG,(struct rusage * ) 0)>0);
}

void usrhandler(int k)
{
}

struct tgt_launcher_message
{
    int type;
    union
    {
	char command[252];
	struct { int x,y; } size;
    } data;
};

#define TGT_LAUNCHERM_COMMAND 1
#define TGT_LAUNCHERM_ATTACH 2


void * tgt_launch_launcher(void)
{
    pid_t pid;
    struct tgt_launcher_info * li;
    int master,slave,fslave;
    int pipes[2];
    char name[PATH_MAX];
    
    openpty(&master,&slave,name,NULL,NULL);
    if(socketpair(AF_UNIX,SOCK_STREAM,0,pipes)==-1)  return(NULL);
				    
    
    switch((pid=fork()))
    {
	case 0:
	    close(master);
	    if((fslave=open(_PATH_TTY,O_RDWR))!=-1)
	    {
	        close(slave);
		ioctl(fslave, TIOCNOTTY, (char *)0);
		close(fslave);
	        if((fslave=open(name,O_RDWR|O_NOCTTY)))
		{
		    setsid();
		    ioctl(fslave, TIOCSCTTY, (char *)0);
		}
		else
		    fslave=slave;
	    }
	    else
		fslave=slave;
		
	    
	    
	    close(pipes[1]);
	    {
		int fd=pipes[0],k;
		char *strr;
		struct tgt_launcher_message msg;
		int left,status;
		pid_t childpid=0;
		
		if(fslave!=1) dup2(fslave,1);
		if(fslave!=2) dup2(fslave,2);
		if(fslave!=0) dup2(fslave,0);
		if(fslave>2) close(fslave);
		signal(SIGUSR2,tgt_int_chldhandler);
		for(;;)
		{
		    for(left=256,strr=(char*) &msg;left!=0;)
		    {
			k=read(fd,strr,left);
			if(k<1)  _exit(0);
			left-=k; strr+=k;
		    }
		    switch(msg.type)
		    {
			case TGT_LAUNCHERM_COMMAND:			
			    signal(SIGCHLD,tgt_int_chldhandler);
			    switch(childpid=fork())
			    {
				case -1: break;
				case 0:
				    execlp("/bin/sh","sh","-c",msg.data.command,NULL);
				    _exit(0);
				    break;
				default:
				    write(fd,&childpid,sizeof(pid_t));
				    wait(&status);
				    break;
			    }
			    break;
			case TGT_LAUNCHERM_ATTACH:
			    {
				char buffer[30];
				snprintf(buffer,29,"%d",msg.data.size.x);
				setenv("COLUMNS",buffer,1);
				snprintf(buffer,29,"%d",msg.data.size.y);
				setenv("LINES",buffer,1);
				{
				    struct winsize ws;
				    bzero((char *)&ws, sizeof(ws));
				    ws.ws_col = msg.data.size.x;
				    ws.ws_row = msg.data.size.y;
				    ioctl(0, TIOCSWINSZ, (char *)&ws);
				}
	    		    }
			    break;
		    }
		}
	    }
	    _exit(0);
	case -1:
	    return(NULL);
	default:
	    li=(struct tgt_launcher_info*) malloc(sizeof(struct tgt_launcher_info));
	    li->pid=pid;
	    li->master=master;
	    li->slave=slave;
	    li->pipe=pipes[1];
	    li->obj=NULL;
	    close(pipes[0]); 
	    return((void*) li);
    }    
}

void * tgt_launcher_system(void *li,char *name)
{
    struct tgt_launcher_message msg;

    kill(((struct tgt_launcher_info*) li)->pid,SIGUSR2);
    msg.type=TGT_LAUNCHERM_COMMAND;
    write(((struct tgt_launcher_info*) li)->slave,"\033[0m\033[H\033[J",10);
    snprintf(msg.data.command,251,"%s",name);
    write(((struct tgt_launcher_info*) li)->pipe,(char*) &msg,256);
    read(((struct tgt_launcher_info*) li)->pipe,&((struct tgt_launcher_info*) li)->childpid,sizeof(pid_t));
    return(NULL);
}

void tgt_launcher_kill(void *li)
{
    kill(((struct tgt_launcher_info*) li)->childpid,SIGKILL);
}

int tgt_launcher_getchildpid(void *li)
{
    return(((struct tgt_launcher_info*) li)->childpid);
}

inline static void tgt_int_termemu_scroll(tgt_cell * surface,int from,int to,int xs,tgt_cell attr)
{
    int x;
    tgt_cell * surfce;
    memmove(surface+(xs*from),surface+(xs*(from+1)),((to-from)*xs)<<1);
    surfce=surface + (xs*to);
    for(x=0;x<xs;x++) *(surfce++)=TGT_T_FCHAR(attr,' ');
}

inline static int tgt_int_buffertask_lineup(struct tgt_object *obj,struct tgt_int_termemuinfo * kti)
{
    tgt_cell cd=kti->attr;

    kti->cy--;
    if(kti->cy>=kti->srs && kti->cy<=kti->sre)
    {
	tgt_int_termemu_scroll(kti->obj->visual_buffer,kti->srs,kti->sre,kti->xs,cd);
	kti->cy=kti->sre;
	kti->cx=0;
    }
    else
    {
        kti->cy--;
	kti->cx=0;
    }
    tgt_refresh(obj);
    return(kti->pos=kti->cy*kti->xs+kti->cx);
}
void tgt_int_buffertask(struct tgt_launcher_info* ti)
{
    struct tgt_object *obj=ti->obj;
    int fh=ti->master;
    struct tgt_int_termemuinfo * kti=obj->class_data;
    unsigned char c;
    int pos=0,max,l;
#ifdef DEBUG_PIPE
    int dpipefd=open("/tmp/accpipe",O_RDONLY),dstate=0;
    FILE *dpipeh=fdopen(dpipefd,"r");
#endif
    unsigned char sequence[42],sequence_pos=0;
    struct pollfd pfd;

    max=obj->xs*obj->ys*2;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    
    for(;;)
    {
	pfd.fd=fh;
	pfd.events=POLLIN;
	pfd.revents=0;
	poll(&pfd,1,-1);
	if(read(fh,&c,1)!=1) { close(fh); break; }
#ifndef DEBUG_PIPE
#ifdef DEBUG_STDERR
	fprintf(stderr,"%c",c);
#endif
#endif
	switch(c)
	{
	    case '\n':
		kti->cy++;
		kti->cx=0;
		pos=kti->cy*obj->xs+kti->cx;
		if(kti->cy > kti->sre)
		{
		    pos=tgt_int_buffertask_lineup(obj,kti);
		}
		else
		    tgt_refresh(obj);
		break;
	    case '\r':
		kti->cx=0;
		pos=kti->cy*obj->xs+kti->cx;
		break;
	    case 7:
		putchar(7);
		fflush(stdout);
		break;
	    case 0x9b:
	    case 0x1b:
		sequence[0]=c;
		sequence_pos=1;
		break;
	    case 8:
		kti->cx--;
		kti->pos=pos-=1;
		tgt_refresh(obj);
		break;
	    case 127:
		l=(kti->xs-kti->cx) << 1;
		if(l) memmove(obj->visual_buffer+pos,obj->visual_buffer+pos+1,l);
		kti->obj->visual_buffer[kti->cy*kti->xs+kti->xs-1]=TGT_T_FCHAR(kti->attr,' ');
		tgt_refresh(obj);
		break;
	    case '[':
		if(sequence_pos==1) { sequence[sequence_pos++]=c; break; }
	    default:
		if(c<32) break;
		if(sequence_pos)
		{
		    if(isdigit(c) || c==';')
		    {
			if(sequence_pos<40) sequence[sequence_pos++]=c;
		    }
		    else
		    {
			sequence[sequence_pos++]=c;
			sequence[sequence_pos++]=0;
			sequence_pos=0;
#ifdef DEBUG_PIPE
			if(dstate==1)
			{
			    char buffer[40];
			    fprintf(stderr,"DoAcceptSequence: %s?\n",sequence+1);
			    fgets(buffer,40,dpipeh);
			    if(strstr(buffer,"cont")) dstate=0;
			}
			else
			{
			    char buffer[40];
			    for(;;) {
			    pfd.fd=dpipefd;
			    pfd.events=POLLIN;
			    pfd.revents=0;
			    if(poll(&pfd,1,1)==0) break;
			    fgets(buffer,40,dpipeh);
			    if(strstr(buffer,"stop")) dstate=1;
			    }
			}
#endif
			if(termemu_sequence_codes[c&0x7f].interpreter)
			    pos=termemu_sequence_codes[c&0x7f].interpreter(sequence,pos,kti);
			tgt_refresh(obj);
		    }
		    break;
		}
#ifdef DEBUG_PIPE
		if(dstate==1)
		{
		    char buffer[40];
		    fprintf(stderr,"DoAcceptKeyout: %c (%d)?\n",c,c);
		    fgets(buffer,40,dpipeh);
		    if(strstr(buffer,"cont")) dstate=0;
		}
#endif
		sequence_pos=0;
		kti->obj->visual_buffer[pos++]=TGT_T_FCHAR(kti->attr,c);
		kti->cx++;
		if(kti->cx >= obj->xs)
		{
		    kti->cy++;
		    kti->cx=0;
		}
/*		else
		{
		    l=(kti->xs-kti->cx) << 1;
		    if(l) memmove(kti->obj->visual_buffer+pos+2,kti->obj->visual_buffer+pos,l);
		}*/
		
		if(kti->cy > kti->sre)
		    pos=tgt_int_buffertask_lineup(obj,kti);
		else
		    tgt_refresh(obj);
		break;
	}
    }
    pthread_exit(NULL);
    return;
}



void tgt_detach_launcher(void * li)
{
    ((struct tgt_int_termemuinfo*) ((struct tgt_launcher_info*)li)->obj->class_data)->li=NULL;
    if(((struct tgt_launcher_info*)li)->thid) 
    {
	pthread_cancel(((struct tgt_launcher_info*)li)->thid);
	((struct tgt_launcher_info*)li)->thid=0;
    }
    ((struct tgt_launcher_info*)li)->obj=NULL;
}

void tgt_attach_launcher(void* li,struct tgt_object *obj)
{
    pthread_t thid;
    struct tgt_launcher_message msg;

    if(!already_initialized_sequence_codes) tgt_termemu_initsequencecodes();
    if(((struct tgt_launcher_info*) li)->obj) tgt_detach_launcher(li);
    ((struct tgt_int_termemuinfo*) obj->class_data)->li=li;
    ((struct tgt_launcher_info*) li)->obj=obj;
    pthread_create(&thid,NULL,(void*(*)(void*))tgt_int_buffertask,li);
    pthread_detach(thid);
    

    msg.type=TGT_LAUNCHERM_ATTACH;
    msg.data.size.x=obj->xs;
    msg.data.size.y=obj->ys;
    write(((struct tgt_launcher_info*) li)->pipe,(char*) &msg,256);

    ((struct tgt_launcher_info*)li) -> thid=thid;
}


void tgt_halt_launcher(void * lnch)
{
    if(((struct tgt_launcher_info *)lnch)->obj) tgt_detach_launcher(lnch);
    close(((struct tgt_launcher_info *)lnch)->master);
    close(((struct tgt_launcher_info *)lnch)->pipe);
    kill(((struct tgt_launcher_info *)lnch)->pid,SIGTERM);
    free(lnch);
}

static int tgt_termemu_seq_m(char* seq,int pos,struct tgt_int_termemuinfo *ti)
{
    int m,fg,bg;
    char *sbuff;
    sbuff=seq+2;
    for(;;)
    {
	m=atoi(sbuff);
	if(m>=30 && m<=37) ti->attr=TGT_T_FG(ti->attr,m-30);
	if(m>=40 && m<=47) ti->attr=TGT_T_BG(ti->attr,m-40);
	if(m==0) ti->attr=TGT_T_BUILDCELL(ti->obj->fg,ti->obj->bg,0,0,0);
	if(m==1) ti->attr=TGT_T_BOLD(ti->attr,1);
	if(m==7)
	{
	    fg=TGT_T_GETFG(ti->attr);
	    bg=TGT_T_GETBG(ti->attr);
	    ti->attr=TGT_T_FG(ti->attr,bg);
	    ti->attr=TGT_T_BG(ti->attr,fg);
	}
	if(!(sbuff=index(sbuff,';'))) break;
	sbuff++;
    }
    return(pos);
}

static int tgt_termemu_seq_H(char* seq,int pos,struct tgt_int_termemuinfo *ti)
{
    int x=0,y=atoi(seq+2)-1;
    char *b;
    if((b=index(seq,';'))) x=atoi(b+1)-1;
    if(x<0) x=0; if(y<0) y=0;
    if(x>=ti->xs) x=ti->xs-1;
    if(y>=ti->ys) y=ti->ys-1;
    ti->cx=x;
    ti->cy=y;
    return(ti->pos=y*ti->xs+x);
}


static int tgt_termemu_seq_cm(char* seq,int pos,struct tgt_int_termemuinfo *ti)
{
    int k=atoi(seq+2);
    char m=seq[strlen(seq)-1];
    if(k==0) k=1;
    switch(m)
    {
	case 'A': ti->cy-=k;  if(ti->cy<0) ti->cy=0;  break;
	case 'B': ti->cy+=k;  if(ti->cy>=ti->ys) ti->cy=ti->ys-1;  break;
	case 'D': ti->cx-=k;  if(ti->cx<0) ti->cx=0;  break;
	case 'C': ti->cx+=k;  if(ti->cx>=ti->xs) ti->cx=ti->xs-1;  break;
    }
    return(ti->pos=ti->cy*ti->xs+ti->cx);
}

static int tgt_termemu_seq_clr(char* seq,int pos,struct tgt_int_termemuinfo *ti)
{
    tgt_cell *buff=ti->obj->visual_buffer+pos;
    tgt_cell csq=TGT_T_FCHAR(ti->attr,' ');
    int i,left;
    for(i=0,left=ti->xs*ti->ys-pos;i<left;i++) *(buff++)=csq;
    return(pos);
}
static int tgt_termemu_seq_clreol(char* seq,int pos,struct tgt_int_termemuinfo *ti)
{
    tgt_cell *buff=buff=ti->obj->visual_buffer+pos;
    int x,left;
    tgt_cell csq=TGT_T_FCHAR(ti->attr,' ');
    for(x=0,left=ti->xs-ti->cx;x<left;x++) *(buff++)=csq;
    return(pos);
}
static int tgt_termemu_seq_clrch(char* seq,int pos,struct tgt_int_termemuinfo *ti)
{
    int n=atoi(seq+2),l=(ti->xs-ti->cx) << 1;
    if(n==0) n=1;
    while(n--)
    {
	if(l) memmove(ti->obj->visual_buffer+pos,ti->obj->visual_buffer+pos+1,l);
	ti->obj->visual_buffer[ti->cy*ti->xs+ti->xs-1]=TGT_T_FCHAR(ti->attr,' ');
    }
    return(pos);
}

static int tgt_termemu_seq_scrollregion(char* seq,int pos,struct tgt_int_termemuinfo *ti)
{
    int y=0,x=atoi(seq+2)-1;
    char *b;
    ti->srs=0; ti->sre=ti->ys-1;
    if((b=index(seq,';'))) 
    {
	y=atoi(b+1)-1;
	if(y>x)
	{
	    if(x<0) x=0;
	    if(y>=ti->ys) y=ti->ys-1;
	    ti->srs=x; ti->sre=y;
	    ti->cx=0; ti->cy=x;
	    return(ti->pos=y*ti->xs+x);
	}
    }
    return(pos);
}

inline static void tgt_int_termemu_scrolldn(tgt_cell * surface,int from,int to,int xs,tgt_cell fc)
{
    int x;
    tgt_cell * surfce;
    memmove(surface+(xs*(from+1)),surface+(xs*from),((to-from)*xs)<<1);
    surfce=surface + (xs*from);
    for(x=0;x<xs;x++) *(surfce++)=TGT_T_FCHAR(fc,' ');
}

static int tgt_termemu_seq_emm(char* seq,int pos,struct tgt_int_termemuinfo *ti)
{
    int x;
    if(seq[1]=='[')
    {
	x=atoi(seq+2); 
	if(x==0) x=1;
	while(x--)
	{
	    if(ti->cy<ti->sre)
		tgt_int_termemu_scroll(ti->obj->visual_buffer,ti->cy,ti->sre,ti->xs,ti->attr);
	}
    }
    else
    {
	x=atoi(seq+1);
	if(x==0) x=1;
	while(x--)
	    if(ti->cy<ti->sre)
		tgt_int_termemu_scrolldn(ti->obj->visual_buffer,ti->cy,ti->sre,ti->xs,ti->attr);
    }
    return(pos);
}
static int tgt_termemu_seq_ell(char* seq,int pos,struct tgt_int_termemuinfo *ti)
{
    int x;
    if(seq[1]=='[') x=atoi(seq+2); else x=atoi(seq+1);
    if(x==0) x=1;
    while(x--)
    {
	if(ti->cy<ti->sre)
	    tgt_int_termemu_scrolldn(ti->obj->visual_buffer,ti->cy,ti->sre,ti->xs,ti->attr);
    }
    return(pos);
}
static int tgt_termemu_seq_ic(char* seq,int pos,struct tgt_int_termemuinfo *ti)
{
    int x,l=(ti->xs-ti->cx) << 1;
    if(seq[1]=='[') x=atoi(seq+2); else x=atoi(seq+1);
    if(l) while(x--)
	memmove(ti->obj->visual_buffer+pos+1,ti->obj->visual_buffer+pos,l);
    return(pos);
}

// inline static void tgt_int_termemu_scroll(unsigned char * surface,int from,int to,int xs,int fc)

void tgt_termemu_initsequencecodes(void)
{
    int i;
    for(i=0;i<128;i++) termemu_sequence_codes[i].interpreter=NULL;
    termemu_sequence_codes['m'].interpreter = tgt_termemu_seq_m;
    termemu_sequence_codes['H'].interpreter = tgt_termemu_seq_H;
    termemu_sequence_codes['A'].interpreter = tgt_termemu_seq_cm;
    termemu_sequence_codes['B'].interpreter = tgt_termemu_seq_cm;
    termemu_sequence_codes['C'].interpreter = tgt_termemu_seq_cm;
    termemu_sequence_codes['D'].interpreter = tgt_termemu_seq_cm;
    termemu_sequence_codes['J'].interpreter = tgt_termemu_seq_clr;
    termemu_sequence_codes['K'].interpreter = tgt_termemu_seq_clreol;
    termemu_sequence_codes['P'].interpreter = tgt_termemu_seq_clrch;
    termemu_sequence_codes['M'].interpreter = tgt_termemu_seq_emm;
    termemu_sequence_codes['L'].interpreter = tgt_termemu_seq_ell;
    termemu_sequence_codes['r'].interpreter = tgt_termemu_seq_scrollregion;
    termemu_sequence_codes['@'].interpreter = tgt_termemu_seq_ic;
    already_initialized_sequence_codes=1;
}
#endif