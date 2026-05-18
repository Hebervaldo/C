#include <stdio.h>
#include <stdlib.h> 
//#include <time.h>
#include <math.h>
//#include <fcntl.h>

#define NUMEROPADROES 4
#define NUMEROENTRADAS 2 
#define NUMEROESCONDIDA 60
#define NUMEROSAIDAS 1
#define NUMEROITERACOES 1

#define rando() ((float)rand()/RAND_MAX)


    int    i, j, k, p, np, op, ranpad[NUMEROPADROES+1], epoca;
    int    NumPadroes = NUMEROPADROES, NumEntrada = NUMEROENTRADAS, NumEscondida = NUMEROESCONDIDA, NumSAIDA = NUMEROSAIDAS;
    float entrada[NUMEROPADROES+1][NUMEROENTRADAS+1];
    float target[NUMEROPADROES+1][NUMEROSAIDAS+1];
    float SomaEscondida[NUMEROPADROES+1][NUMEROESCONDIDA+1], W12[NUMEROENTRADAS+1][NUMEROESCONDIDA+1], Escondida[NUMEROPADROES+1][NUMEROESCONDIDA+1];
    float SomaSaida[NUMEROPADROES+1][NUMEROSAIDAS+1], W23[NUMEROESCONDIDA+1][NUMEROSAIDAS+1], SAIDA[NUMEROPADROES+1][NUMEROSAIDAS+1];
    float DeltaS[NUMEROSAIDAS+1], somaDWS[NUMEROESCONDIDA+1], DeltaE[NUMEROESCONDIDA+1];
    float DeltaW12[NUMEROENTRADAS+1][NUMEROESCONDIDA+1] , DeltaW23[NUMEROESCONDIDA+1][NUMEROSAIDAS+1];
    float Erro, eta = 0.0, alpha = 0, wmax = 1;



void recebe_entradas(void)
{

    int l, m;
    FILE *cfPtr;


    for (m=1; m<=NUMEROPADROES+1; m++)
    {
        for (l=1; l<=NUMEROENTRADAS+1; l++)
        { 
    
            entrada[m][l]=0;
            
        }

    }
    cfPtr = fopen("entradasteste.dat", "r");
    for (m=1; m<=NUMEROPADROES; m++)
    {
    for (l=1; l<=NUMEROENTRADAS; l++)
    { 
    
        fscanf(cfPtr, "%f", &entrada[m][l]);
        //printf("%f\t", ENTRADA[l][m]);
        
        //normalizando...
        entrada[m][l]=entrada[m][l];
       
    }
    printf("\n");
    } 
    fclose(cfPtr);

         
}



void inicializa_pesos()
{

int m, n, o;
    FILE *cfPtr;
    
    cfPtr = fopen("pesos.dat", "r");
    
    for (o=0; o<=NUMEROESCONDIDA; o++)
    {
    for (n=0; n<=NUMEROENTRADAS; n++)
    { 
    
        fscanf(cfPtr, "%f", &W12[n][o]);

       
       
    }
    } 
    
    for (o=0; o<=NUMEROSAIDAS; o++)
    {
    for (n=0; n<=NUMEROESCONDIDA; n++)
    { 
    
        fscanf(cfPtr, "%f", &W23[n][o]);

       
       
    }
    } 
    
    fclose(cfPtr);

}

void exporta_pesos()
{
    
    int l, m; 
    FILE *cfPtr;
    cfPtr=fopen("conferepesos.dat", "w");
    for (m=0; m<=NUMEROESCONDIDA; m++)
    {
        for (l=0; l<=NUMEROENTRADAS; l++)
            fprintf(cfPtr,"%f\t", W12[l][m]);
        fprintf(cfPtr, "\n");    
    } 
    
    
    
    for (m=0; m<=NUMEROSAIDAS; m++)
    {
        for (l=0; l<=NUMEROESCONDIDA; l++)
            fprintf(cfPtr,"%f\t", W23[l][m]);
        fprintf(cfPtr, "\n");    
    } 
    fclose(cfPtr);
}



