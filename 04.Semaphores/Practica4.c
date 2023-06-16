
//Practica 4
//Se requiere que haya una carpeta llamada 'files' en la misma ubicacion
//en donde este programa se ejecute para funcionar.
//Si la carpeta files tiene los archivos txt de las producciones
//ya generados las producciones se agregaran al archivo, por lo que
//se recomienda dejar la carpeta vacia antes de cada ejecucion. 

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

//Variables globales
#define CRITIC_SECTIONS 2 //En este programa las secciones criticas se componen por grupos de 5 secciones,
                          //es decir, cada seccion critica contienen 5 casillas en donde se pueden almacenar producciones.
#define CONSUMERS 4
#define PRODUCERS 4
#define PRODUCTIONS 100000
int *Memoria;
sem_t *semCon[(CRITIC_SECTIONS*5)], *semPro[(CRITIC_SECTIONS*5)], *semCT, *semPT;

//Prototipos
int CrearLigamemoria( );
int DestruyeMemoriaCompartida( int id_Memoria , int *buffer );
sem_t * CrearSemaforo(  char *name , int val );
FILE *fptr;

//Este programa requier que haya una carpeta llamada 'files'
//en la misma ubicacion en donde este se ejecute para funcionar.
void escribir(char c[])
{
    char nombre[20];
    strcpy(nombre, "files/"); //limpia cadena
    strcat(nombre, c);  //copia nombre
    strcat(nombre, ".txt"); // aniade extension
    fptr = fopen(nombre, "a+");
    fprintf(fptr,"%s\n",c);
    fclose(fptr);
}

