

typedef long tgt_numtype;
typedef void* tgt_ptrtype;

typedef tgt_numtype tagitem;
typedef tgt_numtype tgtt;



#define tgt_getnumtag(a,b,c) ((tgt_numtype) tgt_gettag(a,b,(tgtt) (c)))
#define tgt_getptrtag(a,b,c) ((tgt_ptrtype) tgt_gettag(a,b,(tgtt) (c)))
#define SEMAPHORED_REFRESH_DELAY 10000

#define TGT_QUEUES

#define TGT_ERRORHANDLING_TGT 1
#define TGT_ERRORHANDLING_APP 0
