.POSIX :

VARIANT = release
CFLAGS = -Iinclude -fPIC

MAKEFILES = Makefile build/$(VARIANT)/extra.mk

OBJS = build/$(VARIANT)/vector.o build/$(VARIANT)/list.o build/$(VARIANT)/hashmap.o build/$(VARIANT)/treemap.o

.SUFFIXES :

include build/$(VARIANT)/extra.mk

lib : build/$(VARIANT)/libderp.so build/$(VARIANT)/libderp.a

build/$(VARIANT)/libderp.a : $(OBJS)
	ar r $@ $?

build/$(VARIANT)/libderp.so : $(OBJS)
	$(CC) $(CFLAGS) -shared $(OBJS) -o $@

tests : build/$(VARIANT)/test/t_vector build/$(VARIANT)/test/t_list build/$(VARIANT)/test/t_hashmap build/$(VARIANT)/test/t_treemap

build/$(VARIANT)/vector.o : src/vector.c include/derp/vector.h include/derp/common.h $(MAKEFILES)
	$(CC) $(CFLAGS) -o $@ -c src/vector.c

build/$(VARIANT)/list.o : src/list.c include/derp/list.h include/derp/common.h $(MAKEFILES)
	$(CC) $(CFLAGS) -o $@ -c src/list.c

build/$(VARIANT)/hashmap.o : src/hashmap.c include/derp/hashmap.h include/derp/list.h include/derp/common.h $(MAKEFILES)
	$(CC) $(CFLAGS) -o $@ -c src/hashmap.c

build/$(VARIANT)/treemap.o : src/treemap.c include/derp/treemap.h include/derp/list.h include/derp/common.h $(MAKEFILES)
	$(CC) $(CFLAGS) -o $@ -c src/treemap.c

build/$(VARIANT)/test/t_vector : build/$(VARIANT)/vector.o test/t_vector.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/vector.o test/t_vector.c $(LDLIBS)

build/$(VARIANT)/test/t_list : build/$(VARIANT)/list.o test/t_list.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/list.o test/t_list.c $(LDLIBS)

build/$(VARIANT)/test/t_hashmap : build/$(VARIANT)/hashmap.o build/$(VARIANT)/list.o test/t_hashmap.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/hashmap.o build/$(VARIANT)/list.o test/t_hashmap.c $(LDLIBS)

build/$(VARIANT)/test/t_treemap : build/$(VARIANT)/treemap.o build/$(VARIANT)/list.o test/t_treemap.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/treemap.o build/$(VARIANT)/list.o test/t_treemap.c $(LDLIBS)
