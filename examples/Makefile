
SUBDIRS=1 2 3 4 5 6

all: 
	for i in $(SUBDIRS); \
	do $(MAKE) -C $$i; done

clean: 
	for i in $(SUBDIRS); \
	do $(MAKE) -C $$i clean; done
