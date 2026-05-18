#!/bin/bash

DIRETORIO_ATUAL=$(pwd)
DIRECTORY="WiringPi"

cd ~/

if [ ! -d "$DIRECTORY" ]; then
	sudo apt-get install wiringpi
	gpio -v
	# sudo apt-get purge wiringpi
	# hash -r

	sudo apt-get install git-core -y
	sudo apt-get update -y
	sudo apt-get upgrade -y
	cd
	git clone https://github.com/WiringPi/WiringPi
	cd "$DIRECTORY"
	git pull origin
	cd "$DIRECTORY"
	./build
fi

cd "$DIRETORIO_ATUAL"
