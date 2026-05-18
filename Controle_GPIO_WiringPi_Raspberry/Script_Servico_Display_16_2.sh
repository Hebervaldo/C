#!/bin/bash

diretorio_Padrao=$(pwd)

chmod +x Criar_Controle_GPIO_WiringPi_Raspberry.sh
"$diretorio_Padrao/Criar_Controle_GPIO_WiringPi_Raspberry.sh"

echo '[Unit]
Description=Display_16_2
After=multi-user.target

[Service]
Type=simple
ExecStart='$diretorio_Padrao'/Controle_GPIO_WiringPi_Raspberry.o -r 63 16
Restart=on-abort

[Install]
WantedBy=multi-user.target' > "/lib/systemd/system/display_16_2.service"

chmod 644 /lib/systemd/system/display_16_2.service
systemctl daemon-reload
systemctl enable display_16_2.service
systemctl start display_16_2.service
