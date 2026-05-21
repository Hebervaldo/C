#include <math.h>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>

#define rando() ((double)rand()/RAND_MAX)

int TipoSaida = 0;

int TipoErro = 0;

int TipoDeltaS = 0;

int i = 0;

int j = 0;

int k = 0;

int p = 0;

int np = 0;

int op = 0;

int epoca = 0;

int numPadroes = 0;

int numEntrada = 0;

int numEscondida = 100;

int numSaida = 0;

int numIteracoes = 100;

int minimoValorEntrada = 0;

int maximoValorEntrada = 0;

int minimoValorAlvo = 0;

int maximoValorAlvo = 0;

// int ranpad[NUMEROPADROES+1];

int *ranpad;

// double entrada[NUMEROPADROES+1][NUMEROENTRADAS+1];

double **entrada;

// double target[NUMEROPADROES+1][NUMEROSAIDAS+1];

double **target;

// double SomaEscondida[NUMEROPADROES+1][NUMEROESCONDIDA+1];

double **SomaEscondida;

// double W12[NUMEROENTRADAS+1][NUMEROESCONDIDA+1];

double **W12;

// double Escondida[NUMEROPADROES+1][NUMEROESCONDIDA+1];

double **Escondida;

// double SomaSaida[NUMEROPADROES+1][NUMEROSAIDAS+1]; 

double **SomaSaida;

// double W23[NUMEROESCONDIDA+1][NUMEROSAIDAS+1]; 

double **W23;

// double SAIDA[NUMEROPADROES+1][NUMEROSAIDAS+1];

double **Saida;

// double DeltaS[NUMEROSAIDAS+1];

double *DeltaS;

// double somaDWS[NUMEROESCONDIDA+1];

double *somaDWS;

// double DeltaE[NUMEROESCONDIDA+1];

double *DeltaE;

// double DeltaW12[NUMEROENTRADAS+1][NUMEROESCONDIDA+1];

double **DeltaW12;

// double DeltaW23[NUMEROESCONDIDA+1][NUMEROSAIDAS+1];

double **DeltaW23;

double Erro = 0;

double eta = 0.05;

double alpha = 0;

double wmax = 1;

double erroLimite = 0.0004;

double** mtdGerarArquivoMatriz(char *Arquivo, int *coluna, int *linha, int *comprimento, int *minimoValor, int *maximoValor)
{
	char numero[1000];
	int i = 0;
	int j = 0;
	int enterRepetido = 1;
	int espacoRepetido = 1;
	int pontoRepetido = 0;
	int ultimaEntrada = 0;
	int maxcoluna = 0;
	int contador = 0;
	int numeroEspaco = 0;
	int chr = 0;
	double vetnum[100000];
	double **Matriz;

	FILE *cfPtr;
	cfPtr = fopen(Arquivo, "r");
	(*coluna) = 0;
	(*linha) = 0;

	while ((chr = getc(cfPtr)) != EOF)
	{
		if (((chr == '.' | chr == ',') & pontoRepetido == 0) | (chr >= '0' & chr <= '9'))
		{
			if (chr == ',')
			{
				chr = '.';
			}
			numero[contador++] = chr;
			numero[contador] = 0;
			vetnum[numeroEspaco] = atof(numero);
			enterRepetido = 0;
			espacoRepetido = 0;
			if ((chr == '.' | chr == ','))
			{
				pontoRepetido++;
			}
			ultimaEntrada = 1;
		}
		else
		{
			if (!(chr == '.' | chr == ','))
			{
				contador = 0;

				if (espacoRepetido == 0)
				{
					(*coluna)++;
					numeroEspaco++;
				}
				espacoRepetido++;

				if (chr == 10 | chr == 13)
				{
					if (enterRepetido == 0)
					{
						if (maxcoluna < (*coluna))
						{
							maxcoluna = (*coluna);
						}
						(*coluna) = 0;
						(*linha)++;
					}
					enterRepetido++;
				}

				pontoRepetido = 0;
			}
			else
			{
				enterRepetido = 0;
				espacoRepetido = 0;
				pontoRepetido++;
			}
			ultimaEntrada = 0;
		}
	}

	numeroEspaco++;
	(*linha)++;
	(*coluna) = maxcoluna;
	(*comprimento) = numeroEspaco;

	if (ultimaEntrada == 0)
	{
		(*linha)--;
	}

	(*minimoValor) = vetnum[0];
	(*maximoValor) = vetnum[0];

	for (i = 0; i < (*comprimento); i++)
	{
		if ((*minimoValor) >= vetnum[i])
		{
			(*minimoValor) = vetnum[i];
		}
		if ((*maximoValor) <= vetnum[i])
		{
			(*maximoValor) = vetnum[i];
		}
	}

	Matriz = (double **)malloc((*linha + 1) * sizeof(double *));

	for (i = 0; i < (*linha + 1); i++)
	{
		Matriz[i] = (double *)malloc((*coluna + 1) * sizeof(double));
	}

	for (i = 1; i < (*linha + 1); i++)
	{
		for (j = 1; j < (*coluna + 1); j++)
		{
			Matriz[i][j] = ((vetnum[(int)(((i - 1) * (*coluna)) + (j - 1))]) - (*minimoValor)) / ((*maximoValor) - (*minimoValor));
		}
	}

	fclose(cfPtr);

	return Matriz;
}

