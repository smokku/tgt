
/* TGT_CLASS_ACBASE is a huge value (about 100 000 ;), we don't want AC
   tags and normal tags numerations to mix */

#define TGT_AC_HORIZONTALGROUP TGT_CLASS_ACBASE+1
#define TGT_AC_VERTICALGROUP TGT_CLASS_ACBASE+2
#define TGT_AC_SPACINGOBJECT TGT_CLASS_ACBASE+3

#define TGT_AC_CHILD	  TGT_CLASS_ACBASE+100
/* 
    Usage:
         TGT_AC_CHILD,TYPE,
	     child declaration ...
	     TGT_AC_END,0,
*/

#define TGT_AC_ALIGNMENT  TGT_CLASS_ACBASE+101

/* 
    Usage:
        TGT_AC_ALIGNMENT,alignement_code(see below),
	TGT_AC_CHILD,...
	
    Alignement of children will be set to alignement_code ... Note
    that horizontal alignement only makes sense in vertical groups
    and vice-versa.
*/
#define TGT_AC_SPACE      TGT_CLASS_ACBASE+102

/*
    TGT_AC_SPACE,num,
    Insert num spaces between two children, note that num has to be ge 1
*/
    

#define TGT_AC_SAVEAS     TGT_CLASS_ACBASE+103

/* 
    TGT_AC_SAVEAS,&ptr,
    TGT_AC_CHILD,...
    Save a pointer to next child object in location pointed by &ptr.

*/

#define TGT_AC_XS        TGT_CLASS_ACBASE+104
#define TGT_AC_YS        TGT_CLASS_ACBASE+105

/* Force minimal group size */

#define TGT_AC_SPACING   TGT_CLASS_ACBASE+106

#define TGT_AC_FIX_X   TGT_CLASS_ACBASE+107
#define TGT_AC_FIX_Y   TGT_CLASS_ACBASE+108


#define TGT_AC_END	  TGTT_END

#define TGT_AC_SF_XFIXED 1
#define TGT_AC_SF_YFIXED 2

/* Size flags, internal usage only (classes' functions) */

#define TGT_AC_LEFT    0x00
#define TGT_AC_RIGHT   0x01
#define TGT_AC_HCENTER 0x02

#define TGT_AC_TOP     0x00
#define TGT_AC_BOTTOM  0x04
#define TGT_AC_VCENTER 0x08

#define TGT_AC_CUSTOM 0x10



/* This structure is internally used by autocreator functions and
   by classes' functions */

struct tgt_ac_objectinfo
{
    int type;           /* class or special type (TGT_AC_..) of object */
    int nchildren;      /* number of children */
    void * ctl;         /* children list (struct tgt_ac_objectinfo **) 
                           (groups) or taglist (normal objects)         */
    int xsize;          
    int ysize;          /* Minimal or exact size of object */
    int xpos;
    int ypos;           /* Position of object, set by tgt_ac_positionself() */
    struct tgt_object **pointer;   /* Pointer to a locaton to save object pointer at (see TGT_AC_SAVEAS) */
    unsigned char sizeflags;  /* Size flags .. if TGT_AC_SF_XFIXED is set,
             then size specified at xsize is exact, otherwise it's minimal possible
	     object size (however the object could be enlarged) */
    unsigned char alignment; 
    struct tgt_terminal * term;
};
