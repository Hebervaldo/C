#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#define CONSTANTE_TIPO_NUMERO_DIGITOS "%.18Lf\n"
#define TIPO_OPERACAO_GENERICA "opr"
#define TIPO_OPERACAO_ADICAO "add"
#define TIPO_OPERACAO_SUBTRACAO "sub"
#define TIPO_OPERACAO_MULTIPLICACAO "mul"
#define TIPO_OPERACAO_DIVISAO "div"
#define TIPO_OPERACAO_POTENCIACAO "pot"

#define err_c 		1.0e-100
#define exp_c		2.718281828459045235360287471352662497757
#define ln10_c		2.302585092994045901093613792909309267997
#define invln10_c	.4342944819032517867168474018368939937317

int strContains(char* string, char* toFind)
{
    int slen = strlen(string);
    int tFlen = strlen(toFind);
    int found = 0;

    if(slen >= tFlen)
    {
        for(int s = 0, t = 0; s < slen; s++)
        {
            do
			{
                if(string[s] == toFind[t])
                {
                    if(++found == tFlen) 
					{
						return 1;
					}
                    s++;
                    t++;
                }
                else
				{
					s -= found;
					found = 0;
					t = 0;
				}

              }
			  while(found);
        }
        return 0;
    }
    else 
	{
		return -1;
	}
}

long double mtdObterValorAbsoluto(long double x)
{
	long double Retorno = 0;

	if (x >= 0)
	{
		Retorno = x;
	}
	else
	{
		Retorno = -1 * x;
	}

	return Retorno;
}

short int mtdObterSinal(long double x)
{
	return (short int) (x / mtdObterValorAbsoluto(x));
}

long double mtdCalcularExponencial(long double x)
{
	long double Retorno = 0;

	FILE *cfPtr;

	long double xn = (x * invln10_c);

	long double axn = mtdObterValorAbsoluto(xn);
	long double ixn = (long double)((int) xn);
	long double aixn = mtdObterValorAbsoluto(ixn);
	long double afxn = (axn - aixn);
	long double xe = afxn * ln10_c;

	int j = 1;
	long double oldr = 0;
	long double r = 1 + xe;
	long double div = 1;
	long double m = xe;
	long double err = r;

	if (x != 0)
	{
		while (err > err_c)
		{
			j = j + 1;
			div = div * j;
			m = m * xe;
			oldr = r;
			r = r + (m / div);
			err = mtdObterValorAbsoluto(r - oldr);
		}

		long double sixn = (ixn / aixn);
		long double r10n = 1;

		if (aixn > 0)
		{
			for(int i = 0; i <= (aixn - 1); i++)
			{
				r10n = r10n * 10;
			}
			
			if(axn >= 1)
			{
				r = r * r10n;
			}
			else
			{
				r = (r / r10n);
			}
		}

		if (x < 0)
		{
			r = (1 / r);
		}

		Retorno = r;
	}
	else
	{
		j = 0;
		err = 0;
		Retorno = 1;
	}
	
	return Retorno;
}

long double mtdCalcularLogaritmoNatural(long double x)
{
	long double Retorno = 0;

	FILE *cfPtr;

	int i = 0;
	int j = 1;
	int signal = 1;
    long double xn = x;
    long double axn = mtdObterValorAbsoluto(xn);
	long double xe = axn;
	long double r = xe;
	long double m = xe;
	long double oldr = 0;
	long double div = 1;
	long double exp = exp_c;
	long double err = r;

	if (x > 0)
	{
		if (x >= 1)
		{
			signal = 1;
		}
		else
		{
			xe = 1 / xe;
			signal = -1;
		}

		while (xe >= 1)
		{
			xe = xe / exp;
			i = i + 1;
		}

		xe = ((xe - 1) / (xe + 1));
		r = xe;
		m = xe;

		while (err > err_c)
		{
			m = m * (xe * xe);
			oldr = r;
			div = div + 2;
			r = r + (m / div);
			j = j + 1;
			err = mtdObterValorAbsoluto(r - oldr);
		}

		Retorno = (signal * (i + 2 * r));
	}
	else
	{
		j = 0;
		err = 0;
		Retorno = 0;
	}

	return Retorno;
}