void mtdGerarVetorMatriz(void)
{
	// int ranpad[NUMEROPADROES+1];
	ranpad = (int *)malloc((numPadroes + 1) * sizeof(int));

	// double SomaEscondida[NUMEROPADROES+1][NUMEROESCONDIDA+1];
	SomaEscondida = (double **)malloc((numPadroes + 1) * sizeof(double *));

	for (i = 0; i < (numPadroes + 1); i++)
	{
		SomaEscondida[i] = (double *)malloc((numEscondida + 1) * sizeof(double));
	}

	// double W12[NUMEROENTRADAS+1][NUMEROESCONDIDA+1];
	W12 = (double **)malloc((numEntrada + 1) * sizeof(double *));

	for (i = 0; i < (numEntrada + 1); i++)
	{
		W12[i] = (double *)malloc((numEscondida + 1) * sizeof(double));
	}

	// double Escondida[NUMEROPADROES+1][NUMEROESCONDIDA+1];
	Escondida = (double **)malloc((numPadroes + 1) * sizeof(double *));

	for (i = 0; i < (numPadroes + 1); i++)
	{
		Escondida[i] = (double *)malloc((numEscondida + 1) * sizeof(double));
	}

	// double SomaSaida[NUMEROPADROES+1][NUMEROSAIDAS+1]; 
	SomaSaida = (double **)malloc((numPadroes + 1) * sizeof(double *));

	for (i = 0; i < (numPadroes + 1); i++)
	{
		SomaSaida[i] = (double *)malloc((numSaida + 1) * sizeof(double));
	}

	// double W23[NUMEROESCONDIDA+1][NUMEROSAIDAS+1]; 
	W23 = (double **)malloc((numEscondida + 1) * sizeof(double *));

	for (i = 0; i < (numEscondida + 1); i++)
	{
		W23[i] = (double *)malloc((numSaida + 1) * sizeof(double));
	}

	// double SAIDA[NUMEROPADROES+1][NUMEROSAIDAS+1];
	Saida = (double **)malloc((numPadroes + 1) * sizeof(double *));

	for (i = 0; i < (numPadroes + 1); i++)
	{
		Saida[i] = (double *)malloc((numSaida + 1) * sizeof(double));
	}

	// double DeltaS[NUMEROSAIDAS+1];
	DeltaS = (double *)malloc((numSaida + 1) * sizeof(double));

	// double somaDWS[NUMEROESCONDIDA+1];
	somaDWS = (double *)malloc((numEscondida + 1) * sizeof(double));

	// double DeltaE[NUMEROESCONDIDA+1];
	DeltaE = (double *)malloc((numEscondida + 1) * sizeof(double));

	// double DeltaW12[NUMEROENTRADAS+1][NUMEROESCONDIDA+1];
	DeltaW12 = (double **)malloc((numEntrada + 1) * sizeof(double *));

	for (i = 0; i < (numEntrada + 1); i++)
	{
		DeltaW12[i] = (double *)malloc((numEscondida + 1) * sizeof(double));
	}

	// double DeltaW23[NUMEROESCONDIDA+1][NUMEROSAIDAS+1];
	DeltaW23 = (double **)malloc((numEscondida + 1) * sizeof(double *));

	for (i = 0; i < (numEscondida + 1); i++)
	{
		DeltaW23[i] = (double *)malloc((numSaida + 1) * sizeof(double));
	}
}

