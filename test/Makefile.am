AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = test

AM_CFLAGS = -g -O2 -I../include 

test_SOURCES = usage.c

test_LDADD = ../src/libusage.a -lpthread