long double mtdOperarAdicao(long double x, long double y)
{
	return x + y;
}

long double mtdOperarSubtracao(long double x, long double y)
{
	return x - y;
}

long double mtdOperarMultiplicacao(long double x, long double y)
{
	return x * y;
}

long double mtdOperarDivisao(long double x, long double y)
{
	return x / y;
}

long double mtdOperarPotenciacao(long double x, long double y)
{
	long double Retorno = 0;

	if (x >= 0)
	{
		Retorno = (mtdCalcularExponencial(y * mtdCalcularLogaritmoNatural(x)));
	}
	else
	{
		Retorno = 0;
	}

	return Retorno;
}

long double mtdOperarGenerico(char operador, long double x, long double y)
{
	long double Resultado = 0;

	switch (operador)
	{
		case '+':
			Resultado = mtdOperarAdicao(x, y);
		break;
		case '-':
			Resultado = mtdOperarSubtracao(x, y);
		break;
		case '*':
			Resultado = mtdOperarMultiplicacao(x, y);
		break;
		case '/':
			Resultado = mtdOperarDivisao(x, y);
		break;
		case '^':
			Resultado = mtdOperarPotenciacao(x, y);
		break;
	}

	return Resultado;
}

char *mtdDefinirOperacao(char *Entrada)
{
	char *Retorno;

	if (strContains(Entrada, TIPO_OPERACAO_GENERICA))
	{
		Retorno = TIPO_OPERACAO_GENERICA;
	}

	if (strContains(Entrada, TIPO_OPERACAO_ADICAO))
	{
		Retorno = TIPO_OPERACAO_ADICAO;
	}

	if (strContains(Entrada, TIPO_OPERACAO_SUBTRACAO))
	{
		Retorno = TIPO_OPERACAO_SUBTRACAO;
	}

	if (strContains(Entrada, TIPO_OPERACAO_MULTIPLICACAO))
	{
		Retorno = TIPO_OPERACAO_MULTIPLICACAO;
	}

	if (strContains(Entrada, TIPO_OPERACAO_DIVISAO))
	{
		Retorno = TIPO_OPERACAO_DIVISAO;
	}

	if (strContains(Entrada, TIPO_OPERACAO_POTENCIACAO))
	{
		Retorno = TIPO_OPERACAO_POTENCIACAO;
	}

	return Retorno;
}

long double mtdResultadoOperacao(char *operacao, char operador, long double x, long double y)
{
	long double Retorno;

	if (operacao == TIPO_OPERACAO_GENERICA)
	{
		Retorno = mtdOperarGenerico(operador, x, y);
	}

	if (operacao == TIPO_OPERACAO_ADICAO)
	{
		Retorno = mtdOperarAdicao(x, y);
	}

	if (operacao == TIPO_OPERACAO_SUBTRACAO)
	{
		Retorno = mtdOperarSubtracao(x, y);
	}

	if (operacao == TIPO_OPERACAO_MULTIPLICACAO)
	{
		Retorno = mtdOperarMultiplicacao(x, y);
	}

	if (operacao == TIPO_OPERACAO_DIVISAO)
	{
		Retorno = mtdOperarDivisao(x, y);
	}

	if (operacao == TIPO_OPERACAO_POTENCIACAO)
	{
		Retorno = mtdOperarPotenciacao(x, y);
	}

	return Retorno;
}

