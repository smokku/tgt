
OBJECTS = csp list string menu status test simplest buffer fb direct dialog

SUBDIRS=examples

CFLAGS =  -I../include
LDFLAGS = 
LIBS = -L../ -ltgt

#default target: compile
all: tests

tests: csp list string menu status test simplest buffer fb direct dialog
csp: csp.c
	gcc csp.c -o csp -L../ -ltgt -I../include/
list: list.c
	gcc list.c -o list -L../ -ltgt -I../include/
string: string.c
	gcc string.c -o string -L../ -ltgt -I../include/
menu: menu.c
	gcc menu.c -o menu -L../ -ltgt -I../include/
status: status.c
	gcc status.c -o status -L../ -ltgt -I../include/
test: test.c
	gcc test.c -o test -L../ -ltgt -I../include/
simplest: simplest.c
	gcc simplest.c -o simplest -L../ -ltgt -I../include/ -Wall
buffer: buffer.c
	gcc buffer.c -o buffer -L../ -ltgt -I../include/
fb: fb.c
	gcc fb.c -o fb -L../ -ltgt -I../include/
direct: direct.c
	gcc direct.c -o direct -L../ -ltgt -I../include/
dialog: dialog.c
	gcc dialog.c -o dialog -L../ -ltgt -I../include/

# make a clean source tree again
clean: 
	rm -rf libtgt.so *.o *~ core >/dev/null 2>/dev/null
	rm -rf $(OBJECTS) >/dev/null 2>/dev/null

install: cp ./libtgt.so /usr/lib
uninstall: rm -rf /usr/lib/libtgt.so*

distclean: clean
