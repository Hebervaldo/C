#!/bin/bash

rm getipv4.o
rm lgetipv4.a
gcc -c getipv4.c -o getipv4.o
chmod -x getipv4.o
ar -cru lgetipv4.a getipv4.o