void mtdDestruirVetorMatriz(void)
{
	// int ranpad[NUMEROPADROES+1];

	free(ranpad);

	// double entrada[NUMEROPADROES+1][NUMEROENTRADAS+1];

	for (i = 0; i < (numPadroes + 1); i++)
	{
		free(entrada[i]);
	}

	free(entrada);

	// double target[NUMEROPADROES+1][NUMEROSAIDAS+1];

	for (i = 0; i < (numPadroes + 1); i++)
	{
		free(target[i]);
	}

	free(target);

	// double SomaEscondida[NUMEROPADROES+1][NUMEROESCONDIDA+1];

	for (i = 0; i < (numPadroes + 1); i++)
	{
		free(SomaEscondida[i]);
	}

	free(SomaEscondida);

	// double W12[NUMEROENTRADAS+1][NUMEROESCONDIDA+1];

	for (i = 0; i < (numEntrada + 1); i++)
	{
		free(W12[i]);
	}

	free(W12);

	// double Escondida[NUMEROPADROES+1][NUMEROESCONDIDA+1];

	for (i = 0; i < (numPadroes + 1); i++)
	{
		free(Escondida[i]);
	}

	free(Escondida);

	// double SomaSaida[NUMEROPADROES+1][NUMEROSAIDAS+1]; 

	for (i = 0; i < (numPadroes + 1); i++)
	{
		free(SomaSaida[i]);
	}

	free(SomaSaida);

	// double W23[NUMEROESCONDIDA+1][NUMEROSAIDAS+1]; 

	for (i = 0; i < (numEscondida + 1); i++)
	{
		free(W23[i]);
	}

	free(W23);

	// double SAIDA[NUMEROPADROES+1][NUMEROSAIDAS+1];

	for (i = 0; i < (numPadroes + 1); i++)
	{
		free(Saida[i]);
	}

	free(Saida);

	// double DeltaS[NUMEROSAIDAS+1];

	free(DeltaS);

	// double somaDWS[NUMEROESCONDIDA+1];

	free(somaDWS);

	// double DeltaE[NUMEROESCONDIDA+1];

	free(DeltaE);

	// double DeltaW12[NUMEROENTRADAS+1][NUMEROESCONDIDA+1];

	for (i = 0; i < (numEntrada + 1); i++)
	{
		free(DeltaW12[i]);
	}

	free(DeltaW12);

	// double DeltaW23[NUMEROESCONDIDA+1][NUMEROSAIDAS+1];

	for (i = 0; i < (numEscondida + 1); i++)
	{
		free(DeltaW23[i]);
	}

	free(DeltaW23);
}

void mtdObterEntradasTreinamento(void)
{
	int coluna = 0;
	int linha = 0;
	int comprimento = 0;
	// double entrada[NUMEROPADROES+1][NUMEROENTRADAS+1];
	entrada = mtdGerarArquivoMatriz("entradastreinamento.dat", &coluna, &linha, &comprimento, &minimoValorEntrada, &maximoValorEntrada);
	numEntrada = coluna;
	numPadroes = linha;
}

