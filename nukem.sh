#!/bin/bash

# This script will clean out a project of all files that were generated
# while building.  Automake generates all kinds of file, and sometimes
# it is nice to be able to see only the code from the repository.

make distclean

for i in `find . -name Makefile.in`; do
    rm $i
done

for i in `find . -name Makefile`; do
    rm $i
done

for i in `find . -name '*~'`; do
    rm $i
done

rm -f aclocal.m4
rm -fR autom4te-2.53.cache
rm -f config.guess
rm -f config.sub
rm -f configure
rm -f depcomp
rm -fR docs/hld/OpenHPI_HLD/

