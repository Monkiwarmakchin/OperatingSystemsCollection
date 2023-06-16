#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int F=-1, CF=-1, C=-1, P=-1;

	//Ingreso de parametros ded las matrices
	do {
		printf("\nIngresa el numero de Filas que tendra la Matriz 1: ");
		scanf("%d", &F);
		if(F <= 0)
			printf("\n**Numero invalido, ingresa un valor mayor a cero**\n");
	} while(F <= 0);

	do {
		printf("\nIngresa el numero de Columnas que tendra la Matriz 1 y de Filas que tendra la Matriz 2: ");
		scanf("%d", &CF);
		if(CF <= 0)
			printf("\n**Numero invalido, ingresa un valor mayor a cero**\n");
	} while(CF <= 0);

	do {
		printf("\nIngresa el numero de Columnas que tendra la Matriz 2: ");
		scanf("%d", &C);
		if(C <= 0)
			printf("\n**Numero invalido, ingresa un valor mayor a cero**\n");
	} while(C <= 0);
	
	do {
		printf("\nIngresa el numero de Procesos que se utilizaran: ");
		scanf("%d", &P);
		if(P > F || P <= 0)
			printf("\n**Numero invalido, ingresa un valor menor al numero de Filas de la Matriz 1 que ingresaste y mayor a cero**\n");
	} while(P > F || P <= 0);

	//Ingreso de los valores de las matrices
	int M1[F][CF], M2[CF][C], MR[F][C];

	for(int i = 0; i < F; i++)
	{
		for(int j = 0; j < CF; j++)
		{
			printf("\nIngresa el numero que ocupara la posicion Fila %d Columna %d en la Matriz 1 (M1[%d][%d]): ", i, j, i, j);
			scanf("%d", &M1[i][j]);
		}
	}

	for(int i = 0; i < CF; i++)
	{
		for(int j = 0; j < C; j++)
		{
			printf("\nIngresa el numero que ocupara la posicion Fila %d Columna %d en la Matriz 2 (M2[%d][%d]): ", i, j, i, j);
			scanf("%d", &M2[i][j]);
		}
	}

	//Impresion de las matrices
	printf("\n\nMatriz 1:\n");
	for(int i = 0; i < F; i++)
	{
		printf("[ ");
		for(int j = 0; j < CF; j++)
		{
			printf("%d ", M1[i][j]);
		}
		printf("]\n");
	}

	printf("\nMatriz 2:\n");
	for(int i = 0; i < CF; i++)
	{
		printf("[ ");
		for(int j = 0; j < C; j++)
		{
			printf("%d ", M2[i][j]);
		}
		printf("]\n");
	}

	printf("\n");

	//Operacion por procesos
	for(int i = 0; i < P; i++)
	{
		if(fork() == 0)
		{
			int R;
			for(int j = i; j < F; j = j+P)
			{
				for(int k = 0; k < C; k++)
				{
					R = 0;
					for(int l = 0; l < CF; l++)
					{
						R = R + (M1[j][l] * M2[l][k]);
					}
					MR[j][k] = R;
				}
				printf("Proceso %d - Matriz Resultado, Fila %d: [ ", i, j);
				for(int k = 0; k < C; k++)
				{
					printf("%d ", MR[j][k]);
				}
				printf("]\n");
			}
			return 0;
		}
	}

	for( int i=0; i<P; i++ )
		wait();

	printf("\n");
	return 0;

}