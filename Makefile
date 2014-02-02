CFLAGS?=-g -Wall -std=c99

all: wordtrie tagtest

download:
	echo "Downloading data sample..."; \
    wget "http://storage.googleapis.com/books/ngrams/books/googlebooks-eng-1M-2gram-20090715-0.csv.zip" -O data.zip; \
	unzip data.zip; \
	rm data.zip

wordtrie: main.o wordtrie.o gngrams.o tagptr.o

tagtest: tagtest.o tagptr.o

clean:
	rm -f *.o
	rm -f wordtrie tagtest

