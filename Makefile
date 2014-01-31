CFLAGS?=-g -Wall -std=c99

all: wordtrie

wordtrie: main.o wordtrie.o gngrams.o

clean:
	rm -f *.o
	rm -f wordtrie

