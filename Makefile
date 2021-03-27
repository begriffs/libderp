.POSIX :

VARIANT = release
CFLAGS = -Iinclude

MAKEFILES = Makefile build/$(VARIANT)/extra.mk

.SUFFIXES :

include build/$(VARIANT)/extra.mk

lib : build/$(VARIANT)/vector.o

tests : build/$(VARIANT)/test/t_vector

build/$(VARIANT)/vector.o : src/vector.c include/vector.h $(MAKEFILES)
	$(CC) $(CFLAGS) -o $@ -c src/vector.c

build/$(VARIANT)/test/t_vector : build/$(VARIANT)/vector.o test/t_vector.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/vector.o test/t_vector.c $(LDLIBS)