int main()
{
    //Variables locales
    int pid,id, i, j, tag=10, k=0, l=0, x;

    //Arreglos de strings
    char semConName[(CRITIC_SECTIONS*5)][5];
    char semProName[(CRITIC_SECTIONS*5)][5];
    char Aux[3];

    //Creacion del semaforo de produccion total
    char PTName[3];
    strcpy( PTName, "PT" );
    semPT = CrearSemaforo( PTName , 1 );

    //Creacion del semaforo de consumo total
    char CTName[3];
    strcpy( CTName, "CT" );
    semCT = CrearSemaforo( CTName , 1 );

    //Creacion de los semaforos de las secciones criticas y sus casillas
    for(i=0; i<CRITIC_SECTIONS; i++)
    {
        for(j=0; j<5; j++)
        {
            //Creacion del nombre de los semaforos
            strcpy( semConName[(i*5)+j], "Con" );
            strcpy( semProName[(i*5)+j], "Pro" );
            sprintf(Aux, "%d", (i*10) + j);
            strcat( semConName[(i*5)+j], Aux );
            strcat( semProName[(i*5)+j], Aux );

            semCon[(i*5)+j] = CrearSemaforo( semConName[(i*5)+j] , 0 );
            printf("Se ligo al semaforo del consumidor no.%d, nombre %s: %p\n", i, semConName[(i*5)+j], semCon[(i*5)+j]);
            semPro[(i*5)+j] = CrearSemaforo( semProName[(i*5)+j] , 1 );
            printf("Se ligo al semaforo del productor no.%d, nombre %s: %p\n", i, semProName[(i*5)+j], semPro[(i*5)+j]);
        }
    }

    //tag es una variable auxiliar para que los numeros producidos en las secciones criticas sean mas descriptivos
    while(tag < PRODUCTIONS)
        tag = tag * 10;

    //Si por cualquier razon existia la memoria antes de correr este programa,
    //nos ligamos a ella la destruimos y la volvemos a crear 
    id = CrearLigamemoria();
    DestruyeMemoriaCompartida( id , Memoria );
    id = CrearLigamemoria();

    //Inicializamos los dos espacios extra que tenemos en memoria
    //y que utilizaremos para saber cuanto hemos producido y consumido.
    Memoria[(CRITIC_SECTIONS*5)] = 0;
    Memoria[(CRITIC_SECTIONS*5)+1] = 0;

    //Productores
    for(i=0; i<PRODUCERS; i++)
    {
        pid = fork();
        if( pid == -1 )
            printf("Error al crear el hijo\n");
        else if( pid == 0 )
        {
            for(j=0; j<PRODUCTIONS; j++)
            {
                while(1)
                {
                    //Mensaje que muestra la sincronizacion activa realizandose,
                    //pero ocupa mucho espacio en la terminal.
                    //printf("Productor %d buscando en critica %d, casilla %d.\n", getpid(), k, l);

                    //Revisamos si el semaforo de productor esta arriba
                    //y, por tanto, si la casilla de la seccion critica esta vacia. 
                    sem_getvalue( semPro[(k*5)+l], &x);
                    //printf("Productor %d buscando en critica %d, casilla %d. Estaba %d\n", getpid(), k, l, x);
                    if(x == 1)
                    {
                        //Seccion critica de las producciones
                        sem_wait( semPro[(k*5)+l] );

                            Memoria[(k*5)+l] = i+97;
                        
                            //Seccion critica de la produccion total
                            sem_wait( semPT );
                                Memoria[(CRITIC_SECTIONS*5)+1]++; //Valores producidos entre todos los productores
                                printf("%d Produjo %c en la seccion critica %d, casilla %d, ProTotal: %d\n", getpid(), Memoria[(k*5)+l], k, l, Memoria[(CRITIC_SECTIONS*5)+1]);
                            sem_post( semPT );
                            
                        sem_post( semCon[(k*5)+l] );

                        //Cambiamos de casilla dentro de nuestra seccion critica
                        //antes de salirnos del while
                        l++;
                        if(l == 5)
                            l=0;

                        //Salimos del while si es que ya producimos dentro de esta seccion critica
                        break;
                    }

                    //Cambiamos de casilla dentro de nuestra seccion critica
                    //volvemos a la primera si estamos en la ultima 
                    l++;
                    if(l == 5)
                        l=0;
                }

                //Cambiamos de seccion critica
                //volvemos a la primera si estamos en la ultima 
                k++;
                if(k == CRITIC_SECTIONS)
                    k=0;

            }

            //Deslindamos el proceso actual de todos los semaforos
            for(j=0; j<(CRITIC_SECTIONS*5); j++)
            {
                sem_unlink( semProName[j] );
                sem_unlink( semConName[j] );
            }
            sem_unlink( PTName );
            sem_unlink( CTName );

            printf("Productor %d finalizo.\n", getpid());
            exit(0);
        }
    }
    
    //Consumidores
    for(i=0; i<CONSUMERS; i++)
    {
        pid = fork();
        if( pid == -1 )
            printf("Error al crear el hijo\n");
        else if( pid == 0 )
        {
            char str[2]={'x','\0'};

            //Mientras las variables consumidas sean menores a las producciones deseadas (producciones x productores)
            while(Memoria[(CRITIC_SECTIONS*5)] < (PRODUCTIONS * PRODUCERS))
            {
                //Mensaje que muestra la sincronizacion activa realizandose,
                //pero ocupa mucho espacio en la terminal.
                //printf("Consumidor %d buscando en critica %d, casilla.\n", getpid(), k, l);

                //Revisamos si el semaforo de consumidor esta arriba
                //y, por tanto, si la casilla de la seccion critica esta llena. 
                sem_getvalue( semCon[(k*5)+l], &x);
                //printf("Productor %d buscando en critica %d, casilla %d. Estaba %d\n", getpid(), k, l, x);
                if(x == 1)
                {
                    //Seccion critica de las producciones
                    sem_wait( semCon[(k*5)+l] );

                        //Convertimos el valor de la casilla en un string y lo mandamos a imprimir
                        str[0] = Memoria[(k*5)+l];

                        //Seccion critica del consumo total
                        sem_wait( semCT );
                            escribir(str);
                            Memoria[(CRITIC_SECTIONS*5)]++; //Consumidos
                            printf("%d Consumio %c en la seccion critica %d, casilla %d, ConTotal: %d\n", getpid(), Memoria[(k*5)+l], k, l, Memoria[CRITIC_SECTIONS*5]) ;
                        sem_post( semCT );

                    sem_post( semPro[(k*5)+l] );

                    //Al ya haber consumido en esta seccion critica,
                    //podemos pasar a la siguiente
                    //volvemos a la primera si estamos en la ultima
                    k++;
                    if(k == CRITIC_SECTIONS)
                        k=0;

                }
                //Cambiamos de seccion critica de producciones,
                //volvemos a la primera si estamos en la ultima
                l++;
                if(l == 5)
                    l=0;
            }

            //Deslindamos el proceso actual de todos los semaforos
            for(j=0; j<CRITIC_SECTIONS; j++)
            {
                sem_unlink( semProName[j] );
                sem_unlink( semConName[j] );
            }
            sem_unlink( PTName );
            sem_unlink( CTName );

            printf("Consumidor %d finalizo.\n", getpid());
            exit(0);
        }
    }


    //Padre, funcionando como un administrador
    //Esperamos a todos los procesos creados
    for(i=0; i < PRODUCERS + CONSUMERS; i++)
        wait(NULL);

    //Destruimos la memoria compartida
    DestruyeMemoriaCompartida( id , Memoria );

    //Nos deslindamos de todos los semaforos
    for(j=0; j<(CRITIC_SECTIONS*5); j++)
    {
        sem_unlink( semProName[j] );
        sem_unlink( semConName[j] );
    }
    sem_unlink( PTName );
    sem_unlink( CTName );

    //Cerramos y destruimos todos los semaforos
    for(i = 0; i < (CRITIC_SECTIONS*5); i++)
    {
        sem_close( semPro[i] );
        sem_destroy( semPro[i] );
        sem_close( semCon[i] );
        sem_destroy( semCon[i] );
    }
    sem_close( semPT );
    sem_destroy( semPT );
    sem_close( semCT );
    sem_destroy( semCT );

    return 0;
}

