CFLAGS?=-g -Wall -std=c99 -Wno-format

all: wordtrie

wordtrie: main.o wordtrie.o gngrams.o

clean:
	rm -f *.o
	rm -f wordtrie

