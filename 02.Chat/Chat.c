#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXSZ 210

int main(int argc, char *argv[])
{
	char str[MAXSZ];

	//Creacion de los pipes
	int fdsP[2], fdsH[2];
	pipe(fdsP);
	pipe(fdsH);

	//Hijo
	if(fork() == 0)
	{
		close(fdsP[1]);
		close(fdsH[0]);

		while(strcmp(str,"FIN"))
		{
			read(fdsP[0], str, sizeof(str));

			printf("\nPadre dice: %s", str);

			if(!strcmp(str,"FIN"))
				break;

			printf("\nHijo responde: ");
			fgets(str, MAXSZ, stdin);
			if ((strlen(str) > 0) && (str[strlen(str) - 1] == '\n'))
        		str[strlen(str) - 1] = '\0';
			
			write(fdsH[1], str, sizeof(str));
		}

		close(fdsP[0]);
		close(fdsH[1]);
		exit(0);
	}

	//Padre
	close(fdsP[0]);
	close(fdsH[1]);

	printf("\nPadre inicia: ");
	fgets(str, MAXSZ, stdin);
	if ((strlen(str) > 0) && (str[strlen(str) - 1] == '\n'))
        	str[strlen(str) - 1] = '\0';
	
	write(fdsP[1], str, sizeof(str));

	while(strcmp(str,"FIN"))
	{
		read(fdsH[0], str, sizeof(str));

		printf("\nHijo dice: %s", str);

		if(!strcmp(str,"FIN"))
			break;

		printf("\nPadre responde: ");
		fgets(str, MAXSZ, stdin);
		if ((strlen(str) > 0) && (str[strlen(str) - 1] == '\n'))
        	str[strlen(str) - 1] = '\0';
		
		write(fdsP[1], str, sizeof(str));
	}

	wait();

	printf("\n\n");

	close(fdsH[0]);
	close(fdsP[1]);
	return 0;

}