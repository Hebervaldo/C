#include "i2clcd.h"
#include "getipv4.h"
#include <limits.h>
#include <softPwm.h>
#include <time.h>

#define TAMANHO_STRING_CHAR 200
#define Intervalo_Padrao 1.0

char bspd[TAMANHO_STRING_CHAR];

int contador = 0;

int porta = 0;
int iteracoes = 0;
double intervalo = Intervalo_Padrao;
int valor = LOW;

int portaEntrada = 0;
int portaSaida = 0;

bool mtdConfigurarDirecaoPortaPinoGPIO(int Pino, int Direcao);
bool mtdConfigurarDirecaoPortaPinoPWMGPIO(int Pino, int Modo, int Frequencia, int Alcance);
int mtdLerEntradaGPIO(int Pino, double Intervalo);
void mtdImprimirSaidaGPIO(int Pino, int Valor, double Intervalo);
void mtdImprimirSaidaPWMGPIO(int Pino, int Valor, double Intervalo);
void mtdCodificarMensagemCodigoMorse(int Porta, int I2c_addr, char *Mensagem, int NumeroCaracteresLinha, int Repeticoes, double Intervalo_Sinal);
void mtdFinalizacao(int NSinal);
int mtdObterComprimentoVetorChar(char *VetorChar);
void mtdObterVetorCharTextoAlinhadoEstatico(char *Texto, int NumeroCaracteresLinha, char Alinhamento, char *Retorno);
void mtdObterVetorCharTextoAlinhadoDinamico(char *Texto, int NumeroCaracteresLinha, char Direcao, int Passo, char *Retorno);
void mtdObterVetorCharTexto(char *Texto, int NumeroCaracteresLinha, char Alinhamento, char Direcao, int Passo, char *Retorno);
void mtdGerarDelay(double Intervalo);
bool mtdConfigurarDirecaoPortaPinoLCDGPIO(int I2c_addr);
void mtdImprimirSaidaLCDGPIO(int I2c_addr, char *Linha01, char *Linha02, int NumeroCaracteresLinha, char Alinhamento, char Direcao, int Repeticoes, double Intervalo);
void mtdImprimirTempoLCDGPIO(int I2c_addr, int NumeroCaracteresLinha, int Repeticoes, double Intervalo);
void mtdImprimirNomeIPInterfaceLCDGPIO(int I2c_addr, int NumeroCaracteresLinha, int Repeticoes, double Intervalo);
void mtdGerarLog_Tela(char *Registro);
void mtdPadrao();
void mtdSair();

int mtdConverterCharToCodeIntegerASCII(char c);

bool mtdConfigurarDirecaoPortaPinoGPIO(int Pino, int Direcao)
{
	bool Retorno = false;

	if(wiringPiSetupGpio() != -1)
	{
		pinMode(Pino, Direcao);

		Retorno = true;
	}
	else
	{
		Retorno = false;
	}

	return Retorno;
}

bool mtdConfigurarDirecaoPortaPinoPWMGPIO(int Pino, int Modo, int Frequencia, int Alcance)
{
	bool Retorno = false;
	
	Retorno = mtdConfigurarDirecaoPortaPinoGPIO(Pino, OUTPUT);

	if(Retorno)
	{
		softPwmCreate(Pino, 1, 100);
		pwmSetMode(Modo);
		pwmSetClock(Frequencia);
		pwmSetRange(Alcance);
	}

	return Retorno;
}

int mtdLerEntradaGPIO(int Pino, double Intervalo)
{
	int Retorno = -1;
	
	char bspl[TAMANHO_STRING_CHAR];
	char bsph[TAMANHO_STRING_CHAR];

	snprintf(bspl, TAMANHO_STRING_CHAR, "mtdLerEntradaGPIO - %d - LOW.\n", Pino);
	snprintf(bsph, TAMANHO_STRING_CHAR, "mtdLerEntradaGPIO - %d - HIGH.\n", Pino);

	porta = Pino;

	if(mtdConfigurarDirecaoPortaPinoGPIO(Pino, INPUT))
	{
		Retorno = digitalRead(Pino);
		
		switch(digitalRead(Pino))
		{
			case LOW:
				mtdGerarLog_Tela(bspl);

				break;
			case HIGH:
				mtdGerarLog_Tela(bsph);

				break;
		}
	
		mtdGerarDelay(Intervalo);	
	}
	
	return Retorno;
}

