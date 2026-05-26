#!/bin/bash

./RedeNeural.o 060 1000000 .00001
mv erro_TreinamentoRedeNeural.dat erro_TreinamentoRedeNeural_060.dat
mv resultadostreinamento.dat resultadostreinamento_060.dat
./RedeNeural.o -1
mv erro_ExecucaoRedeNeural.dat erro_ExecucaoRedeNeural.dat
