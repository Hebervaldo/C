#!/bin/bash

killall -r Controle_GPIO_W
chmod +x Script_Instalar_Biblioteca_WiringPi_Raspian.sh
./Script_Instalar_Biblioteca_WiringPi_Raspian.sh
cd Biblioteca/
chmod +x Criar_Biblioteca_i2clcd.sh
./Criar_Biblioteca_i2clcd.sh
chmod +x Criar_Biblioteca_getipv4.sh
./Criar_Biblioteca_getipv4.sh
cd ..
gcc Controle_GPIO_WiringPi_Raspberry.c ./Biblioteca/i2clcd.o ./Biblioteca/getipv4.o -I./Biblioteca -L./Biblioteca -o Controle_GPIO_WiringPi_Raspberry.o -lwiringPi
chmod +x Controle_GPIO_WiringPi_Raspberry.o
chmod +x Script_Exemplo.sh
# ./Script_Exemplo.sh
