#include "tgt.h"

int main(int argc,char **argv)
{
    void *ph;
    if(argc<=1)
	ph=tgt_load_conf("config.conf");
    else
	ph=tgt_load_conf(argv[1]);

    printf("Section 'main', key 'test' = '%s'\n",
	    tgt_get_conf(ph,"main","test","<default>"));
    printf("Section 'test-section', key 'alpha' = '%s'\n",
	    tgt_get_conf(ph,"test-section","alpha","<default>"));
    printf("Section 'blah', key 'foobar' = '%s'\n",
	    tgt_get_conf(ph,"blah","foobar","<default>"));

    tgt_set_conf(ph,"blah","foobar","This ought to be set in \" foobar \" field \n");
    tgt_save_conf(ph,"config.conf.sav");
    tgt_unload_conf(ph);
}
