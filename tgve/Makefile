
SOURCES= tgve.c operations.c util.c dh.c data.c editor.c\
	 edit/button.c
OBJECTS = ${SOURCES:.c=.o}

OUT= tgve

CFLAGS = -O2 -I../include -I.
LDFLAGS = 
LIBS = -L../ -ltgt

all: ${OBJECTS}
	gcc $(OBJECTS) -o $(OUT) $(LDFLAGS) $(LIBS)
	strip $(OUT)
clean:
	rm -rf $(OBJECTS) $(OUT)
	