void mtdObterEntradasExecucao(void)
{
	int coluna = 0;
	int linha = 0;
	int comprimento = 0;
	// double entrada[NUMEROPADROES+1][NUMEROENTRADAS+1];
	entrada = mtdGerarArquivoMatriz("entradasteste.dat", &coluna, &linha, &comprimento, &minimoValorEntrada, &maximoValorEntrada);
	numEntrada = coluna;
	numPadroes = linha;
}

void mtdObterAlvosTreinamento(void)
{
	int coluna = 0;
	int linha = 0;
	int comprimento = 0;
	// double target[NUMEROPADROES+1][NUMEROSAIDAS+1];
	target = mtdGerarArquivoMatriz("target.dat", &coluna, &linha, &comprimento, &minimoValorAlvo, &maximoValorAlvo);
	numSaida = coluna;
	numPadroes = linha;
}

void mtdObterAlvosExecucao(int linha)
{
	mtdObterAlvosTreinamento();

	int coluna = numSaida;

	// double target[NUMEROPADROES+1][NUMEROSAIDAS+1];

	target = (double **)malloc((linha + 1) * sizeof(double *));

	for (i = 0; i < (linha + 1); i++)
	{
		target[i] = (double *)malloc((coluna + 1) * sizeof(double));
	}

	for (i = 0; i < (linha + 1); i++)
	{
		for (j = 1; j < (coluna + 1); j++)
		{
			target[i][j] = 0;
		}
	}

	numSaida = coluna;
	numPadroes = linha;
}

void mtdExportarPesos(void)
{
	int  m, n;
	FILE *cfPtr;
	cfPtr = fopen("pesos.dat", "w");

	for (n = 0; n < (numEscondida + 1); n++)
	{
		for (m = 0; m < (numEntrada + 1); m++)
		{
			fprintf(cfPtr, "%lf\t", W12[m][n]);
		}

		fprintf(cfPtr, "\n");
	}

	for (n = 0; n < (numSaida + 1); n++)
	{
		for (m = 0; m < (numEscondida + 1); m++)
		{
			fprintf(cfPtr, "%lf\t", W23[m][n]);
		}

		fprintf(cfPtr, "\n");
	}

	fclose(cfPtr);
}

void mtdIniciarPesos()
{
	int m, n;
	FILE *cfPtr;
	cfPtr = fopen("pesos.dat", "r");

	for (n = 0; n < (numEscondida + 1); n++)
	{
		for (m = 0; m < (numEntrada + 1); m++)
		{
			fscanf(cfPtr, "%lf", &W12[m][n]);
		}
	}

	for (n = 0; n < (numSaida + 1); n++)
	{
		for (m = 0; m < (numEscondida + 1); m++)
		{
			fscanf(cfPtr, "%lf", &W23[m][n]);
		}
	}

	fclose(cfPtr);
}

void mtdEscreverSaida(void)
{
	FILE *cfPtr;
	cfPtr = fopen("resultados.dat", "w");

	fprintf(cfPtr, "NETWORK DATA - Epoca %d\n\nPat:\t", epoca); // Mostra as SAIDAs

	for (i = 1; i < (numEntrada + 1); i++)
	{
		fprintf(cfPtr, "Entrada%-4d\t", i);
	}

	for (k = 1; k < (numSaida + 1); k++)
	{
		fprintf(cfPtr, "Alvo%-4d\tSaida%-4d\t", k, k);
	}

	for (p = 1; p < (numPadroes + 1); p++)
	{
		fprintf(cfPtr, "\n%d\t", p);

		for (i = 1; i < (numEntrada + 1); i++)
		{
			fprintf(cfPtr, "%lf\t", entrada[p][i] * (maximoValorEntrada - minimoValorEntrada) + (minimoValorEntrada));
		}

		for (k = 1; k < (numSaida + 1); k++)
		{
			fprintf(cfPtr, "%lf\t%lf\t", target[p][k] * maximoValorAlvo, Saida[p][k] * (maximoValorAlvo - minimoValorAlvo) + (minimoValorAlvo));
		}
	}

	fprintf(stdout, "NETWORK DATA - Epoca %d\n\nPat:\t", epoca); // Mostra as SAIDAs

	for (i = 1; i < (numEntrada + 1); i++)
	{
		fprintf(stdout, "Entrada%-4d\t", i);
	}

	for (k = 1; k < (numSaida + 1); k++)
	{
		fprintf(stdout, "Alvo%-4d\tSaida%-4d\t", k, k);
	}

	for (p = 1; p < (numPadroes + 1); p++)
	{
		fprintf(stdout, "\n%d\t", p);
		for (i = 1; i < (numEntrada + 1); i++)
		{
			fprintf(stdout, "%lf\t", entrada[p][i] * (maximoValorEntrada - minimoValorEntrada) + (minimoValorEntrada));
		}
		for (k = 1; k < (numSaida + 1); k++)
		{
			fprintf(stdout, "%lf\t%lf\t", target[p][k] * maximoValorAlvo, Saida[p][k] * (maximoValorAlvo - minimoValorAlvo) + (minimoValorAlvo));
		}
	}

	printf("\n");

	fclose(cfPtr);
}

