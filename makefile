CC = ccache clang

SRCS != find . -name '*.c'

all: browser

browser: $(SRCS)
	$(CC) $(SRCS) -o prog

debug: $(SRCS)
	$(CC) -g -O0 $(SRCS) -o prog.dbg

clean:
	rm *.o prog prog.dbg
