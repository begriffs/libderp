.POSIX :

VARIANT = release
CFLAGS = -Iinclude -fPIC

MAKEFILES = Makefile build/$(VARIANT)/extra.mk

OBJS = build/$(VARIANT)/vector.o build/$(VARIANT)/list.o build/$(VARIANT)/hashmap.o

.SUFFIXES :

include build/$(VARIANT)/extra.mk

lib : build/$(VARIANT)/libderp.so build/$(VARIANT)/libderp.a

build/$(VARIANT)/libderp.a : $(OBJS)
	ar r $@ $?

build/$(VARIANT)/libderp.so : $(OBJS)
	$(CC) $(CFLAGS) -shared $(OBJS) -o $@

tests : build/$(VARIANT)/test/t_vector build/$(VARIANT)/test/t_list build/$(VARIANT)/test/t_hashmap

build/$(VARIANT)/vector.o : src/vector.c include/vector.h include/common.h $(MAKEFILES)
	rm -f build/$(VARIANT)/vector.gcda
	$(CC) $(CFLAGS) -o $@ -c src/vector.c

build/$(VARIANT)/list.o : src/list.c include/list.h include/common.h $(MAKEFILES)
	rm -f build/$(VARIANT)/list.gcda
	$(CC) $(CFLAGS) -o $@ -c src/list.c

build/$(VARIANT)/hashmap.o : src/hashmap.c include/hashmap.h include/list.h include/common.h $(MAKEFILES)
	rm -f build/$(VARIANT)/hashmap.gcda
	$(CC) $(CFLAGS) -o $@ -c src/hashmap.c

build/$(VARIANT)/test/t_vector : build/$(VARIANT)/vector.o test/t_vector.c
	rm -f t_vector.gcda
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/vector.o test/t_vector.c $(LDLIBS)

build/$(VARIANT)/test/t_list : build/$(VARIANT)/list.o test/t_list.c
	rm -f t_list.gcda
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/list.o test/t_list.c $(LDLIBS)

build/$(VARIANT)/test/t_hashmap : build/$(VARIANT)/hashmap.o build/$(VARIANT)/list.o test/t_hashmap.c
	rm -f t_hashmap.gcda
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/hashmap.o build/$(VARIANT)/list.o test/t_hashmap.c $(LDLIBS)
