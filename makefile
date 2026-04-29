CC = ccache clang

SRCS != find . -name '*.c'
LIBS = -lssl -lcrypto -lncursesw -ltinfow

all: browser

browser: $(SRCS)
	$(CC) $(SRCS) -o prog $(LIBS)

debug: $(SRCS)
	$(CC) -g -O0 $(SRCS) -o prog.dbg $(LIBS)

clean:
	rm *.o prog prog.dbg
