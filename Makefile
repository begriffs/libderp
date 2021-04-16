.POSIX :

VARIANT = release
CFLAGS = -Iinclude

MAKEFILES = Makefile build/$(VARIANT)/extra.mk

.SUFFIXES :

include build/$(VARIANT)/extra.mk

lib : build/$(VARIANT)/vector.o build/$(VARIANT)/list.o

tests : build/$(VARIANT)/test/t_vector build/$(VARIANT)/test/t_list

build/$(VARIANT)/vector.o : src/vector.c include/vector.h $(MAKEFILES)
	$(CC) $(CFLAGS) -o $@ -c src/vector.c

build/$(VARIANT)/list.o : src/list.c include/list.h $(MAKEFILES)
	$(CC) $(CFLAGS) -o $@ -c src/list.c

build/$(VARIANT)/test/t_vector : build/$(VARIANT)/vector.o test/t_vector.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/vector.o test/t_vector.c $(LDLIBS)

build/$(VARIANT)/test/t_list : build/$(VARIANT)/list.o test/t_list.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/list.o test/t_list.c $(LDLIBS)
