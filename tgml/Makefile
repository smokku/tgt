include ../Make.conf

CC	= gcc -O2 -Wall

SOURCES	= tgmlp.c parse.c tags.c
OBJECTS	= ${SOURCES:.c=.o}

OUT	= tgmlp

LIBS	= tags/tags.a

all: ${OBJECTS}
	gcc $(LDFLAGS) -o $(OUT) $(OBJECTS) $(LIBS)
	strip $(OUT)

.c.o:
	$(CC) $(CFLAGS) -c $(srcdir)/tgml/$< -o $@

clean:
	rm -rf $(OBJECTS) $(OUT)

distclean: clean
