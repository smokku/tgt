#
# Makefile
#
# Makefile for libtgt

include Make.conf

CC	= gcc -O2 -Wall

OBJECTS = ${SOURCES:.c=.o}

all: shared subdirs
	echo Build done!

.c.o:
	$(CC) $(CFLAGS) $(DEFINES) -c $(srcdir)/$< -o $@

shared: $(OBJECTS)
	$(CC) -shared $(OBJECTS) -o libtgt.so $(LIBS)

subdirs:
	for i in $(SUBDIRS); \
	do $(MAKE) -C $$i srcdir="$(srcdir)"; done

# make a clean source tree again

clean_subdirs:
	for i in $(SUBDIRS); \
	do $(MAKE) -C $$i clean; done


clean:  clean_subdirs
	rm -f *.o *.a *~ $$~* a.out config.log >/dev/null 2>&1
	rm -f ./libtgt.so *.class
	rm -f $(OBJECTS) >/dev/null 2>&1

install: 
	mkdir -p $(DESTDIR)$(libdir)
	mkdir -p $(DESTDIR)$(includedir)
	rm -rf $(DESTDIR)$(libdir)/libtgt.so $(DESTDIR)$(includedir)/tgt
	cp ./libtgt.so $(DESTDIR)$(libdir)
	cp -rfd $(srcdir)/include $(DESTDIR)$(includedir)/tgt

uninstall:
	rm -rf $(libdir)/libtgt.so*
	rm -rf $(includedir)/tgt

distclean: clean clean_subdirs
	echo -e "SUBDIRS = $(SUBDIRS)\nerror:\n\t@echo Please run ./configure first..." >Make.conf
	rm -f libtgt.so fbpic.class $(srcdir)/classes_so/fb_display_config.h >/dev/null 2>&1
