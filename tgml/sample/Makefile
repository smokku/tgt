include ../../Make.conf

TESTS= dbadd

LIBS = -ltgt -I ../../include

OUT=$(NUM)

all: 	bimberman
	for i in $(TESTS); \
	do $(MAKE) test NUM=$$i ; done

bimberman: bimbergui.c
	cc -o bimberman bimberman.c bimbergui.c $(LIBS)
	strip bimberman
	
bimbergui.c: bimberman.tgml
	../tgmlp bimberman.tgml > bimbergui.c
	
	
bimbclean:
	rm -fr bimbergui.c bimberman

clean:	bimbclean
	for i in $(TESTS); \
	do $(MAKE) testclean NUM=$$i; done
	rm sample.o
	
test: $(NUM)

$(NUM): $(NUM).o sample.o
	gcc $(LDFLAGS) -o $(OUT) sample.o $(NUM).o $(LIBS)
	strip $(OUT)

$(NUM).o: $(NUM).c

$(NUM).c: $(NUM).tgml
	../tgmlp $(NUM).tgml >$(NUM).c

testclean:
	rm -rf $(OUT) $(NUM).c $(NUM).o

distclean: clean