int main(int argc, char *argv[])
{
    long double x = 0;
    long double y = 0;
    long double Resultado = 0;

	FILE *cfPtr;

	char *comando;
	comando = argv[0];
	char *operacao = mtdDefinirOperacao(comando);
	char operador;

    switch (argc)
	{
		case 1:
			if (operacao == TIPO_OPERACAO_GENERICA)
			{
				cfPtr = fopen("opr.input", "r");
			}
			if (operacao == TIPO_OPERACAO_ADICAO)
			{
				cfPtr = fopen("add.input", "r");
			}
			if (operacao == TIPO_OPERACAO_SUBTRACAO)
			{
				cfPtr = fopen("sub.input", "r");
			}
			if (operacao == TIPO_OPERACAO_MULTIPLICACAO)
			{
				cfPtr = fopen("mul.input", "r");
			}
			if (operacao == TIPO_OPERACAO_DIVISAO)
			{
				cfPtr = fopen("div.input", "r");
			}
			if (operacao == TIPO_OPERACAO_POTENCIACAO)
			{
				cfPtr = fopen("pot.input", "r");
			}

			fscanf(cfPtr, "%Lf", &x);
			if (operacao == TIPO_OPERACAO_GENERICA)
			{
				fscanf(cfPtr, "%s", &operador);
			}
			fscanf(cfPtr, "%Lf", &y);
		break;
		case 2:
		{
			char *chr = argv[1];
			int cchr = 0;
			char *xc;
			char *yc;

			int pix = 0;
			int piy = 0;

			int bx = 0;
			int by = 0;

			int cx = 0;
			int cy = 0;

			while (bx != -1 && by != -1)
			{
				if (chr[cchr] == '(')
				{
					bx = 1;	
					by = 0;
					pix = cchr + 1;
				}

				if (bx == 1 && by == 0)
				{
					cx = cx + 1;
				}

				if (chr[cchr] == ',' || chr[cchr] == '+' || chr[cchr] == '-' || chr[cchr] == '*' || chr[cchr] == '/' || chr[cchr] == '^')
				{
					bx = 0;
					by = 1;
					piy = cchr + 1;
					cx = cx - 1;
					
					if(chr[cchr] != ',')
					{
						operador = chr[cchr];
					}
				}

				if (bx == 0 && by == 1)
				{
					cy = cy + 1;
				}

				if (chr[cchr] == ')')
				{
					bx = -1;
					by = -1;
					cy = cy - 1;
				}

				cchr = cchr + 1;
			}

			xc = (char *)malloc((cx + 1) * sizeof(char));
			yc = (char *)malloc((cy + 1) * sizeof(char));

			int j = 0;
			int k = 0;

			for (int i = 0; i < cchr; i++)
			{
				if (i >= pix && i < pix + cx - 1)
				{
					xc[j] = chr[i];
					j = j + 1;
				}
				
				if (i >= piy && i < piy + cy - 1)
				{
					yc[k] = chr[i]; 
					k = k + 1;
				}
			}

			x = atof(xc);
			y = atof(yc);
		}
		break;
		case 3:
			x = atof(argv[1]);
			y = atof(argv[2]);
		break;
		case 4:
			x = atof(argv[1]);
			operador = argv[2][0];
			y = atof(argv[3]);
		break;
		default:
			printf("%s: numero invalido de argumentos.\n", operacao);
			return 0;
		break;
	}

	Resultado = mtdResultadoOperacao(operacao, operador, x, y);

	if (operacao == TIPO_OPERACAO_GENERICA)
	{
		printf("(%c)\t", operador);
	}
	printf(CONSTANTE_TIPO_NUMERO_DIGITOS, Resultado);

	if (operacao == TIPO_OPERACAO_GENERICA)
	{
		cfPtr = fopen("opr.result", "w");
		fprintf(cfPtr, "(%c)\t", operador);
	}
	if (operacao == TIPO_OPERACAO_ADICAO)
	{
		cfPtr = fopen("add.result", "w");
	}
	if (operacao == TIPO_OPERACAO_SUBTRACAO)
	{
		cfPtr = fopen("sub.result", "w");
	}
	if (operacao == TIPO_OPERACAO_MULTIPLICACAO)
	{
		cfPtr = fopen("mul.result", "w");
	}
	if (operacao == TIPO_OPERACAO_DIVISAO)
	{
		cfPtr = fopen("div.result", "w");
	}
	if (operacao == TIPO_OPERACAO_POTENCIACAO)
	{
		cfPtr = fopen("pot.result", "w");
	}

	fprintf(cfPtr, CONSTANTE_TIPO_NUMERO_DIGITOS, Resultado);

    fclose(cfPtr);
}