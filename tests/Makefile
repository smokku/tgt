include ../Make.conf

OBJECTS = csp list string menu status simplest buffer fb direct dialog config autocreat termemu window TGT


all: tests

tests: $(OBJECTS)
csp: $(srcdir)/tests/csp.c
	gcc $(CFLAGS) $(srcdir)/tests/csp.c -o csp -L../ -ltgt
termemu: $(srcdir)/tests/termemu.c
	gcc $(CFLAGS) $(srcdir)/tests/termemu.c -o termemu -L../ -ltgt
config: $(srcdir)/tests/config.c
	gcc $(CFLAGS) $(srcdir)/tests/config.c -o config -L../ -ltgt
list: $(srcdir)/tests/list.c
	gcc $(CFLAGS) $(srcdir)/tests/list.c -o list -L../ -ltgt
string: $(srcdir)/tests/string.c
	gcc $(CFLAGS) $(srcdir)/tests/string.c -o string -L../ -ltgt
menu: $(srcdir)/tests/menu.c
	gcc $(CFLAGS) $(srcdir)/tests/menu.c -o menu -L../ -ltgt
status: $(srcdir)/tests/status.c
	gcc $(CFLAGS) $(srcdir)/tests/status.c -o status -L../ -ltgt
test: $(srcdir)/tests/test.c
	gcc $(CFLAGS) $(srcdir)/tests/test.c -o test -L../ -ltgt
simplest: $(srcdir)/tests/simplest.c
	gcc $(CFLAGS) $(srcdir)/tests/simplest.c -o simplest -L../ -ltgt
buffer: $(srcdir)/tests/buffer.c
	gcc $(CFLAGS) $(srcdir)/tests/buffer.c -o buffer -L../ -ltgt
fb: $(srcdir)/tests/fb.c
	gcc $(CFLAGS) $(srcdir)/tests/fb.c -o fb -L../ -ltgt
direct: $(srcdir)/tests/direct.c
	gcc $(CFLAGS) $(srcdir)/tests/direct.c -o direct -L../ -ltgt
dialog: $(srcdir)/tests/dialog.c
	gcc $(CFLAGS) $(srcdir)/tests/dialog.c -o dialog -L../ -ltgt
autocreat: $(srcdir)/tests/autocreat.c
	gcc $(CFLAGS) $(srcdir)/tests/autocreat.c -o autocreat -L../ -ltgt
window: $(srcdir)/tests/window.c
	gcc $(CFLAGS) $(srcdir)/tests/window.c -o window -L../ -ltgt
TGT: $(srcdir)/tests/TGT.c
	gcc $(CFLAGS) $(srcdir)/tests/TGT.c -o TGT -L../ -ltgt

# make a clean source tree again
clean: 
	rm -rf libtgt.so *.o *~ core >/dev/null 2>/dev/null
	rm -rf $(OBJECTS) >/dev/null 2>/dev/null

distclean: clean
