#include <SDL.h>
#include <stdio.h>
//--------------------------Ejemplo sem�foro condici�n de carrera---------------------------
//int contador = 0;
//SDL_sem* semaforo;
//
//int incrementarContador(void* data) {
//    for (int i = 0; i < 5; ++i) {
//        SDL_SemWait(semaforo);  // Entrada en secci�n cr�tica
//        int temp = contador;
//        SDL_Delay(50);          // Simula trabajo en secci�n cr�tica
//        contador = temp + 1;
//        printf("Contador: %d\n", contador);
//        SDL_SemPost(semaforo);  // Salida de secci�n cr�tica
//    }
//    return 0;
//}
//
//int main() {
//    SDL_Init(SDL_INIT_VIDEO);
//
//    semaforo = SDL_CreateSemaphore(1);
//
//    SDL_Thread* thread1 = SDL_CreateThread(incrementarContador, "Hilo1", NULL);
//    SDL_Thread* thread2 = SDL_CreateThread(incrementarContador, "Hilo2", NULL);
//
//    SDL_WaitThread(thread1, NULL);
//    SDL_WaitThread(thread2, NULL);
//
//    SDL_DestroySemaphore(semaforo);
//    SDL_Quit();
//    return 0;
//}

//-----------------------Ejemplo sem�foro sincronizaci�n de hilos-----------------------------
SDL_sem* semaforo;

int tarea1(void* data) {
    printf("Tarea 1 iniciada.\n");
    SDL_Delay(1000); // Simula trabajo
    printf("Tarea 1 completada.\n");
    SDL_SemPost(semaforo); // Se�ala que termin� la tarea 1
    return 0;
}

int tarea2(void* data) {
    printf("Tarea 2 esperando a que tarea 1 termine.\n");
    SDL_SemWait(semaforo); // Espera hasta que tarea 1 termine
    printf("Tarea 2 iniciada despu�s de tarea 1.\n");
    return 0;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    semaforo = SDL_CreateSemaphore(0);

    SDL_Thread* hilo1 = SDL_CreateThread(tarea1, "Hilo1", NULL);
    SDL_Thread* hilo2 = SDL_CreateThread(tarea2, "Hilo2", NULL);

    SDL_WaitThread(hilo1, NULL);
    SDL_WaitThread(hilo2, NULL);

    SDL_DestroySemaphore(semaforo);
    SDL_Quit();
    return 0;
}