void escreve_saida()
{
     
    FILE *cfPtr;
        
    cfPtr=fopen("resultadosteste.dat", "w");
        fprintf(cfPtr, "\n\nDados da rede - numero de iteracoes %d\n\nPat\t", epoca) ;   /* mostra as SAIDAs */
    for( i = 1 ; i <= NumEntrada ; i++ ) {
        fprintf(cfPtr, "ENTRADA%-4d\t", i) ;
    }
    for( k = 1 ; k <= NumSAIDA ; k++ ) {
        fprintf(cfPtr, "target%-4d\tSAIDA%-4d\t", k, k) ;
    }
    for( p = 1 ; p <= NumPadroes ; p++ ) {        
    fprintf(cfPtr, "\n%d\t", p) ;
        for( i = 1 ; i <= NumEntrada ; i++ ) {
            fprintf(cfPtr, "%f\t", entrada[p][i]) ;
        }
        for( k = 1 ; k <= NumSAIDA ; k++ ) {
            fprintf(cfPtr, "%f\t%f\t", target[p][k], SAIDA[p][k]) ;
        }
    }
    fprintf(cfPtr, "\n\nPassar bem!\n\n") ;




     fclose(cfPtr);
     exporta_pesos();
     
}



int main(void) {

    recebe_entradas();  
   // recebe_target();
    inicializa_pesos(); 
  
/*    for( j = 1 ; j <= NumEscondida ; j++ ) {   
        for( i = 0 ; i <= NumEntrada ; i++ ) { 
            DeltaW12[i][j] = 0.0 ;
            W12[i][j] = 2.0 * ( rando() - 0.5 ) * wmax ;
        }
    }
    for( k = 1 ; k <= NumSAIDA ; k ++ ) {   
        for( j = 0 ; j <= NumEscondida ; j++ ) {
            DeltaW23[j][k] = 0.0 ;              
            W23[j][k] = 2.0 * ( rando() - 0.5 ) * wmax ;
        }
    }
    */
     
    for( epoca = 0 ; epoca < NUMEROITERACOES ; epoca++) {    /* faz a iteração da atualização dos pesos */
        for( p = 1 ; p <= NumPadroes ; p++ ) {    /* randomiza a ordem dos indivíduos */
            ranpad[p] = p ;
        }
        for( p = 1 ; p <= NumPadroes ; p++) {
            np = p + rando() * ( NumPadroes + 1 - p ) ;
            op = ranpad[p] ; ranpad[p] = ranpad[np] ; ranpad[np] = op ;
        }
        Erro = 0.0 ;
        for( np = 1 ; np <= NumPadroes ; np++ ) {    /* repete para todos os padrões de treinamento */
            p = ranpad[np];
            for( j = 1 ; j <= NumEscondida ; j++ ) {    /* computa as ativações da unidade escondida */
                SomaEscondida[p][j] = W12[0][j] ;
                for( i = 1 ; i <= NumEntrada ; i++ ) {
                    SomaEscondida[p][j] += entrada[p][i] * W12[i][j] ;
                }
                Escondida[p][j] = 1.0/(1.0 + exp(-SomaEscondida[p][j])) ;
            }
            for( k = 1 ; k <= NumSAIDA ; k++ ) {    /* computa as unidades de ativação da saída e erros */
                SomaSaida[p][k] = W23[0][k] ;
                for( j = 1 ; j <= NumEscondida ; j++ ) {
                    SomaSaida[p][k] += Escondida[p][j] * W23[j][k] ;
                }
                SAIDA[p][k] = 1.0/(1.0 + exp(-SomaSaida[p][k])) ;   /* Sigmoidal SAIDAs */
/*              SAIDA[p][k] = SomaSaida[p][k];      Linear SAIDAs */
                Erro += 0.5 * (target[p][k] - SAIDA[p][k]) * (target[p][k] - SAIDA[p][k]) ;   /* SSE */
/*              Erro -= ( target[p][k] * log( SAIDA[p][k] ) + ( 1.0 - target[p][k] ) * log( 1.0 - SAIDA[p][k] ) ) ;    Erro de Entropia Cruzada */
                DeltaS[k] = (target[p][k] - SAIDA[p][k]) * SAIDA[p][k] * (1.0 - SAIDA[p][k]) ;   /* Sigmoidal SAIDAs, SSE */
/*              DeltaS[k] = target[p][k] - SAIDA[p][k];     Sigmoidal SAIDAs, Cross-Entropy Erro */
/*              DeltaS[k] = target[p][k] - SAIDA[p][k];     Linear SAIDAs, SSE */
            }
            for( j = 1 ; j <= NumEscondida ; j++ ) {    /* retropropagação de erros para a camada escondida */
                somaDWS[j] = 0.0 ;
                for( k = 1 ; k <= NumSAIDA ; k++ ) {
                    somaDWS[j] += W23[j][k] * DeltaS[k] ;
                }
                DeltaE[j] = somaDWS[j] * Escondida[p][j] * (1.0 - Escondida[p][j]) ;
            }
            for( j = 1 ; j <= NumEscondida ; j++ ) {     /* atualiza pesos w12 */
                DeltaW12[0][j] = eta * DeltaE[j] + alpha * DeltaW12[0][j] ;
                W12[0][j] += DeltaW12[0][j] ;
                for( i = 1 ; i <= NumEntrada ; i++ ) { 
                    DeltaW12[i][j] = eta * entrada[p][i] * DeltaE[j] + alpha * DeltaW12[i][j];
                    W12[i][j] += DeltaW12[i][j] ;
                }
            }
            for( k = 1 ; k <= NumSAIDA ; k ++ ) {    /* atualiza pesos W23 */
                DeltaW23[0][k] = eta * DeltaS[k] + alpha * DeltaW23[0][k] ;
                W23[0][k] += DeltaW23[0][k] ;
                for( j = 1 ; j <= NumEscondida ; j++ ) {
                    DeltaW23[j][k] = eta * Escondida[p][j] * DeltaS[k] + alpha * DeltaW23[j][k] ;
                    W23[j][k] += DeltaW23[j][k] ;
                }
            }
        }
        fprintf(stdout, "\nepoca %-5d :   Erro = %f", epoca, Erro) ;
        if( Erro < 0.00001 ) break ;  /* pára o aprendizado quando o erro convergir para o valor descrito */
    }
    
    fprintf(stdout, "\n\nNETWORK DATA - epoca %d\n\nPat\t", epoca) ;   /* mostra as SAIDAs */
    for( i = 1 ; i <= NumEntrada ; i++ ) {
        fprintf(stdout, "ENTRADA%-4d\t", i) ;
    }
    for( k = 1 ; k <= NumSAIDA ; k++ ) {
        fprintf(stdout, "target%-4d\tSAIDA%-4d\t", k, k) ;
    }
    for( p = 1 ; p <= NumPadroes ; p++ ) {        
    fprintf(stdout, "\n%d\t", p) ;
        for( i = 1 ; i <= NumEntrada ; i++ ) {
            fprintf(stdout, "%f\t", entrada[p][i]) ;
        }
        for( k = 1 ; k <= NumSAIDA ; k++ ) {
            fprintf(stdout, "%f\t%f\t", target[p][k], SAIDA[p][k]) ;
        }
    }
    fprintf(stdout, "\n\nPassar bem!\n\n") ;
    //system("pause");
    for(i=1; i<=NUMEROPADROES;i++)
    {
        for(j=1;j<=NUMEROENTRADAS; j++)
            printf("%f\t", entrada[i][j]);
        printf("\n");
    }
    //system("pause");
    for(i=1; i<=NUMEROPADROES;i++)
    {
        for(j=1;j<=NUMEROSAIDAS; j++)
            printf("%f\t", target[i][j]);
        printf("\n");
    }
    
    
    
    
    
    //////////////////////////////////////////////////////////////////////////////////////
    
    
    
    

   //system("pause");
   escreve_saida();

    return 1 ;
}


