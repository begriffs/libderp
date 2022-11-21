CC = clang
CFLAGS  += -std=c99 -pedantic -Wall -Wextra -Werror -O0 \
           -DDERP_USE_BOEHM_GC --coverage
LDLIBS  += -lgc
