
OBJECTS = csp list string menu

SUBDIRS=examples

CFLAGS =  -I../include
LDFLAGS = 
LIBS = -L../ -ltgt

#default target: compile
all: tests

tests: csp list string menu
csp: 
	gcc csp.c -o csp -L../ -ltgt -I../include/
list:
	gcc list.c -o list -L../ -ltgt -I../include/
string:
	gcc string.c -o string -L../ -ltgt -I../include/
menu:
	gcc menu.c -o menu -L../ -ltgt -I../include/

# make a clean source tree again
clean: 
	rm -rf libtgt.so *.o *~ core test >/dev/null 2>/dev/null
	rm -rf $(OBJECTS) >/dev/null 2>/dev/null

install: cp ./libtgt.so /usr/lib
uninstall: rm -rf /usr/lib/libtgt.so*

distclean: clean
