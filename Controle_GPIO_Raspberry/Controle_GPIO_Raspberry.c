#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define TAMANHO_STRING_CHAR	100

#define HIGH    1
#define LOW 	0
#define INPUT   0
#define OUTPUT  1

#define Intervalo_Padrao 1.0

char *itoa(int val, int base);

bool mtdAcessarPortaPinoGPIO(int Pino);
bool mtdConfigurarDirecaoPortaPinoGPIO(int Pino, int Direcao);
int mtdConfigurarPortaPinoValorEntradaGPIO(int Pino);
bool mtdConfigurarPortaPinoValorSaidaGPIO(int Pino, int Valor);
bool mtdDesExportarPortaPinoGPIO(int Pino);
bool mtdExportarPortaPinoGPIO(int Pino);
void mtdLerEntradaLoopImprimirSaidaLoopGPIO(int PinoEntrada, int PinoSaida, int Iteracoes, double Intervalo);

void delay(double time);

void mtdGerarLog_Tela(char *Registro);
void mtdAcrescentarLog_Arquivo(char *NomeArquivo, char *Registro);
void mtdEscreverLog_Arquivo(char *NomeArquivo, char *Registro);
void mtdFinalizacao(int NSinal);

bool mtdLerEntradaGPIO(int Pino);
void mtdLerEntradaLoopGPIO(int Pino, int Iteracoes, double Intervalo);
void mtdImprimirSaidaGPIO(int Pino, int Valor, double Intervalo);
void mtdImprimirSaidaLoopGPIO(int Pino, int Iteracoes, double Intervalo);

void mtdSair();
void mtdPadrao();

bool modoLog = true;
int arquivo = 0;
int porta = 0;
int portaEntrada = 0;
int portaSaida = 0;
int valor = LOW;
int iteracoes = 0;
float intervalo = Intervalo_Padrao;
char *nomeArquivoLog = "log.txt";
char buffer[3];
char path[TAMANHO_STRING_CHAR];

bool mtdAcessarPortaPinoGPIO(int Pino)
{
	bool Retorno = false;

	snprintf(path, TAMANHO_STRING_CHAR, "/sys/class/gpio/gpio%d/direction", Pino);

	if(access(path, 0) == -1)
	{
		Retorno = false;
	}
	else
	{
		Retorno = true;
	}

	return Retorno;
}

bool mtdExportarPortaPinoGPIO(int Pino)
{
	bool Retorno = false;
	char bsp[TAMANHO_STRING_CHAR];

	snprintf(bsp, TAMANHO_STRING_CHAR, "mtdExportarPortaPinoGPIO - %d - Arquivo foi aberto incorretamente.\n", Pino);
	
	arquivo = open("/sys/class/gpio/export", O_WRONLY);
	
	if(arquivo == -1)
	{
		mtdGerarLog_Tela(bsp);
		Retorno = false;
	}
	else
	{
		snprintf(buffer, 3,"%d", Pino);

		if(write(arquivo, buffer, 3) == -1)
		{
			close(arquivo);
			
			Retorno = false;
		}
		else
		{
			close(arquivo);
			
			Retorno = true;
		}
	}

	return Retorno;
}

bool mtdConfigurarDirecaoPortaPinoGPIO(int Pino, int Direcao)
{
	bool Retorno = false;

	snprintf(path, TAMANHO_STRING_CHAR, "/sys/class/gpio/gpio%d/direction", Pino);
	arquivo = open(path, O_WRONLY);

	if(arquivo == -1)
	{
		Retorno = false;
	}
	else
	{
		snprintf(buffer, 3, "%d", Pino);
		if(write(arquivo, ((Direcao == INPUT) ? "in" : "out"), 3) == -1)
		{
			close(arquivo);
			
			Retorno = false;
		}
		else
		{
			close(arquivo);
			
			Retorno = true;
		}
	}

	return Retorno;
}

int mtdConfigurarPortaPinoValorEntradaGPIO(int Pino)
{
	int Retorno = false;
	char vetRetorno[3];

	snprintf(path, TAMANHO_STRING_CHAR, "/sys/class/gpio/gpio%d/value", Pino);
	arquivo = open(path, O_RDONLY);

	if(arquivo == -1)
	{
		Retorno = false;
	}
	else
	{
		if (read(arquivo, vetRetorno, 3) == -1)
		{
			close(arquivo);
			
			Retorno = false;
		}
		else
		{
			close(arquivo);
			
			Retorno = atoi(vetRetorno);
		}
	}

	return Retorno;
}

