
OBJECTS = csp list string menu status simplest buffer fb direct dialog config autocreat termemu window


CFLAGS =  -Werror -Wall
LDFLAGS = 
LIBS = -L../ -ltgt

#default target: compile
all: tests

tests: csp list string status simplest fb direct dialog config autocreat buffer window termemu menu
csp: csp.c
	gcc $(CFLAGS) csp.c -o csp -L../ -ltgt -I../include/
termemu: termemu.c
	gcc $(CFLAGS) termemu.c -o termemu -L../ -ltgt -I../include/
config: config.c
	gcc $(CFLAGS) config.c -o config -L../ -ltgt -I../include/
list: list.c
	gcc $(CFLAGS) list.c -o list -L../ -ltgt -I../include/
string: string.c
	gcc $(CFLAGS) string.c -o string -L../ -ltgt -I../include/
menu: menu.c
	gcc $(CFLAGS) menu.c -o menu -L../ -ltgt -I../include/
status: status.c
	gcc $(CFLAGS) status.c -o status -L../ -ltgt -I../include/
test: test.c
	gcc $(CFLAGS) test.c -o test -L../ -ltgt -I../include/
simplest: simplest.c
	gcc $(CFLAGS) simplest.c -o simplest -L../ -ltgt -I../include/ -Wall
buffer: buffer.c
	gcc $(CFLAGS) buffer.c -o buffer -L../ -ltgt -I../include/
fb: fb.c
	gcc $(CFLAGS) fb.c -o fb -L../ -ltgt -I../include/
direct: direct.c
	gcc $(CFLAGS) direct.c -o direct -L../ -ltgt -I../include/
dialog: dialog.c
	gcc $(CFLAGS) dialog.c -o dialog -L../ -ltgt -I../include/
autocreat: autocreat.c
	gcc $(CFLAGS) autocreat.c -o autocreat -L../ -ltgt -I../include/
window: window.c
	gcc $(CFLAGS) window.c -o window -L../ -ltgt -I../include/

# make a clean source tree again
clean: 
	rm -rf libtgt.so *.o *~ core >/dev/null 2>/dev/null
	rm -rf $(OBJECTS) >/dev/null 2>/dev/null

install: cp ./libtgt.so /usr/lib
uninstall: rm -rf /usr/lib/libtgt.so*

distclean: clean
