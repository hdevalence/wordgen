wordgen
=======

let's play with google ngrams data

usage
-----

run `make download` to download some sample data, or visit the [google page][0] to get more of it.

    ./wordtrie 'ngram string' data1.csv [data2.csv data3.csv...]

builds a data structure and fetches information about the ngram string.

notes
-----

there are a lot of inline functions used instead of macros. hence compiling without optimization makes the code much slower.

todo
----

* rewrite the code in rust, and handle the v2 dataset, which has part of speech tags.


[0]: http://storage.googleapis.com/books/ngrams/books/datasetsv2.html