bool mtdConfigurarPortaPinoValorSaidaGPIO(int Pino, int Valor)
{
	bool Retorno = false;

	snprintf(path, TAMANHO_STRING_CHAR, "/sys/class/gpio/gpio%d/value", Pino);
	arquivo = open(path, O_WRONLY);

	if(arquivo == -1)
	{
		Retorno = false;
	}
	else
	{
		if(write(arquivo, ((Valor == HIGH) ? "1" : "0"), 1) == -1)
		{
			close(arquivo);
			
			Retorno = false;
		}
		else
		{
			close(arquivo);
			
			Retorno = true;
		}
	}

	return Retorno;
}

bool mtdDesExportarPortaPinoGPIO(int Pino)
{
	bool Retorno = false;
	char bsp[TAMANHO_STRING_CHAR];

	snprintf(bsp, TAMANHO_STRING_CHAR, "mtdDesExportarPortaPinoGPIO - %d - Arquivo foi aberto incorretamente.\n", Pino);
	
	arquivo = open("/sys/class/gpio/unexport", O_WRONLY);
	
	if(arquivo == -1)
	{
		mtdGerarLog_Tela(bsp);
		Retorno = false;
	}
	else
	{
		
		snprintf(buffer, 3,"%d", Pino);

		if(write(arquivo, buffer, 3) == -1)
		{
			close(arquivo);
			
			Retorno = false;
		}
		else
		{
			close(arquivo);
			
			Retorno = true;
		}
	}

	return Retorno;
}

void mtdFinalizacao(int NSinal)
{
	mtdGerarLog_Tela("mtdFinalizacao - Desvinculando o pino.\n");

	if(mtdDesExportarPortaPinoGPIO(porta))
	{
		delay(intervalo);
		mtdGerarLog_Tela("mtdFinalizacao - Desexportando o pino.\n");
		delay(intervalo);
		mtdGerarLog_Tela("mtdFinalizacao - Programa finalizado.\n");
		exit(0);
	}
}

void mtdGerarLog_Tela(char *Registro)
{
	struct tm *tmTempoAtual;

	time_t tmTempoAtualSegundos;

	time(&tmTempoAtualSegundos);

	tmTempoAtual = localtime(&tmTempoAtualSegundos);

	printf
	(
		"%0*d/%0*d/%0*d-%0*d:%0*d:%0*d: %s",
		2, tmTempoAtual->tm_mday,
		2, tmTempoAtual->tm_mon,
		4, (tmTempoAtual->tm_year + 1900),
		2, tmTempoAtual->tm_hour,
		2, tmTempoAtual->tm_min,
		2, tmTempoAtual->tm_sec,
		Registro
	);
}

void mtdAcrescentarLog_Arquivo(char *NomeArquivo, char *Registro)
{
	struct tm *tmTempoAtual;

	time_t tmTempoAtualSegundos;

	time(&tmTempoAtualSegundos);

	tmTempoAtual = localtime(&tmTempoAtualSegundos);

	FILE *cfPtr;

	cfPtr=fopen(NomeArquivo, "a");
	fprintf
	(
		cfPtr,
		"%0*d/%0*d/%0*d-%0*d:%0*d:%0*d: %s",
		2, tmTempoAtual->tm_mday,
		2, tmTempoAtual->tm_mon,
		4, (tmTempoAtual->tm_year + 1900),
		2, tmTempoAtual->tm_hour,
		2, tmTempoAtual->tm_min,
		2, tmTempoAtual->tm_sec,
		Registro
	);

	fclose(cfPtr);
}

void mtdEscreverLog_Arquivo(char *NomeArquivo, char *Registro)
{
	struct tm *tmTempoAtual;

	time_t tmTempoAtualSegundos;

	time(&tmTempoAtualSegundos);

	tmTempoAtual = localtime(&tmTempoAtualSegundos);

	FILE *cfPtr;

	cfPtr=fopen(NomeArquivo, "w");
	fprintf
	(
		cfPtr,
		"%0*d/%0*d/%0*d-%0*d:%0*d:%0*d: %s",
		2, tmTempoAtual->tm_mday,
		2, tmTempoAtual->tm_mon,
		4, (tmTempoAtual->tm_year + 1900),
		2, tmTempoAtual->tm_hour,
		2, tmTempoAtual->tm_min,
		2, tmTempoAtual->tm_sec,
		Registro
	);

	fclose(cfPtr);
}

