.POSIX :

VARIANT = release
CFLAGS = -Iinclude -g $(EXTRA_CFLAGS)

MAKEFILES = Makefile build/$(VARIANT)/extra.mk config.mk

OBJS = build/$(VARIANT)/common.o \
	   build/$(VARIANT)/vector.o \
	   build/$(VARIANT)/list.o \
	   build/$(VARIANT)/hashmap.o \
	   build/$(VARIANT)/treemap.o

OBJS_PIC = build/$(VARIANT)/pic/common.o \
		   build/$(VARIANT)/pic/vector.o \
		   build/$(VARIANT)/pic/list.o \
		   build/$(VARIANT)/pic/hashmap.o \
		   build/$(VARIANT)/pic/treemap.o

.SUFFIXES :

include config.mk
include build/$(VARIANT)/extra.mk

lib : build/$(VARIANT)/libderp.${SO} build/$(VARIANT)/libderp.a

build/$(VARIANT)/libderp.a : $(OBJS)
	$(AR) r $@ $?

build/$(VARIANT)/libderp.${SO} : $(OBJS_PIC) VERSION
	$(CC) $(CFLAGS) -fPIC ${SOFLAGS} $(OBJS_PIC) -o $@

tests : build/$(VARIANT)/test/t_vector build/$(VARIANT)/test/t_list build/$(VARIANT)/test/t_hashmap build/$(VARIANT)/test/t_treemap

build/$(VARIANT)/common.o : src/common.c include/derp/common.h
	$(CC) $(CFLAGS) -o $@ -c src/common.c
build/$(VARIANT)/pic/common.o : src/common.c include/derp/common.h
	$(CC) $(CFLAGS) -fPIC -o $@ -c src/common.c

build/$(VARIANT)/vector.o : src/vector.c include/derp/vector.h include/derp/common.h $(MAKEFILES)
	$(CC) $(CFLAGS) -o $@ -c src/vector.c
build/$(VARIANT)/pic/vector.o : src/vector.c include/derp/vector.h include/derp/common.h $(MAKEFILES)
	$(CC) $(CFLAGS) -fPIC -o $@ -c src/vector.c

build/$(VARIANT)/list.o : src/list.c include/derp/list.h include/derp/common.h $(MAKEFILES)
	$(CC) $(CFLAGS) -o $@ -c src/list.c
build/$(VARIANT)/pic/list.o : src/list.c include/derp/list.h include/derp/common.h $(MAKEFILES)
	$(CC) $(CFLAGS) -fPIC -o $@ -c src/list.c

build/$(VARIANT)/hashmap.o : src/hashmap.c include/derp/hashmap.h include/derp/list.h include/derp/common.h $(MAKEFILES)
	$(CC) $(CFLAGS) -o $@ -c src/hashmap.c
build/$(VARIANT)/pic/hashmap.o : src/hashmap.c include/derp/hashmap.h include/derp/list.h include/derp/common.h $(MAKEFILES)
	$(CC) $(CFLAGS) -fPIC -o $@ -c src/hashmap.c

build/$(VARIANT)/treemap.o : src/treemap.c include/derp/treemap.h include/derp/list.h include/derp/common.h $(MAKEFILES)
	$(CC) $(CFLAGS) -o $@ -c src/treemap.c
build/$(VARIANT)/pic/treemap.o : src/treemap.c include/derp/treemap.h include/derp/list.h include/derp/common.h $(MAKEFILES)
	$(CC) $(CFLAGS) -fPIC -o $@ -c src/treemap.c

build/$(VARIANT)/test/t_vector : build/$(VARIANT)/common.o build/$(VARIANT)/vector.o test/t_vector.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/common.o build/$(VARIANT)/vector.o test/t_vector.c $(LDLIBS)

build/$(VARIANT)/test/t_list : build/$(VARIANT)/common.o build/$(VARIANT)/list.o test/t_list.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/common.o build/$(VARIANT)/list.o test/t_list.c $(LDLIBS)

build/$(VARIANT)/test/t_hashmap : build/$(VARIANT)/common.o build/$(VARIANT)/hashmap.o build/$(VARIANT)/list.o test/t_hashmap.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/common.o build/$(VARIANT)/hashmap.o build/$(VARIANT)/list.o test/t_hashmap.c $(LDLIBS)

build/$(VARIANT)/test/t_treemap : build/$(VARIANT)/common.o build/$(VARIANT)/treemap.o build/$(VARIANT)/list.o test/t_treemap.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/$(VARIANT)/common.o build/$(VARIANT)/treemap.o build/$(VARIANT)/list.o test/t_treemap.c $(LDLIBS)
