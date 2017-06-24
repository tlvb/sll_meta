vpath %.h src/
vpath %.c src/

.PHONY: default

CFLAGS += -g
CFLAGS += -Werror -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wcast-qual -Wunreachable-code
CFLAGS += -O2 -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fPIE -pie
CFLAGS += -Wl,-z,relro -Wl,-z,now

default: example

example: example_main.o
	$(CC) $(CFLAGS) -o $@ $^

example_main.o: example_main.c sll_meta.h
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm *.o example || true