void delay(double time)
{
	struct timespec t;
	int seg = time;
	t.tv_sec = seg;
	t.tv_nsec = (time - seg) * 1e9;
	nanosleep(&t, NULL);
}

bool mtdLerEntradaGPIO(int Pino)
{
	bool Retorno = LOW;
	char bspl[TAMANHO_STRING_CHAR];
	char bsph[TAMANHO_STRING_CHAR];

	snprintf(bspl, TAMANHO_STRING_CHAR, "mtdLerEntradaGPIO - %d - LOW.\n", Pino);
	snprintf(bsph, TAMANHO_STRING_CHAR, "mtdLerEntradaGPIO - %d - HIGH.\n", Pino);

	porta = Pino;
	signal(SIGINT, mtdFinalizacao);
	
	if(!mtdAcessarPortaPinoGPIO(Pino))
	{
		mtdExportarPortaPinoGPIO(Pino);
	}
	
	if(mtdConfigurarDirecaoPortaPinoGPIO(Pino, INPUT))
	{
		Retorno = mtdConfigurarPortaPinoValorEntradaGPIO(Pino);

		switch(Retorno)
		{
			case LOW:
				mtdGerarLog_Tela(bspl);

				break;
			case HIGH:
				mtdGerarLog_Tela(bsph);

				break;
		}
	}
	
	if(mtdAcessarPortaPinoGPIO(Pino))
	{
		mtdDesExportarPortaPinoGPIO(Pino);
	}
	
	return Retorno;
}

void mtdLerEntradaLoopGPIO(int Pino, int Iteracoes, double Intervalo)
{
	char bspl[TAMANHO_STRING_CHAR];
	char bsph[TAMANHO_STRING_CHAR];

	snprintf(bspl, TAMANHO_STRING_CHAR, "mtdLerEntradaLoopGPIO - %d - LOW.\n", Pino);
	snprintf(bsph, TAMANHO_STRING_CHAR, "mtdLerEntradaLoopGPIO - %d - HIGH.\n", Pino);
	
	porta = Pino;
	signal(SIGINT, mtdFinalizacao);

	int contador = 0;
	int iteracoesRestantes = 0;

	if(!mtdAcessarPortaPinoGPIO(Pino))
	{
		mtdExportarPortaPinoGPIO(Pino);
	}

	if(mtdConfigurarDirecaoPortaPinoGPIO(Pino, INPUT))
	{
		while(Iteracoes == 0 || contador++ < Iteracoes)
		{
			iteracoesRestantes = (Iteracoes - contador + 1);

			printf("Iteracoes restantes: %d.\n", iteracoesRestantes);

			switch(mtdConfigurarPortaPinoValorEntradaGPIO(Pino))
			{
				case LOW:
					mtdGerarLog_Tela(bspl);

					break;
				case HIGH:
					mtdGerarLog_Tela(bsph);

					break;
			}

			delay(Intervalo);
		}
	}

	if(mtdAcessarPortaPinoGPIO(Pino))
	{
		mtdDesExportarPortaPinoGPIO(Pino);
	}
}

void mtdImprimirSaidaGPIO(int Pino, int Valor, double Intervalo)
{
	char bspl[TAMANHO_STRING_CHAR];
	char bsph[TAMANHO_STRING_CHAR];

	snprintf(bspl, TAMANHO_STRING_CHAR, "mtdImprimirSaidaGPIO - %d - LOW.\n", Pino);
	snprintf(bsph, TAMANHO_STRING_CHAR, "mtdImprimirSaidaGPIO - %d - HIGH.\n", Pino);

	porta = Pino;
	signal(SIGINT, mtdFinalizacao);

	if(!mtdAcessarPortaPinoGPIO(Pino))
	{
		mtdExportarPortaPinoGPIO(Pino);
	}
	
	if(mtdConfigurarDirecaoPortaPinoGPIO(Pino, OUTPUT))
	{
		mtdConfigurarPortaPinoValorSaidaGPIO(Pino, Valor);
		
		switch(Valor)
		{
			case LOW:
				mtdGerarLog_Tela(bspl);

				break;
			case HIGH:
				mtdGerarLog_Tela(bsph);

				break;
		}
		
		if(Intervalo > -1)
		{
			delay(Intervalo);
		}
		else
		{
			int c = getchar();
		}
	}

	if(mtdAcessarPortaPinoGPIO(Pino))
	{
		mtdDesExportarPortaPinoGPIO(Pino);
	}
}

