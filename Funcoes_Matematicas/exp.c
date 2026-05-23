#include <math.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <sys/time.h>

#define err_c 		1.0e-100
#define exp_c		2.718281828459045235360287471352662497757
#define ln10_c		2.302585092994045901093613792909309267997
#define invln10_c	.4342944819032517867168474018368939937317

long tInicial, tFinal, tTempoDecorrido;

struct ut_tv
{
   int32_t tv_sec; /* Seconds */
   int32_t tv_usec; /* Microseconds */
}; /* Time entry was made */

struct timeval ut_tv; /* Time entry was made */
struct timeval tvInicial, tvFinal;

long mtdCronometroIniciar()
{
	gettimeofday(&tvInicial, NULL);

	tInicial = (tvInicial.tv_sec * 1e6) + (tvInicial.tv_usec);
	return tInicial;
}

long mtdCronometroParar()
{
	gettimeofday(&tvFinal, NULL);

	tFinal = (tvFinal.tv_sec * 1e6) + (tvFinal.tv_usec); 
	return tFinal;
}

long mtdCronometroTempoDecorrido()
{
	tTempoDecorrido = (tFinal - tInicial);
	return tTempoDecorrido;
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

long double mtdCalcularExponencial(long double x)
{
	long double Retorno = 0;

	FILE *cfPtr;

	mtdCronometroIniciar();

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

	mtdCronometroParar();
	
	cfPtr = fopen("exp.result", "w");
	fprintf(cfPtr, "%.100Lfn", Retorno);
	cfPtr = fopen("exp.loop", "w");
	fprintf(cfPtr, "%d\n", (j - 1));
	cfPtr = fopen("exp.error", "w");
	fprintf(cfPtr, "%.100Lf\n", err);
	cfPtr = fopen("exp.time", "w");
	fprintf(cfPtr, "%ld us\n", mtdCronometroTempoDecorrido());

	fclose(cfPtr);
	
	return Retorno;
}

long double mtdCalcularExponencial_n(long double x, int n)
{
	long double Retorno = 0;

	FILE *cfPtr;

	mtdCronometroIniciar();

	long double xn = (x * invln10_c);

	long double axn = mtdObterValorAbsoluto(xn);
	long double ixn = (long double)((int) xn);
	long double aixn = mtdObterValorAbsoluto(ixn);
	long double afxn = (axn - aixn);
	long double xe = afxn * ln10_c;

	long double oldr = 0;
	long double r = 1 + xe;
	long double div = 1;
	long double m = xe;
	long double err = 0;
	long double exp_maiorexatidao = mtdCalcularExponencial(x);

	if (x != 0)
	{
		for (int j = 2; j <= (n + 1); j++)
		{
			div = div * j;
			m = m * xe;
			oldr = r;
			r = r + (m / div);
			err = mtdObterValorAbsoluto(r - exp_maiorexatidao);
		}

		long double sixn = (ixn / aixn);
		long double r10n = 1;

		if (aixn > 0)
		{
			for(int i = 0; i < aixn; i++)
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
		err = 0;
		Retorno = 1;
	}

	mtdCronometroParar();

	cfPtr = fopen("exp.result", "w");
	fprintf(cfPtr, "%.100Lfn", Retorno);
	cfPtr = fopen("exp.loop", "w");
	fprintf(cfPtr, "%d\n", n);
	cfPtr = fopen("exp.erro", "w");
	fprintf(cfPtr, "%.100Lf\n", err);
	cfPtr = fopen("exp.time", "w");
	fprintf(cfPtr, "%ld us\n", mtdCronometroTempoDecorrido());

	fclose(cfPtr);
	
	return Retorno;
}

int main(int argc, char *argv[])
{
	switch (argc)
	{
	case 2:
		printf("%.100Lf\n", mtdCalcularExponencial(atof(argv[1])));
		break;
	case 3:
		printf("%.100Lf\n", mtdCalcularExponencial_n(atof(argv[1]), atof(argv[2])));
		break;
	default:
		printf("exp: numero invalido de argumentos.\n");
		break;
	}

	printf("%.100lf\n", exp(atof(argv[1])));
}