int mtdTreinarRedeNeural(void)
{
	int retorno = 0;

	FILE *cfPtr;

	mtdObterEntradasTreinamento();
	mtdObterAlvosTreinamento();
	mtdGerarVetorMatriz();

	cfPtr = fopen("erro.dat", "w");

	for (j = 1; j < (numEscondida + 1); j++)
	{
		// Inicializa W12 e DeltaW12
		for (i = 0; i < (numEntrada + 1); i++)
		{
			DeltaW12[i][j] = 0.0;
			W12[i][j] = 2.0 * (rando() - 0.5) * wmax;
		}
	}

	for (k = 1; k < (numSaida + 1); k++)
	{
		// Inicializa W23 e DeltaW23
		for (j = 0; j < (numEscondida + 1); j++)
		{
			DeltaW23[j][k] = 0.0;
			W23[j][k] = 2.0 * (rando() - 0.5) * wmax;
		}
	}

	for (epoca = 0; epoca < numIteracoes; epoca++)
	{
		// Faz a iteração da atualização dos pesos
		for (p = 1; p < (numPadroes + 1); p++)
		{
			// Randomiza a ordem dos indivíduos
			ranpad[p] = p;
		}

		for (p = 1; p < (numPadroes + 1); p++)
		{
			np = (p + rando() * (numPadroes - p + 0));
			op = ranpad[p];
			ranpad[p] = ranpad[np];
			ranpad[np] = op;
		}
		Erro = 0.0;

		for (np = 1; np < (numPadroes + 1); np++)
		{
			// Repete para todos os padrões de treinamento
			p = ranpad[np];

			for (j = 1; j < (numEscondida + 1); j++)
			{
				// Computa as ativações da unidade escondida
				SomaEscondida[p][j] = W12[0][j];
				for (i = 1; i < (numEntrada + 1); i++)
				{
					SomaEscondida[p][j] += entrada[p][i] * W12[i][j];
				}
				Escondida[p][j] = 1.0 / (1.0 + exp(-SomaEscondida[p][j]));
			}

			for (k = 1; k < (numSaida + 1); k++)
			{
				// Computa as unidades de ativação da saída e erros
				SomaSaida[p][k] = W23[0][k];
				for (j = 1; j < (numEscondida + 1); j++)
				{
					SomaSaida[p][k] += Escondida[p][j] * W23[j][k];
				}

				switch (TipoSaida)
				{
				case 0:
					Saida[p][k] = 1.0 / (1.0 + exp(-SomaSaida[p][k])); // Sigmoidal SAIDAs

					break;
				case 1:
					Saida[p][k] = SomaSaida[p][k]; // Linear SAIDAs

					break;
				}

				switch (TipoErro)
				{
				case 0:
					Erro += 0.5 * (target[p][k] - Saida[p][k]) * (target[p][k] - Saida[p][k]); // SSE

					break;
				case 1:
					Erro -= (target[p][k] * log(Saida[p][k]) + (1.0 - target[p][k]) * log(1.0 - Saida[p][k])); // Erro de Entropia Cruzada

					break;
				}

				switch (TipoDeltaS)
				{
				case 0:
					DeltaS[k] = (target[p][k] - Saida[p][k]) * Saida[p][k] * (1.0 - Saida[p][k]); // Sigmoidal SAIDAs, SSE */

					break;
				case 1:
					DeltaS[k] = target[p][k] - Saida[p][k]; // Sigmoidal SAIDAs, Cross-Entropy Erro

					break;
				case 2:
					DeltaS[k] = target[p][k] - Saida[p][k]; // Linear SAIDAs, SSE

					break;
				}
			}

			for (j = 1; j < (numEscondida + 1); j++)
			{
				// Retropropagação de erros para a camada escondida
				somaDWS[j] = 0.0;
				for (k = 1; k < (numSaida + 1); k++)
				{
					somaDWS[j] += W23[j][k] * DeltaS[k];
				}
				DeltaE[j] = somaDWS[j] * Escondida[p][j] * (1.0 - Escondida[p][j]);
			}

			for (j = 1; j < (numEscondida + 1); j++)
			{
				// Atualiza pesos w12
				DeltaW12[0][j] = eta * DeltaE[j] + alpha * DeltaW12[0][j];
				W12[0][j] += DeltaW12[0][j];
				for (i = 1; i < (numEntrada + 1); i++)
				{
					DeltaW12[i][j] = eta * entrada[p][i] * DeltaE[j] + alpha * DeltaW12[i][j];
					W12[i][j] += DeltaW12[i][j];
				}
			}

			for (k = 1; k < (numSaida + 1); k++)
			{
				// Atualiza pesos W23 */
				DeltaW23[0][k] = eta * DeltaS[k] + alpha * DeltaW23[0][k];
				W23[0][k] += DeltaW23[0][k];
				for (j = 1; j < (numEscondida + 1); j++)
				{
					DeltaW23[j][k] = eta * Escondida[p][j] * DeltaS[k] + alpha * DeltaW23[j][k];
					W23[j][k] += DeltaW23[j][k];
				}
			}
		}

		if (epoca % 1000 == 0)
		{
			fprintf(cfPtr, "\nEpoca:\t%d\tErro:\t%lf\n", epoca, Erro);
		}

		if (Erro < erroLimite)
		{
			break; // Para o aprendizado quando o erro convergir para o valor descrito
		}
	}

	fclose(cfPtr);

	mtdExportarPesos();
	mtdEscreverSaida();
	mtdDestruirVetorMatriz();
	retorno = 1;

	return retorno;
}

