CC = clang
CFLAGS  += -std=c99 -pedantic -Wall -Wextra -Werror -O0 \
           $(EXTRA_DEV_CFLAGS) --coverage
LDFLAGS += $(EXTRA_DEV_LDFLAGS)
LDLIBS  += -lgc