void mtdImprimirSaidaLoopGPIO(int Pino, int Iteracoes, double Intervalo)
{
	char bspl[TAMANHO_STRING_CHAR];
	char bsph[TAMANHO_STRING_CHAR];

	snprintf(bspl, TAMANHO_STRING_CHAR, "mtdImprimirSaidaLoopGPIO - %d - LOW.\n", Pino);
	snprintf(bsph, TAMANHO_STRING_CHAR, "mtdImprimirSaidaLoopGPIO - %d - HIGH.\n", Pino);

	porta = Pino;
	signal(SIGINT, mtdFinalizacao);

	int contador = 0;
	int iteracoesRestantes = 0;
	
	char bspi[TAMANHO_STRING_CHAR];

	if(!mtdAcessarPortaPinoGPIO(Pino))
	{
		mtdExportarPortaPinoGPIO(Pino);
	}

	if(mtdConfigurarDirecaoPortaPinoGPIO(Pino, OUTPUT))
	{
		while(Iteracoes == 0 || contador++ < Iteracoes)
		{
			iteracoesRestantes = (Iteracoes - contador + 1);
			
			snprintf(bspi, TAMANHO_STRING_CHAR, "Iteracoes restantes: %d.\n", iteracoesRestantes);
			mtdGerarLog_Tela(bspi);

			mtdConfigurarPortaPinoValorSaidaGPIO(Pino, HIGH);
			mtdGerarLog_Tela(bspl);

			delay(Intervalo / 2.0);

			mtdConfigurarPortaPinoValorSaidaGPIO(Pino, LOW);
			mtdGerarLog_Tela(bsph);

			delay(Intervalo / 2.0);
		}
	}
	
	if(mtdAcessarPortaPinoGPIO(Pino))
	{
		mtdDesExportarPortaPinoGPIO(Pino);
	}
}

void mtdLerEntradaLoopImprimirSaidaLoopGPIO(int PinoEntrada, int PinoSaida, int Iteracoes, double Intervalo)
{
	int contador = 0;
	int iteracoesRestantes = 0;

	char bspi[TAMANHO_STRING_CHAR];

	while(Iteracoes == 0 || contador++ < Iteracoes)
	{
		iteracoesRestantes = (Iteracoes - contador + 1);

		snprintf(bspi, TAMANHO_STRING_CHAR, "Iteracoes restantes: %d.\n", iteracoesRestantes);
		mtdGerarLog_Tela(bspi);
		mtdImprimirSaidaGPIO(PinoSaida, mtdLerEntradaGPIO(PinoEntrada), Intervalo);
	}
}

int mtdConverterCharToCodeIntegerASCII(char c)
{
	return  c;
}

void mtdPadrao()
{
	int opcao = 0;

	do
	{
		system("cls || clear");
		
		printf("Menu GPIO\n");
		printf("0. Executar Entrada GPIO\n");
		printf("1. Executar Entrada Loop GPIO\n");
		printf("2. Executar Saida GPIO\n");
		printf("3. Executar Saida Loop GPIO\n");
		printf("5. Sair.\n");
		printf("Obs.: Aperte as teclas Ctrl+C para cancelar uma acao.\n");
		printf("\nEscolha uma opcao: ");

		scanf("%d", &opcao);
		system("cls || clear");

		switch(opcao)
		{
			case 0:
				printf("Configurar Numero da Porta: ");
				scanf("%d", &porta);
				mtdLerEntradaGPIO(porta);

				break;
			case 1:
				printf("Configurar Numero da Porta: ");
				scanf("%d", &porta);
				printf("Configurar Numero de Iteracoes (0 - ilimitado): ");
				scanf("%d", &iteracoes);
				printf("Configurar Intervalo de Tempo [segundos]: ");
				scanf("%f", &intervalo);
				mtdLerEntradaLoopGPIO(porta, iteracoes, intervalo);

				break;
			case 2:
				printf("Configurar Numero da Porta: ");
				scanf("%d", &porta);
				printf("Configurar Valor da Porta: ");
				scanf("%d", &valor);
				printf("Configurar Intervalo de Tempo [segundos]: ");
				scanf("%f", &intervalo);
				mtdImprimirSaidaGPIO(porta, valor, intervalo);

				break;
			case 3:
				printf("Configurar Numero da Porta: ");
				scanf("%d", &porta);
				printf("Configurar Numero de Iteracoes (0 - ilimitado): ");
				scanf("%d", &iteracoes);
				printf("Configurar Intervalo de Tempo [segundos]: ");
				scanf("%f", &intervalo);
				mtdImprimirSaidaLoopGPIO(porta, iteracoes, intervalo);

				break;
			case 4:
				printf("Configurar Numero da Porta de Entrada: ");
				scanf("%d", &portaEntrada);
				printf("Configurar Numero da Porta de Saida: ");
				scanf("%d", &portaSaida);
				printf("Configurar Numero de Iteracoes (0 - ilimitado): ");
				scanf("%d", &iteracoes);
				printf("Configurar Intervalo de Tempo [segundos]: ");
				scanf("%f", &intervalo);
				mtdLerEntradaLoopImprimirSaidaLoopGPIO(portaEntrada, portaSaida, iteracoes, intervalo);

				break;
			case 5:
				mtdSair();

				break;
			default:
				printf("Digite uma opcao valida.\n");

				break;
		}

		// system("pause");
		int c = getchar();
		c = getchar();
	}
	while (opcao != 5);
}

