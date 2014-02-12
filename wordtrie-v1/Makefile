CFLAGS?=-O0 -pipe
CFLAGS+=-g -Wall -Wextra -Wpedantic -std=c99

all: wordtrie tagtest print_wtrie

download:
	echo "Downloading data sample..."; \
	wget "http://storage.googleapis.com/books/ngrams/books/googlebooks-eng-1M-2gram-20090715-0.csv.zip" -O data.zip; \
	unzip data.zip; \
	rm data.zip

wordtrie: main.o wordtrie.o gngrams.o tagptr.o tagarray.o util.o mempool.o

print_wtrie: print_wtrie.o mempool.o tagptr.o tagarray.o util.o wordtrie.o

tagtest: tagtest.o tagptr.o tagarray.o util.o mempool.o

clean:
	rm -f *.o
	rm -f wordtrie tagtest

