
typedef long codetype;
typedef long valuetype;
typedef void* pointertype;

typedef union { long vdata; void * ptrdata; } tagdata;

struct ti { codetype tcode; tagdata tdata; };
typedef struct ti tagitem;

#define tgt_getnumtag(a,b,c) tgt_gettag(a,b,(tagdata)(valuetype) (c)).vdata
#define tgt_getptrtag(a,b,c) tgt_gettag(a,b,(tagdata)(pointertype) (c)).ptrdata
#define SEMAPHORED_REFRESH_DELAY 10000
