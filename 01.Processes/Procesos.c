#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <signal.h>
#include <string.h>

int cloneando(int flag)
{
    if(flag == 1)
        printf("\n\nHola, soy el proceso %d, hijo de %d, me dicen 'El Agüelo'...\n
                    JAJAJA... ¡Caiste en mi trampa! ¡Yo en realidad soy un clon de 'El Agüelo'!\n
                    Estuve esperando que 'El Padre' muriera, mediante 'waitpid()', para que no hubiera testigos,\n
                    incluso, utilizando la flag 'CLONE_PARENT', me pude robar el pid del padre de 'El Agüelo' para mejorar mi disfraz,\n
                    es una de la ventaja de los clones frente a los hijos, los clones podemos utilizar cierta informacion de nuestros procesos invocadores,\n
                    tambien, por ejemplo, a diferencia de esos feos forkeados, yo estoy ejecutando una funcion completamente diferente a la de mi 'padr...\n
                    incluso me pasaron un argumento para poder ejecutar esta funcion, argumento bastante inutil pero es solo demostrar que se pue...", getpid(), getppid());
    return 0;
}
 
int main(int argc, char *argv[])
{
    pid_t pid1, pid2;
    int status1, status2;
 
    if ( (pid1=fork()) == 0 )
    { /* hijo (1a generacion) = padre */
        if ( (pid2=fork()) == 0 )
        { /* hijo (2a generacion)  = nieto */
            printf("\n\nHola, soy el proceso %d, hijo de %d, me dicen 'El Nieto'.\n
                        Yo voy a ejecutar la funcion de shell 'ls -st' mediante 'execl()',\n
                        por lo que desplegare una lista detallada de todos los archivos que tengas\n
                        en el directorio en que estes ejecutando este programa,\nestaran ordenados por tiempo de modificacion:\n\n", getpid(), getppid());
            execl("/bin/ls", "ls", "-lt", 0);
        }
        else
        { /* padre (2a generacion) = padre */
            wait(&status2);
            printf("\n\nHola, soy el proceso %d, hijo de %d, me dicen 'El Padre'.\n
                        Estuve esperando que mi hijo: 'El Nieto', acabara de hacer su porqueria con 'wait()',\n
                        y ahora que por fin esta muerto, yo voy crear una carpeta en este mismo directorio\n
                        usando el comando 'mkdir' mediante 'execve()',\n
                        mandaria variables de entorno para que usar 'exeve()' tenga sentido,\n
                        pero para hacer eso deberia haber otro programa que usara dichas variables,\n
                        un programa que se tendria explicar que hace y mandar junto con este y bueno,\n
                        primero: que flojera, y segundo: nadie especifico que se tenia que hacer eso\n
                        asi que por lo mientras nos conformaremos con esto.", getpid(), getppid());
            char *args[] = { "/bin/mkdir", "LukeYoSoy'ElPadre'", NULL };
            execve("/bin/mkdir", args, NULL);
        }
    }
    else
    { /* padre (1a generacion) = abuelo */
        waitpid(pid1, &status1, 0);
        //void **pila_hijo;
        //pila_hijo = malloc(100000);
        char pila_hijo[1024*1024];
        clone( cloneando, pila_hijo+sizeof(pila_hijo), CLONE_PARENT | CLONE_VFORK, 1);
        printf("\n\nAhhh..., estos clones, siempre se creen la gran cosa, nunca se acuerdan de que uno es el que los invoca,\n
                    ¿sabias que estas cosas son como parasitos?, literalmente necesitan que les des una parte de la pila que como proceso tienes asignada...\n
                    ni se dio cuenta que tenia tambien la flag 'CLONE_VFORK', con ella yo solo espero a que se calle para empezar mi trabajo...\n
                    Hola por cierto, yo soy el proceso %d, hijo de %d, aunque a este ultimo ya lo debiste conocer por mi copia barata,\n
                    sí, soy 'El Agüelo', pero el de deberas,y pues la verdad estoy aqui solo para cerrar el changarro mi buen.\n
                    Sí, ya fue mucho de hijos por aqui y por alla, asi que 'ushquele de mi programa, chismoson\n
                    ¿Que no tiene nada mejor que hacer?...\n\n", getpid(), getppid());
    }
 
    return 0;
}