CC = clang
CFLAGS += -std=c99 -pedantic -Wall -Wextra -Werror -O0 -g --coverage
LDFLAGS += -fsanitize=address -fsanitize=undefined -fsanitize=leak
