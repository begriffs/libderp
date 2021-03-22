.POSIX :

VARIANT = release
CFLAGS = -Iinclude

.SUFFIXES :

include build/$(VARIANT)/extra.mk

build/$(VARIANT)/vector.o : src/vector.c include/vector.h
	$(CC) $(CFLAGS) -o $@ -c src/vector.c

clean :
	rm -f build/$(VARIANT)/*.o
