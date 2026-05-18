#!/bin/bash

killall -r Controle_GPIO_W
chmod +x Controle_GPIO_WiringPi_Raspberry.o
# ./Controle_GPIO_WiringPi_Raspberry.o -t 63 16
./Controle_GPIO_WiringPi_Raspberry.o -r 63 16