int mtdExecutarRedeNeural()
{
	int retorno = 0;

	mtdObterEntradasExecucao();
	mtdObterAlvosExecucao(numPadroes);
	mtdGerarVetorMatriz();
	mtdIniciarPesos();

	for (epoca = 0; epoca < numIteracoes; epoca++)
	{
		// Faz a iteração da atualização dos pesos
		for (p = 1; p < (numPadroes + 1); p++)
		{
			// Randomiza a ordem dos indivíduos
			ranpad[p] = p;
		}
		for (p = 1; p < (numPadroes + 1); p++)
		{
			np = (p + rando() * (numPadroes - p + 0));
			op = ranpad[p];
			ranpad[p] = ranpad[np];
			ranpad[np] = op;
		}
		Erro = 0.0;
		for (np = 1; np < (numPadroes + 1); np++)
		{
			// Repete para todos os padrões de treinamento
			p = ranpad[np];
			for (j = 1; j < (numEscondida + 1); j++)
			{
				// Computa as ativações da unidade escondida
				SomaEscondida[p][j] = W12[0][j];
				for (i = 1; i < (numEntrada + 1); i++)
				{
					SomaEscondida[p][j] += entrada[p][i] * W12[i][j];
				}
				Escondida[p][j] = 1.0 / (1.0 + exp(-SomaEscondida[p][j]));
			}
			for (k = 1; k < (numSaida + 1); k++)
			{
				// Computa as unidades de ativação da saída e erros
				SomaSaida[p][k] = W23[0][k];
				for (j = 1; j < (numEscondida + 1); j++)
				{
					SomaSaida[p][k] += Escondida[p][j] * W23[j][k];
				}

				switch (TipoSaida)
				{
				case 0:
					Saida[p][k] = 1.0 / (1.0 + exp(-SomaSaida[p][k])); // Sigmoidal SAIDAs

					break;
				case 1:
					Saida[p][k] = SomaSaida[p][k]; // Linear SAIDAs

					break;
				}

				switch (TipoErro)
				{
				case 0:
					Erro += 0.5 * (target[p][k] - Saida[p][k]) * (target[p][k] - Saida[p][k]); // SSE

					break;
				case 1:
					Erro -= (target[p][k] * log(Saida[p][k]) + (1.0 - target[p][k]) * log(1.0 - Saida[p][k])); // Erro de Entropia Cruzada

					break;
				}

				switch (TipoDeltaS)
				{
				case 0:
					DeltaS[k] = (target[p][k] - Saida[p][k]) * Saida[p][k] * (1.0 - Saida[p][k]); // Sigmoidal SAIDAs, SSE */

					break;
				case 1:
					DeltaS[k] = target[p][k] - Saida[p][k]; // Sigmoidal SAIDAs, Cross-Entropy Erro

					break;
				case 2:
					DeltaS[k] = target[p][k] - Saida[p][k]; // Linear SAIDAs, SSE

					break;
				}
			}
			for (j = 1; j < (numEscondida + 1); j++)
			{
				// Retropropagação de erros para a camada escondida
				somaDWS[j] = 0.0;
				for (k = 1; k < (numSaida + 1); k++)
				{
					somaDWS[j] += W23[j][k] * DeltaS[k];
				}
				DeltaE[j] = somaDWS[j] * Escondida[p][j] * (1.0 - Escondida[p][j]);
			}
			for (j = 1; j < (numEscondida + 1); j++)
			{
				// Atualiza pesos w12
				DeltaW12[0][j] = eta * DeltaE[j] + alpha * DeltaW12[0][j];
				W12[0][j] += DeltaW12[0][j];
				for (i = 1; i < (numEntrada + 1); i++)
				{
					DeltaW12[i][j] = eta * entrada[p][i] * DeltaE[j] + alpha * DeltaW12[i][j];
					W12[i][j] += DeltaW12[i][j];
				}
			}
			for (k = 1; k < (numSaida + 1); k++)
			{
				// Atualiza pesos W23
				DeltaW23[0][k] = eta * DeltaS[k] + alpha * DeltaW23[0][k];
				W23[0][k] += DeltaW23[0][k];
				for (j = 1; j < (numEscondida + 1); j++)
				{
					DeltaW23[j][k] = eta * Escondida[p][j] * DeltaS[k] + alpha * DeltaW23[j][k];
					W23[j][k] += DeltaW23[j][k];
				}
			}
		}

		fprintf(stdout, "\nEpoca:\t%d\tErro:\t%lf\n", epoca, Erro);

		if (Erro < erroLimite)
		{
			break; // Para o aprendizado quando o erro convergir para o valor descrito
		}
	}

	mtdEscreverSaida();
	mtdDestruirVetorMatriz();
	retorno = 1;

	return retorno;
}

