CC = clang
CFLAGS += -std=c99 -pedantic -DMALLOC_STATS \
          -Wall -Wextra -Werror -O0 -g # \
          # -fprofile-instr-generate -fcoverage-mapping 