int CrearLigamemoria( void )
{
    int key2,shmid;
    key2 = ftok("/bin/ls",33);

    //Verifica si existe la zona de memoria
    if( ( shmid = shmget( key2 , sizeof( int ) * ((CRITIC_SECTIONS*5) + 2) , IPC_CREAT|IPC_EXCL|0666) ) == -1 )
    {
        // Si existe obtiene la memoria 
        if( ( shmid = shmget( key2 , sizeof( int ) * ((CRITIC_SECTIONS*5) + 2) , 0 ) ) == -1 )
        {
            perror("shmget \n");
            exit(1);
        }
        else
            printf("Se ligo a la memoria con id: %d\n", shmid);
    }
    else
        printf("Creo la memoria con id: %d\n", shmid);

    //Se liga a la zona de memoria compartida
    if( ( Memoria = (int*)shmat( shmid , (int*) 0 , 0 ) ) == (void*)-1 )
    {
        perror("shmat \n");
        exit(1);
    }

    return shmid; //Descriptos de la memoria
}

int DestruyeMemoriaCompartida( int id_Memoria , int *buffer ){
    if( id_Memoria != 0 || id_Memoria != -1 ) //Valida si ya se destruyo
    {
        shmdt( buffer ); //Quita la liga hacia la memoria
        shmctl( id_Memoria , IPC_RMID , (struct shmid_ds *) NULL ); //Destruye la zona de memoria
        printf("Se destruyo la memoria\n");
    } 
    else{
        shmdt( buffer );
        printf("Se desligo la memoria\n");
    }
    
}

sem_t * CrearSemaforo( char *name , int val)
{
    sem_t *mut;
    if( ( mut = sem_open( name , O_CREAT | O_EXCL , 0666 , val ) ) == SEM_FAILED )
    {
        if( ( mut = sem_open( name , 0 ) ) == SEM_FAILED )
            printf("Error al abrir el semaforo\n");
        else
            printf("Ligado al semaforo correctamente %p\n", mut);
    }
    else
    {
        printf("Semaforo creado: %p\n",mut);
        sem_init( mut , 1 , val );
    }

    return mut;
}