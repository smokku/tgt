
SOURCES= lowlevel/lowlevel.c classes/keyboard.c classes/class.c classes/management.c\
	 app/async.c app/queue.c lowlevel/keytree.c classes/tags.c\
	 classes/c_desktop.c classes/c_window.c classes/c_button.c\
	 classes/c_plaintext.c classes/c_string.c classes/c_list.c\
	 lowlevel/menu.c classes/c_menu.c classes/c_cycle.c\
	 classes/c_checkbox.c classes/c_slider.c classes/c_progress.c
OBJECTS = ${SOURCES:.c=.o}

CFLAGS =  -Iinclude/ -DTGT_POSSIBLE_INFORMFIRST -DTGT_POSSIBLE_ASYNC -DTGT_QUEUES -DTIMEOUT_KEYBOARD
LDFLAGS = 
LIBS = 

#default target: compile
all: shared test

# generate source file dependencies

# compile and link

shared: ${OBJECTS}
	gcc -shared $(OBJECTS) -o libtgt.so -ltermcap -lpthread

test: shared
	gcc test.c -o test -L./ -Iinclude/ -ltgt

examples/string: shared
	gcc examples/string.c -o examples/string -L./ -Iinclude/ -ltgt
examples/list: shared
	gcc examples/list.c -o examples/list -L./ -Iinclude/ -ltgt
examples/menu: shared
	gcc examples/menu.c -o examples/menu -L./ -Iinclude/ -ltgt
examples/csp: shared
	gcc examples/csp.c -o examples/csp -L./ -Iinclude/ -ltgt
	
tests: examples/string examples/list examples/menu examples/csp

# make a clean source tree again
clean: 
	rm -r libtgt.so *.o *~ core test examples/list examples/string examples/context $(OBJECTS) >/dev/null

distclean: clean
