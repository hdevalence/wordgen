CFLAGS?=-g -Wall -std=c99 -Wno-format

all: wordtrie tagtest

wordtrie: main.o wordtrie.o gngrams.o

tagtest: tagtest.o tagptr.o

clean:
	rm -f *.o
	rm -f wordtrie tagtest

