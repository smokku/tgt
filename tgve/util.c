#include "tgt.h"
#include "tgve.h"
#include <stdio.h>
#include <unistd.h>

inline static tgt_cell insc(char gs, int c1, int c2, tgt_cell cc)
{
    if(TGT_T_GETBG(cc) == c1) c1 = c2;
    return(TGT_T_BOLD(TGT_T_FG(TGT_T_CHAR(cc,gs),c1),1));
}

void framedraw(tgt_cell *buff, int xsize, int sx, int sy, int dx, int dy)
{
    int x, y;
    struct tgt_terminal * term = tgt_getterminal();
    
    buff += sy * xsize + sx;
    if(dx>sx && dy>sy)
    {
    
	buff[0] = insc(TGT_TC_UL, COLOR_DRAGFRAME1, COLOR_DRAGFRAME2, buff[0]);
	for(x = 0, dx-=sx + 2; x < dx; x++) 
	    buff[x+1] = insc(TGT_TC_HL, COLOR_DRAGFRAME1, COLOR_DRAGFRAME2, buff[x+1]);
	buff[dx+1] = insc(TGT_TC_UR, COLOR_DRAGFRAME1, COLOR_DRAGFRAME2, buff[dx+1]);

	buff += xsize;

	for(y = 0, dy-=sy + 2; y < dy; y++)
	{
	    buff[0] = insc(TGT_TC_VL, COLOR_DRAGFRAME1, COLOR_DRAGFRAME2, buff[0]);
	    buff[dx+1] = insc(TGT_TC_VL, COLOR_DRAGFRAME1, COLOR_DRAGFRAME2, buff[dx+1]);
	    buff+=xsize;
	}

	buff[0] = insc(TGT_TC_LL, COLOR_DRAGFRAME1, COLOR_DRAGFRAME2, buff[0]);
	for(x = 0; x < dx; x++) 
	    buff[x+1] = insc(TGT_TC_HL, COLOR_DRAGFRAME1, COLOR_DRAGFRAME2, buff[x+1]);
	buff[dx+1] = insc(TGT_TC_LR, COLOR_DRAGFRAME1, COLOR_DRAGFRAME2, buff[dx+1]);
    }
    else if(dx>sx)
	for(x = 0, dx -= sx; x<dx; x++) buff[x] = insc('*',COLOR_DRAGFRAME1, COLOR_DRAGFRAME2, buff[x]);
    else if(dy>sy)
	for(y = 0, dy -= sy; y<dy; y++, buff+=xsize) *buff = insc('*',COLOR_DRAGFRAME1, COLOR_DRAGFRAME2, *buff);
    
}

struct tgve_objinfo * create_objdatainfo(int class, int vid)
{
    struct tgve_objinfo * ret = (struct tgve_objinfo*) malloc(sizeof(struct tgve_objinfo));
    
    ret->type = WINTYPE_OBJECT;
    ret->objdata.class = class;
    strcpy(ret->objdata.virtual_id,"");
    strcpy(ret->objdata.virtual_callback,"");
    ret->objdata.locked = 0;
    return(ret);
}

struct tgve_objinfo * create_editordatainfo(struct tgve_editorinfo * ei, struct tgt_object *ref)
{
    struct tgve_objinfo * ret = (struct tgve_objinfo*) malloc(sizeof(struct tgve_objinfo));
    
    ret->type = WINTYPE_EDITOR;
    ret->editordata.ei = ei;
    ret->editordata.reference = ref;
    return(ret);
}


int lock_object(struct tgt_object * obj)
{
    if(! obj->user_data) return(0);
    if(((struct tgve_objinfo*) (obj->user_data))->objdata.locked) return(0);
    return( (((struct tgve_objinfo*) (obj->user_data))->objdata.locked = 1) );
}

void unlock_object(struct tgt_object * obj)
{
    ((struct tgve_objinfo*) (obj->user_data))->objdata.locked = 0;
}
