#/bin/bash

rm -R .git
rm -R README.md
echo "# Funcoes_Matematicas" >> README.md
git init
git add README.md
git commit -m "Funcoes_Matematicas: Adicao do Arquivo README.md"
git branch -M main
git remote add origin https://github.com/Hebervaldo/Funcoes_Matematicas.git
./Rotina_git_Enviar_Alteracoes.sh
