CC = clang
CFLAGS += -std=c99 -pedantic -Wall -Wextra -Werror -O0 -g
LDFLAGS += -fprofile-instr-generate -fcoverage-mapping \
           -fsanitize=address -fsanitize=undefined
