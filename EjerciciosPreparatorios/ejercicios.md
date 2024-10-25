# Ejercicios preparatorios
Dados algunos problemas tratando de realizar los ejercicios en Visual Studio, continuaré utilizando Codespaces.

## Ejercicio 6: esperar un hilo
En el ejercicio 5 el programa terminaba independientemente de si los hilos habían terminado sus tareas o no. Para solucionarlo, hay que esperar a que terminen los procesos que se les han encargado antes de acabar el programa.

``` C
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
struct threadParam_t{
    char character;
    int counter;
};

void* imprime(void *parg){
    struct threadParam_t *pargTmp = (struct threadParam_t *)parg;
    for(int i = 0; i < pargTmp->counter;i++){
        printf("%c",pargTmp->character);
    }
    return NULL;
}

int main(int argc, char *argv[]){
    pthread_t threadID1;
    pthread_t threadID2;

    struct threadParam_t threadParam1 = {'a',30000};
    struct threadParam_t threadParam2 = {'b',20000};

    pthread_create(&threadID1,NULL,&imprime, &threadParam1);
    pthread_create(&threadID2,NULL,&imprime, &threadParam2);

    pthread_join(threadID1,NULL);
    pthread_join(threadID2,NULL);

    exit(EXIT_SUCCESS);
}
```
La función `pthread_join` se utiliza para esperar a que un hilo termine su ejecución. Esta se asegura de que el hilo principal (en este caso, el que ejecuta **main**) no continúe hasta que los hilos especificados (**threadID1** y **threadID2**) hayan finalizado. Esto es crucial para garantizar que todos los resultados de los hilos se completen antes de que el programa finalice.

Para que un hilo en particular pueda terminar, debe incluirse la siguiente línea de código luego de haberlo creado con los argumentos especificados.

``` C
pthread_join(identificadorHilo,valorDeRetornoHilo);
```

**Diferencia entre los siguientes fragmentos de código**

**1).**
``` C
pthread_create(&threadID1,NULL,&imprime, &threadParam1);
pthread_join(threadID1,NULL);
pthread_create(&threadID2,NULL,&imprime, &threadParam2);
pthread_join(threadID2,NULL);
```

**2).**
``` C
pthread_create(&threadID1,NULL,&imprime, &threadParam1);
pthread_create(&threadID2,NULL,&imprime, &threadParam2);
pthread_join(threadID1,NULL);
pthread_join(threadID2,NULL);
```
Teniendo en cuenta lo que hace la función `pthread_join`, podría decirse que mientras que en **1** se crea el hilo threadID1 y se espera que termine antes de poder crear el hilo threadID2 y también esperar que termine, en **2** se crean ambos hilos y luego se espera a que termine threadID1, para luego esperar a que termine threadID2.

## Ejercicio 7: para pensar
Para luego poder crear hilos en el juego y manejarlos con SDL2, es clave tener la siguiente información:

### ¿Cómo se crea un hilo?
Para crear un hilo en SDL2, se utiliza la función SDL_CreateThread. Esta función toma como argumentos el nombre de la función que se ejecutará en el hilo, un puntero a un argumento que se pasará a esa función, y un puntero para recibir la identificación del hilo.
### Ejemplo
```C
#include <SDL.h>
#include <stdio.h>

//Función que ejecutará el hilo
int threadFunction(void *data) {
    int *num = (int *)data;
    for (int i = 0; i < 5; i++) {
        printf("Hilo: %d, Iteración: %d\n", *num, i);
        SDL_Delay(500); // Pausa la ejecución del hilo durante 500 milisegundos (0.5 segundos)
    }
    return 0;
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING); // Inicializa todas las subsistemas de SDL
    
    int threadData = 1; // Crea una variable entera que se pasará al hilo
    
    SDL_Thread *thread = SDL_CreateThread(threadFunction, "MiHilo", &threadData);
    
    // Esperar a que el hilo termine
    SDL_WaitThread(thread, NULL);
    
    SDL_Quit();
    return 0;
}
```
### ¿Cuál es el equivalente de `join` en la API de SDL2?
### Ejemplo.
### ¿Para qué sirven los semáforos en SDL2?
### ¿Qué es una condición de carrera?
### Ejemplo de semáforo que evita una condición de carrera
### Ejemplo de semáforo para sincronizar dos hilos