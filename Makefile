
SOURCES= lowlevel/lowlevel.c classes/keyboard.c classes/class.c classes/management.c\
	 app/async.c app/queue.c lowlevel/keytree.c classes/tags.c\
	 classes/c_desktop.c classes/c_window.c classes/c_button.c\
	 classes/c_plaintext.c
OBJECTS = ${SOURCES:.c=.o}

CFLAGS =  -Iinclude/ -DTGT_POSSIBLE_INFORMFIRST -DTGT_POSSIBLE_ASYNC -DTGT_QUEUES
LDFLAGS = 
LIBS = 

#default target: compile
all: shared test

# generate source file dependencies

# compile and link

shared: ${OBJECTS}
	gcc -shared $(OBJECTS) -o libtgui.so -ltermcap -lpthread

test: ./libtgui.so
	gcc test.c -o test -L./ -Iinclude/ -ltgui
	
# make a clean source tree again
clean: 
	rm -r libtgui.so *.o *~ core test $(OBJECTS)


distclean: clean