void mtdSair(void)
{
	// system("exit");
	exit(1);
}

void mtdPadrao()
{
	int opcao = 0;
	int tempoInicial = 0;
	int tempoFinal = 0;

	do
	{
		system("cls || clear");
		printf("Menu Rede Neural - Escolha uma opcao\n");
		printf("0. Treinar a Rede Neural\n");
		printf("1. Executar a Rede Neural\n");
		printf("2. Sair.\n");

		scanf("%d", &opcao);
		system("cls || clear");

		switch (opcao)
		{
		case 0:
			printf("Digite o numero de neuronios da Rede Neural:\n");
			scanf("%d", &numEscondida);
			printf("Digite o numero de iteracoes da Rede Neural:\n");
			scanf("%d", &numIteracoes);
			printf("Digite o erro limite: \n");
			scanf("%lf", &erroLimite);
			// printf("Escolha o tipo de Saida: \n");
			// scanf("%d", &TipoSaida);
			// printf("Escolha o tipo de Erro: \n");
			// scanf("%lf", &TipoErro);
			// printf("Escolha o tipo de DeltaS: \n");
			// scanf("%lf", &TipoDeltaS);

			tempoInicial = time(NULL);
			if (mtdTreinarRedeNeural() == 1)
			{
				printf("Rede treinada com sucesso.\n");
			}
			else
			{
				printf("Ocorreram erros.\n");
			}
			tempoFinal = time(NULL);
			printf("Tempo decorrido para o treinamento da Rede Neural: %.0lf [s].\n", difftime(tempoFinal, tempoInicial));

			break;
		case 1:
			if (numEscondida <= 0)
			{
				printf("Digite o numero de neuronios da Rede Neural:\n");
				scanf("%d", &numEscondida);
			}
			numIteracoes = 1;
			if (erroLimite <= 0)
			{
				printf("Digite o erro limite: \n");
				scanf("%lf", &erroLimite);
			}
			// printf("Escolha o tipo de Saida: \n");
			// scanf("%d", &TipoSaida);
			// printf("Escolha o tipo de Erro: \n");
			// scanf("%lf", &TipoErro);
			// printf("Escolha o tipo de DeltaS: \n");
			// scanf("%lf", &TipoDeltaS);

			tempoInicial = time(NULL);
			if (mtdExecutarRedeNeural() == 1)
			{
				printf("Rede executada com sucesso.\n");
			}
			else
			{
				printf("Ocorreram erros.\n");
			}
			tempoFinal = time(NULL);
			printf("Tempo decorrido para a execucao da Rede Neural: %.0lf [s].\n", difftime(tempoFinal, tempoInicial));

			break;
		case 2:
			mtdSair();

			break;
		default:
			printf("Digite uma opcao valida.\n");

			break;
		}

		system("pause");
	} while (opcao != 2);
}

