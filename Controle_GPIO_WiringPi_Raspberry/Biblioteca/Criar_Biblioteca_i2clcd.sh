#!/bin/bash

rm i2clcd.o
rm li2clcd.a
gcc -c i2clcd.c -o i2clcd.o -lwiringPi
chmod -x i2clcd.o
ar -cru li2clcd.a i2clcd.o
