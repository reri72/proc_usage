#!/bin/sh

libtoolize
autoscan
aclocal
autoheader
autoconf
automake --force-missing --add-missing
./configure