void mtdTreinamento(int Escondida, int Iteracoes, double ErroLimite)
{
	int tempoInicial = 0;
	int tempoFinal = 0;

	tempoInicial = time(NULL);
	numEscondida = Escondida;
	numIteracoes = Iteracoes;
	erroLimite = ErroLimite;
	if (mtdTreinarRedeNeural() == 1)
	{
		printf("Rede treinada com sucesso.\n");
	}
	else
	{
		printf("Ocorreram erros.\n");
	}
	tempoFinal = time(NULL);
	printf("Tempo decorrido para o treinamento da Rede Neural: %.0lf [s].\n", difftime(tempoFinal, tempoInicial));
	system("pause");
}

void mtdExecucao(int Escondida, int Iteracoes, double ErroLimite)
{
	int tempoInicial = 0;
	int tempoFinal = 0;

	tempoInicial = time(NULL);
	numEscondida = Escondida;
	numIteracoes = Iteracoes;
	erroLimite = ErroLimite;
	if (mtdExecutarRedeNeural() == 1)
	{
		printf("Rede executada com sucesso.\n");
	}
	else
	{
		printf("Ocorreram erros.\n");
	}
	tempoFinal = time(NULL);
	printf("Tempo decorrido para a execucao da Rede Neural: %.0lf [s].\n", difftime(tempoFinal, tempoInicial));
	system("pause");
}

int main(int argc, char *argv[])
{
	switch (argc)
	{
	case 3:
		mtdExecucao(atoi(argv[1]), 1, atof(argv[2]));
		break;
	case 4:
		mtdTreinamento(atoi(argv[1]), atoi(argv[2]), atof(argv[3]));
		break;
	default:
		mtdPadrao();
		break;
	}

	return 0;
}