void mtdSair(void)
{
	// system("exit");
	exit(1);
}

int main(int argc, char *argv[])
{
	int Retorno = 0;
	
	printf("Aqui!!! (%d).\n", argc);

	// mtdEscreverLog(nomeArquivoLog, "Entrando na funcao main().\n");

	switch (argc)
	{
		case 2:
			mtdLerEntradaGPIO(atoi(argv[1]));

		break;
		case 3:
			switch (mtdConverterCharToCodeIntegerASCII(argv[1][0]))
			{
				case 97: // 'a'
					if(mtdAcessarPortaPinoGPIO(atoi(argv[2])))
					{
						mtdGerarLog_Tela("VERDADEIRO\n");
					}
					else
					{
						mtdGerarLog_Tela("FALSO\n");
					}

				break;
				case 101: // 'e'
					if(mtdExportarPortaPinoGPIO(atoi(argv[2])))
					{
						mtdGerarLog_Tela("VERDADEIRO\n");
					}
					else
					{
						mtdGerarLog_Tela("FALSO\n");
					}

				break;
				case 108: // 'l'
					mtdLerEntradaGPIO(atoi(argv[2]));

				break;
				case 120: // 'x'
					if(mtdDesExportarPortaPinoGPIO(atoi(argv[2])))
					{
						mtdGerarLog_Tela("VERDADEIRO\n");
					}
					else
					{
						mtdGerarLog_Tela("FALSO\n");
					}

				break;
				default:
					mtdImprimirSaidaGPIO(atoi(argv[2]), mtdLerEntradaGPIO(atoi(argv[1])), intervalo);

				break;
			}

		break;
		case 4:
			switch (mtdConverterCharToCodeIntegerASCII(argv[1][0]))
			{
				case 100: // 'd'
					if(mtdConfigurarDirecaoPortaPinoGPIO(atoi(argv[2]), atof(argv[3])))
					{
						mtdGerarLog_Tela("VERDADEIRO\n");
					}
					else
					{
						mtdGerarLog_Tela("FALSO\n");
					}

				break;
				default:
					mtdImprimirSaidaGPIO(atoi(argv[1]), atoi(argv[2]), atof(argv[3]));

				break;
			}

		break;
		case 5:
			switch (mtdConverterCharToCodeIntegerASCII(argv[1][0]))
			{
				case 105: // 'i'
					mtdImprimirSaidaGPIO(atoi(argv[2]), atoi(argv[3]), atof(argv[4]));

				break;
				default:
					switch(atoi(argv[2]))
					{
						case INPUT:
							mtdLerEntradaLoopGPIO(atoi(argv[1]), atoi(argv[3]), atof(argv[4]));

						break;
						case OUTPUT:
							mtdImprimirSaidaLoopGPIO(atoi(argv[1]), atoi(argv[3]), atof(argv[4]));

						break;
					}

				break;
			}

		break;
		case 6:
			switch (mtdConverterCharToCodeIntegerASCII(argv[1][0]))
			{
				default:
					switch (atoi(argv[2]))
					{
						case 2:
							mtdLerEntradaLoopImprimirSaidaLoopGPIO(atoi(argv[1]), atoi(argv[3]), atoi(argv[4]), atof(argv[5]));

						break;
					}
				break;
			}

		break;
		default:
			mtdPadrao();
		break;
	}

	// mtdEscreverLog(nomeArquivoLog, "Saindo da funcao main().\n");

	return Retorno;
}