void mtdImprimirSaidaGPIO(int Pino, int Valor, double Intervalo)
{
	char bspl[TAMANHO_STRING_CHAR];
	char bsph[TAMANHO_STRING_CHAR];

	snprintf(bspl, TAMANHO_STRING_CHAR, "mtdImprimirSaidaGPIO - %d - LOW.\n", Pino);
	snprintf(bsph, TAMANHO_STRING_CHAR, "mtdImprimirSaidaGPIO - %d - HIGH.\n", Pino);

	porta = Pino;

	if(mtdConfigurarDirecaoPortaPinoGPIO(Pino, OUTPUT))
	{
		digitalWrite(Pino, Valor);

		switch(Valor)
		{
			case LOW:
				mtdGerarLog_Tela(bspl);

				break;
			case HIGH:
				mtdGerarLog_Tela(bsph);

				break;
		}
	
		mtdGerarDelay(Intervalo);	
	}
}

void mtdImprimirSaidaPWMGPIO(int Pino, int Valor, double Intervalo)
{
	char bsp[TAMANHO_STRING_CHAR];

	porta = Pino;

	if(mtdConfigurarDirecaoPortaPinoPWMGPIO(Pino, PWM_MODE_MS, 2400, 8))
	{
		softPwmWrite(Pino, Valor);

		snprintf(bsp, TAMANHO_STRING_CHAR, "mtdImprimirSaidaPWMGPIO - %d - %d.\n", Pino, Valor);
		mtdGerarLog_Tela(bsp);

		mtdGerarDelay(Intervalo);
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

int mtdConverterCharToCodeIntegerASCII(char c)
{
	return c;
}   

void mtdPadrao()
{
	int opcao = 0;

	do
	{
		system("cls || clear");
		
		printf("Menu GPIO\n");
		// printf("0. Executar Entrada GPIO\n");
		// printf("1. Executar Entrada Loop GPIO\n");
		// printf("2. Executar Saida GPIO\n");
		// printf("3. Executar Saida Loop GPIO\n");
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
				// mtdLerEntradaGPIO(porta);

				break;
			case 1:
				printf("Configurar Numero da Porta: ");
				scanf("%d", &porta);
				printf("Configurar Numero de Iteracoes (0 - ilimitado): ");
				scanf("%d", &iteracoes);
				printf("Configurar Intervalo de Tempo [segundos]: ");
				scanf("%f", &intervalo);
				// mtdLerEntradaLoopGPIO(porta, iteracoes, intervalo);

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
				// mtdImprimirSaidaLoopGPIO(porta, iteracoes, intervalo);

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
				// mtdLerEntradaLoopImprimirSaidaLoopGPIO(portaEntrada, portaSaida, iteracoes, intervalo);
 
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

char *mtdCodificarCaractereCodigoMorse(char Caractere)
{
    // refer to the Morse table
    // image attached in the article
    switch (Caractere)
    {
		case 'A':
		case 'a':
			return ".-";
		case 'B':
		case 'b':
			return "-...";
		case 'C':
		case 'c':
			return "-.-.";
		case 'D':
		case 'd':
			return "-..";
		case 'E':
		case 'e':
			return ".";
		case 'F':
		case 'f':
			return "..-.";
		case 'G':
		case 'g':
			return "--.";
		case 'H':
		case 'h':
			return "....";
		case 'I':
		case 'i':
			return "..";
		case 'J':
		case 'j':
			return ".---";
		case 'K':
		case 'k':
			return "-.-";
		case 'L':
		case 'l':
			return ".-..";
		case 'M':
		case 'm':
			return "--";
		case 'N':
		case 'n':
			return "-.";
		case 'O':
		case 'o':
			return "---";
		case 'P':
		case 'p':
			return ".--.";
		case 'Q':
		case 'q':
			return "--.-";
		case 'R':
		case 'r':
			return ".-.";
		case 'S':
		case 's':
			return "...";
		case 'T':
		case 't':
			return "-";
		case 'U':
		case 'u':
			return "..-";
		case 'V':
		case 'v':
			return "...-";
		case 'W':
		case 'w':
			return ".--";
		case 'X':
		case 'x':
			return "-..-";
		case 'Y':
		case 'y':
			return "-.--";
		case 'Z':
		case 'z':
			return "--..";
		case '1':
			return ".----";
		case '2':
			return "..---";
		case '3':
			return "...--";
		case '4':
			return "....-";
		case '5':
			return ".....";
		case '6':
			return "-....";
		case '7':
			return "--...";
		case '8':
			return "---..";
		case '9':
			return "----.";
		case '0':
			return "-----";
		default:
			return "";
    }
}

void mtdCodificarMensagemCodigoMorse(int Porta, int I2c_addr, char *Mensagem, int NumeroCaracteresLinha, int Repeticoes, double Intervalo_Sinal)
{
	// character by character print
	// Morse code

   	char bspl[5][TAMANHO_STRING_CHAR];
   	char bspldl[2][TAMANHO_STRING_CHAR];

   	char bspm[TAMANHO_STRING_CHAR];
	snprintf(bspm, TAMANHO_STRING_CHAR, "%s", Mensagem);

   	char bspc[TAMANHO_STRING_CHAR];
   	
	int intContador = 0;
	
	while(Repeticoes > -1 ? intContador < Repeticoes : 1)
	{
		for (int i = 0; i < mtdObterComprimentoVetorChar(bspm); i++)
		{
			snprintf(bspc, TAMANHO_STRING_CHAR, "%s\0", mtdCodificarCaractereCodigoMorse(Mensagem[i]));

			for (int j = 0; j < mtdObterComprimentoVetorChar(bspc); j++)
			{
				snprintf(bspl[0], TAMANHO_STRING_CHAR, "Contador: %d\n", contador);
				snprintf(bspl[1], TAMANHO_STRING_CHAR, "Mensagem: %s\n", bspm);
				snprintf(bspl[2], TAMANHO_STRING_CHAR, "Caractere: %c\n", bspm[i]);
				snprintf(bspl[3], TAMANHO_STRING_CHAR, "Codigo Morse do Caractere: %s\n", bspc);
				snprintf(bspl[4], TAMANHO_STRING_CHAR, "Sinal Morse do Caractere: %c\n", bspc[j]);

				snprintf(bspldl[0], TAMANHO_STRING_CHAR, "%s", bspm);
				snprintf(bspldl[1], TAMANHO_STRING_CHAR, "%c: %s -> %c", bspm[i], bspc, bspc[j]);


				mtdGerarLog_Tela(bspl[0]);
				mtdGerarLog_Tela(bspl[1]);
				mtdGerarLog_Tela(bspl[2]);
				mtdGerarLog_Tela(bspl[3]);
				mtdGerarLog_Tela(bspl[4]);
				
				mtdImprimirSaidaLCDGPIO(I2c_addr, bspldl[0], bspldl[1], 16, 'c', 'a', 1, (Intervalo_Sinal / 100));
				
				switch(bspc[j])
				{
					case '.':
						mtdImprimirSaidaGPIO(Porta, 0, (2 * Intervalo_Sinal / 5));
						mtdImprimirSaidaGPIO(Porta, 1, (1 * Intervalo_Sinal / 5));
						mtdImprimirSaidaGPIO(Porta, 0, (2 * Intervalo_Sinal / 5));

						break;
					case '-':
						mtdImprimirSaidaGPIO(Porta, 1, (Intervalo_Sinal));

						break;
				}
				
				mtdImprimirSaidaGPIO(Porta, 0, (Intervalo_Sinal / 5));
			}

			mtdImprimirSaidaGPIO(Porta, 0, (Intervalo_Sinal));
		}
		
		intContador++;
	}
}

void mtdFinalizacao(int NSinal)
{
	ClrLcd();

	exit(0);
}

int mtdObterComprimentoVetorChar(char *VetorChar)
{
	int Retorno = 0;

	while(*VetorChar++)
	{
		Retorno++;
	}

	return Retorno;
}

void mtdObterVetorCharTextoAlinhadoEstatico(char *Texto, int NumeroCaracteresLinha, char Alinhamento, char *Retorno)
{
	int intNumeroCaracteresTexto = 0;
	intNumeroCaracteresTexto = mtdObterComprimentoVetorChar(Texto);

	int intNumeroEspaco = 0;

	char bspei[TAMANHO_STRING_CHAR];
	char bspef[TAMANHO_STRING_CHAR];

	for(int i = 0; i < TAMANHO_STRING_CHAR; i++)
	{
		Retorno[i] = '\0';
		bspei[i] = '\0';
		bspef[i] = '\0';
	}

	int intContador = 0;

	if(intNumeroCaracteresTexto < NumeroCaracteresLinha)
	{
		switch(Alinhamento)
		{
			case 101: // 'e'
				intNumeroEspaco = -1;

			break;
			case 99: // 'c'
				intNumeroEspaco = round((NumeroCaracteresLinha - intNumeroCaracteresTexto) / 2);

			break;
			case 100: // 'd'
				intNumeroEspaco = round(NumeroCaracteresLinha - intNumeroCaracteresTexto);

			break;
			default:
				intNumeroEspaco = - 1;

			break;
		}

		for(int i = 0; i < intNumeroEspaco; i++)
		{
			bspei[i] = ' ';
		}

		for(int i = 0; i < NumeroCaracteresLinha - (intNumeroCaracteresTexto + intNumeroEspaco); i++)
		{
			bspef[i] = ' ';
		}
	}

	snprintf(Retorno, TAMANHO_STRING_CHAR, "%s%s%s", bspei, Texto, bspef);
}

void mtdObterVetorCharTextoAlinhadoDinamico(char *Texto, int NumeroCaracteresLinha, char Direcao, int Passo, char *Retorno) 
{
	int intNumeroCaracteresTexto = 0;
	intNumeroCaracteresTexto = mtdObterComprimentoVetorChar(Texto);

	int intContador = 0;
	int intIncremento = -1;
	int intPasso = (Passo % intNumeroCaracteresTexto);

	for(int i = 0; i < TAMANHO_STRING_CHAR; i++)
	{
		Retorno[i] = (i <= NumeroCaracteresLinha ? ' ' : '\0');
	}

	switch(Direcao)
	{
		case 97: // a
			intIncremento = 0;

			for(int i = 0; i < NumeroCaracteresLinha; i++)
			{
				intContador = (i + intPasso);

				if(intContador >= intNumeroCaracteresTexto)
				{
					intContador = intIncremento++;
				}

				Retorno[i] = Texto[intContador];
			}

		break;
		case 114: // r
			intIncremento = intNumeroCaracteresTexto;

			for(int i = 0; i < NumeroCaracteresLinha; i++)
			{
				intContador = (i - intPasso);

				if(intContador < 0)
				{
					intContador = intIncremento + (i - intPasso);
				}

				Retorno[i] = Texto[intContador];
			}    

		break;
		default:

		break;
	}
}

void mtdObterVetorCharTexto(char *Texto, int NumeroCaracteresLinha, char Alinhamento, char Direcao, int Passo, char *Retorno)
{
	int intNumeroCaracteresTexto = 0;
	intNumeroCaracteresTexto = mtdObterComprimentoVetorChar(Texto);

	int intIncremento = -1;
	int intPasso = (Passo % intNumeroCaracteresTexto);

	if(intNumeroCaracteresTexto <= NumeroCaracteresLinha)
	{
		mtdObterVetorCharTextoAlinhadoEstatico(Texto, NumeroCaracteresLinha, Alinhamento, Retorno);
	}
	else
	{
		mtdObterVetorCharTextoAlinhadoDinamico(Texto, NumeroCaracteresLinha, Direcao, Passo, Retorno);
	}
}

void mtdTextoAcrescentarCaractere(char *Texto, char Caractere, int NumeroCaracteresLinha, char *Retorno)
{
	int intNumeroCaracteresTexto = 0;
	intNumeroCaracteresTexto = mtdObterComprimentoVetorChar(Texto);

	if(intNumeroCaracteresTexto > NumeroCaracteresLinha)
	{
		snprintf(Retorno, TAMANHO_STRING_CHAR, "%s%c", Texto, Caractere);
	}
	else
	{
		snprintf(Retorno, TAMANHO_STRING_CHAR, "%s", Texto);
	}
}

void mtdGerarDelay(double Intervalo)
{
	if(Intervalo > -1)
	{
		delay(Intervalo * 1000);
	}
	else
	{
		int c = getchar();
	}	
}

bool mtdConfigurarDirecaoPortaPinoLCDGPIO(int I2c_addr)
{
	bool Retorno = false;

	if(wiringPiSetupGpio() != -1)
	{
		setfd(wiringPiI2CSetup(I2c_addr));
		lcd_init(); // setup LCD

		Retorno = true;
	}
	else
	{
		Retorno = false;
	}

	return Retorno;
}

void mtdImprimirSaidaLCDGPIO(int I2c_addr, char *Linha01, char *Linha02, int NumeroCaracteresLinha, char Alinhamento, char Direcao, int Repeticoes, double Intervalo)
{
	char bspl[2][TAMANHO_STRING_CHAR];
	char Saida[2][TAMANHO_STRING_CHAR];
	
	int intContador = 0;
	
	if(mtdConfigurarDirecaoPortaPinoLCDGPIO(I2c_addr))
	{
		switch(Direcao)
		{
			case 97: // a
				mtdTextoAcrescentarCaractere(Linha01, '<', NumeroCaracteresLinha, bspl[0]);
				mtdTextoAcrescentarCaractere(Linha02, '<', NumeroCaracteresLinha, bspl[1]);

			break;
			case 114: // r
				mtdTextoAcrescentarCaractere(Linha01, '>', NumeroCaracteresLinha, bspl[0]);
				mtdTextoAcrescentarCaractere(Linha02, '>', NumeroCaracteresLinha, bspl[1]);

			break;
		}

		while(Repeticoes > -1 ? intContador++ < Repeticoes : 1)
		{
			// ClrLcd();

			lcdLoc(LINE1);
			mtdObterVetorCharTexto(bspl[0], NumeroCaracteresLinha, Alinhamento, Direcao, contador, Saida[0]);
			typeln(Saida[0]);
			lcdLoc(LINE2);
			mtdObterVetorCharTexto(bspl[1], NumeroCaracteresLinha, Alinhamento, Direcao, contador, Saida[1]);
			typeln(Saida[1]);

			contador = contador < INT_MAX ? contador + 1 : 0;

			mtdGerarDelay(Intervalo);
		}
	}
}

void mtdImprimirTempoLCDGPIO(int I2c_addr, int NumeroCaracteresLinha, int Repeticoes, double Intervalo)
{
	char bspl[2][TAMANHO_STRING_CHAR];
	char Saida[2][TAMANHO_STRING_CHAR];

	int intContador = 0;

	time_t t;
	struct tm tmTempoAtual;

	if(mtdConfigurarDirecaoPortaPinoLCDGPIO(I2c_addr))
	{
		while(Repeticoes > -1 ? intContador++ < Repeticoes : 1)
		{
			t = time(NULL);

			tmTempoAtual = *localtime(&t);

			snprintf(bspl[0], TAMANHO_STRING_CHAR, "%02d/%02d/%0004d", tmTempoAtual.tm_mday, tmTempoAtual.tm_mon + 1, 1900 + tmTempoAtual.tm_year);
			snprintf(bspl[1], TAMANHO_STRING_CHAR, "%02d:%02d:%02d", tmTempoAtual.tm_hour, tmTempoAtual.tm_min, tmTempoAtual.tm_sec);

			lcdLoc(LINE1);
			mtdObterVetorCharTexto(bspl[0], NumeroCaracteresLinha, 'c', 'a', contador, Saida[0]);
			typeln(Saida[0]);
			lcdLoc(LINE2);
			mtdObterVetorCharTexto(bspl[1], NumeroCaracteresLinha, 'c', 'a', contador, Saida[1]);
			typeln(Saida[1]);
			contador = contador < INT_MAX ? contador + 1 : 0;

			mtdGerarDelay(Intervalo);
		}
	}
}

void mtdImprimirTempoLCDGPIO_SemConfiguracaoDirecaoPortaPino(int NumeroCaracteresLinha, int Repeticoes, double Intervalo)
{
	char bspl[2][TAMANHO_STRING_CHAR];
	char Saida[2][TAMANHO_STRING_CHAR];

	int intContador = 0;

	time_t t;
	struct tm tmTempoAtual;

	while(Repeticoes > -1 ? intContador++ < Repeticoes : 1)
	{
		t = time(NULL);

		tmTempoAtual = *localtime(&t);

		snprintf(bspl[0], TAMANHO_STRING_CHAR, "%02d/%02d/%0004d", tmTempoAtual.tm_mday, tmTempoAtual.tm_mon + 1, 1900 + tmTempoAtual.tm_year);
		snprintf(bspl[1], TAMANHO_STRING_CHAR, "%02d:%02d:%02d", tmTempoAtual.tm_hour, tmTempoAtual.tm_min, tmTempoAtual.tm_sec);

		lcdLoc(LINE1);
		mtdObterVetorCharTexto(bspl[0], NumeroCaracteresLinha, 'c', 'a', contador, Saida[0]);
		typeln(Saida[0]);
		lcdLoc(LINE2);
		mtdObterVetorCharTexto(bspl[1], NumeroCaracteresLinha, 'c', 'a', contador, Saida[1]);
		typeln(Saida[1]);
		contador = contador < INT_MAX ? contador + 1 : 0;

		mtdGerarDelay(Intervalo);
	}
}

void mtdImprimirNomeIPInterfaceLCDGPIO(int I2c_addr, int NumeroCaracteresLinha, int Repeticoes, double Intervalo)
{
    char bspl[2][TAMANHO_STRING_CHAR];
	char Saida[2][TAMANHO_STRING_CHAR];

	int intContador = 0;
	int intNumeroInterface = 0;
	
	if(mtdConfigurarDirecaoPortaPinoLCDGPIO(I2c_addr))
	{
		while(Repeticoes > -1 ? intContador++ < Repeticoes : 1)
		{
			mtdGerarVetorEstruturaInformacoesInterfaceRede();
			intNumeroInterface = getNumeroInterface();

			if(intNumeroInterface > 0)
			{
				for(int i = 0; i < intNumeroInterface + 1; i++)
				{
					if(i < (intNumeroInterface))
					{
						snprintf(bspl[0], TAMANHO_STRING_CHAR, "%s", mtdObterNomeInterface(i));
						snprintf(bspl[1], TAMANHO_STRING_CHAR, "%s", mtdObterEnderecoIPInterface(i));
						mtdObterVetorCharTexto(bspl[0], NumeroCaracteresLinha, 'c', 'a', contador, Saida[0]);
						mtdObterVetorCharTexto(bspl[1], NumeroCaracteresLinha, 'c', 'a', contador, Saida[1]);

						lcdLoc(LINE1);
						typeln(Saida[0]);
						lcdLoc(LINE2);
						typeln(Saida[1]);
						contador = contador < INT_MAX ? contador + 1 : 0;

						mtdGerarDelay(Intervalo);
					}
					else
					{
						mtdImprimirTempoLCDGPIO_SemConfiguracaoDirecaoPortaPino(NumeroCaracteresLinha, 35, .1);
					}
				}
			}
			else
			{
				mtdImprimirTempoLCDGPIO_SemConfiguracaoDirecaoPortaPino(NumeroCaracteresLinha, 1, .1);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	int Retorno = 0;
	
	signal(SIGINT, mtdFinalizacao);

	if(argv[1][0] == '-')
	{
		switch (argc)
		{
			case 2:
				mtdLerEntradaGPIO(atoi(argv[2]), intervalo);

			break;
			case 3:
				switch (mtdConverterCharToCodeIntegerASCII(argv[1][1]))
				{
					case 108: // 'l'
						mtdLerEntradaGPIO(atoi(argv[2]), intervalo);

					break;
					default:
						mtdImprimirSaidaGPIO(atoi(argv[2]), mtdLerEntradaGPIO(atoi(argv[1]), intervalo), intervalo);

					break;
				}

			break;
			case 4:
				switch (mtdConverterCharToCodeIntegerASCII(argv[1][1]))
				{
					case 105: // 'i'
						mtdImprimirSaidaGPIO(atoi(argv[2]), atoi(argv[3]), intervalo);

					break;
					case 108: // 'l'
						mtdLerEntradaGPIO(atoi(argv[2]), atof(argv[3]));

					break;
					case 112: // 'p'
						mtdImprimirSaidaPWMGPIO(atoi(argv[2]), atoi(argv[3]), intervalo);

					break;
					case 114: // 'r'
						mtdImprimirNomeIPInterfaceLCDGPIO(atoi(argv[2]), atoi(argv[3]), -1, 10);
					break;
					case 116: // 't'
						mtdImprimirTempoLCDGPIO(atoi(argv[2]), atoi(argv[3]), -1, .1);
					break;
					default:
						mtdImprimirSaidaGPIO(atoi(argv[1]), atoi(argv[2]), atof(argv[3]));
					break;
				}

				break;
			case 5:
				switch (mtdConverterCharToCodeIntegerASCII(argv[1][1]))
				{
					case 105: // 'i'
						mtdImprimirSaidaGPIO(atoi(argv[2]), atoi(argv[3]), atof(argv[4]));

					break;
					case 112: // 'p'
						mtdImprimirSaidaPWMGPIO(atoi(argv[2]), atoi(argv[3]), atof(argv[4]));

					break;
					case 114: // 'r'
						mtdImprimirNomeIPInterfaceLCDGPIO(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), 10);
					break;
					case 116: // 't'
						mtdImprimirTempoLCDGPIO(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), .1);
					break;
					default:
						switch(atoi(argv[2]))
						{
							case INPUT:
								// mtdLerEntradaLoopGPIO(atoi(argv[1]), atoi(argv[3]), atof(argv[4]));

							break;
							case OUTPUT:
								// mtdImprimirSaidaLoopGPIO(atoi(argv[1]), atoi(argv[3]), atof(argv[4]));

							break;
							case PWM_OUTPUT: // '2'
								mtdImprimirSaidaPWMGPIO(atoi(argv[1]), atoi(argv[3]), atof(argv[4]));
							break;
						}

					break;
				}

				break;
			case 6:
				switch (mtdConverterCharToCodeIntegerASCII(argv[1][1]))
				{
					case 114: // 'r'
						mtdImprimirNomeIPInterfaceLCDGPIO(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atof(argv[5]));
					break;
					case 116: // 't'
						mtdImprimirTempoLCDGPIO(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atof(argv[5]));
					break;
					case 2:
						// mtdLerEntradaLoopImprimirSaidaLoopGPIO(atoi(argv[1]), atoi(argv[3]), atoi(argv[4]), atof(argv[5]));

					break;	
					case 100: // d
						// mtdImprimirSaidaLCDGPIO(atoi(argv[2]), argv[3], argv[4], 16, 'c', atof(argv[5]));

					break;
				}

				break;
			case 8:
				switch (mtdConverterCharToCodeIntegerASCII(argv[1][1]))
				{
					case 109: // 'm'
					mtdCodificarMensagemCodigoMorse(atoi(argv[2]), atoi(argv[3]), argv[4], atoi(argv[5]), atoi(argv[6]), atof(argv[7]));

					break;
				}

				break;
			case 10:
				switch (mtdConverterCharToCodeIntegerASCII(argv[1][1]))
				{
					case 100: // d
						mtdImprimirSaidaLCDGPIO
						(
							atoi(argv[2]), 
							argv[3], 
							argv[4], 
							atoi(argv[5]), 
							mtdConverterCharToCodeIntegerASCII(argv[6][0]), 
							mtdConverterCharToCodeIntegerASCII(argv[7][0]), 
							atoi(argv[8]), 
							atof(argv[9])
						);

					break;
				}

				break;
			default:
				mtdPadrao();

			break;
		}
	}

	return Retorno;
}
