
SOURCES= lowlevel/lowlevel.c classes/keyboard.c classes/class.c classes/management.c\
	 app/async.c app/queue.c lowlevel/keytree.c classes/tags.c\
	 classes/c_desktop.c classes/c_window.c classes/c_button.c\
	 classes/c_label.c classes/c_string.c classes/c_list.c\
	 lowlevel/menu.c classes/c_menu.c classes/c_cycle.c\
	 classes/c_checkbox.c classes/c_slider.c classes/c_progress.c\
	 classes/c_status.c classes/c_textbuffer.c classes/c_direct.c app/config.c\
	 app/util.c
OBJECTS = ${SOURCES:.c=.o}

SUBDIRS=examples

CFLAGS =  -Iinclude/ -DTGT_POSSIBLE_INFORMFIRST -DTGT_POSSIBLE_ASYNC\
	     -DTGT_QUEUES -DTIMEOUT_KEYBOARD -DTGT_DLTERMCAP\
	     -DTGT_DLCLASSES -DSEMAPHORED_REFRESH
LDFLAGS = 
LIBS = 

#default target: compile
all: shared fbclass
	for i in $(SUBDIRS); \
	do $(MAKE) -C $$i; done

# Termcap compiled-in
#shared: ${OBJECTS}
#	gcc -shared $(OBJECTS) -o libtgt.so -ltermcap -lpthread

# Termcap loaded by libdl
shared: ${OBJECTS}
	gcc -shared $(OBJECTS) -o libtgt.so -ldl -lpthread

fbclass: classes_so/c_fbpic.c
	gcc -shared classes_so/c_fbpic.c classes_so/fb_display.c -o fbpic.class $(CFLAGS)
	
# make a clean source tree again
clean: 
	for i in $(SUBDIRS); \
	do $(MAKE) -C $$i clean; done
		
	rm -rf libtgt.so *.o *~ core >/dev/null 2>/dev/null
	rm -rf $(OBJECTS) >/dev/null 2>/dev/null

install: 
	cp ./libtgt.so /usr/lib
	cp -rf include /usr/include/tgt

uninstall:
	rm -rf /usr/lib/libtgt.so*
	rm -rf /usr/include/tgt

distclean: clean
