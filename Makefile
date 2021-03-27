.POSIX :

VARIANT = release
CFLAGS = -Iinclude

.SUFFIXES :

include build/$(VARIANT)/extra.mk

lib : build/$(VARIANT)/vector.o

test : build/$(VARIANT)/test/t_vector

build/$(VARIANT)/vector.o : src/vector.c include/vector.h
	$(CC) $(CFLAGS) -o $@ -c src/vector.c

build/$(VARIANT)/test/t_vector : build/$(VARIANT)/vector.o test/t_vector.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/vector.o test/t_vector.c $(LDLIBS)

clean :
	rm -f build/$(VARIANT)/*.o
