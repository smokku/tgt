

CFLAGS= -O2 -Wall -I../../include
LDFLAGS= -L../../ -ltgt

all: 2.o gui.o
	$(CC) -o 2 2.o gui.o $(LDFLAGS)

gui.c: 2.tgml
	../../tgml/tgmlp 2.tgml gui.c 

clean:
	rm -f 2 gui.c gui.o 2.o
