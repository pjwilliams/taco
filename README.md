taco
====

Introduction
------------

taco is a toolkit for implementing TArget COnstraints in a target-constrained
statistical machine translation (SMT) system, such as that described in

  "Agreement Constraints for Statistical Machine Translation into German"
  Philip Williams and Philipp Koehn, in WMT 2011.

and

  "Using Feature Structures to Improve Verb Translation in English-to-German
  Statistical MT"
  Philip Williams and Philipp Koehn, in HyTra 2014.

The toolkit provides a language-independent library, libtaco, intended for use
with a decoder that supports string-to-tree or tree-to-tree translation (such
as Moses).  It also provides a number of language- and annotation-specific
tools for extracting target constraints and feature structure lexicons from
training data.


Caveats
-------
...


Requirements
------------

Boost 1.48 or later.


Build Instructions
------------------

$ autoreconf -i
$ ./configure --with-boost=/path/to/boost --prefix=/path/to/dest
$ make && make install
