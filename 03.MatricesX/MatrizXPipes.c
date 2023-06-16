#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	//Ingreso de parametros para crear las matrices
	int F=-1, CF=-1, C=-1, P=-1;

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
	int M1[F][CF], M2[CF][C], MR[F][C+1];

	for(int i = 0; i < F; i++)
	{
		MR[i][C] = -1;
	}

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

	//Creacion de los pipes
	int fds[P][2];
	for(int i = 0; i < P; i++)
		pipe(fds[i]);

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
				MR[j][C] = i;
			}
			close(fds[i][0]);
			write(fds[i][1], MR, sizeof(MR));
			close(fds[i][1]);
			exit(0);
		}
	}

	//Juntando los resultados en MR en el proceso padre
	int MA[F][C+1];
	for(int i = 0; i < P; i++)
	{
		close(fds[i][1]);
		read(fds[i][0], MA, sizeof(MA));
		close(fds[i][0]);

		for(int j = 0; j < F; j++)
		{
			if(MA[j][C] != -1)
			{
				for(int k = 0; k < C+1; k++)
					MR[j][k] = MA[j][k];
			}
		}
	}

	//Impresion de la matriz resultado
	printf("\nMatriz Resultado:\n");
	for(int i = 0; i < F; i++)
	{
		printf("[ ");
		for(int j = 0; j < C; j++)
		{
			printf("%d ", MR[i][j]);
		}
		printf("] - Proceso %d\n", MR[i][C]);
	}

	printf("\n");
	return 0;

}