#!/bin/bash

usuario="Hebervaldo"
token="$(echo `cat /home/usuario/Documentos/git/$usuario/Token_Git_Hebervaldo.txt`)"
nome_repositorio="Funcoes_Matematicas"

declare -a v_extensao=( "*.c" "*.sh" )

for i in "${v_extensao[@]}"
do
	for j in $i
	do
		git add $j
	done
done

git commit -m "Funcoes_Matematicas: Enviando arquivos atualizados *.c"
git push https://$token@github.com/$usuario/$nome_repositorio.git