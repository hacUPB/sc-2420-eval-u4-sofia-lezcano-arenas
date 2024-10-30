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

**Resultado de la ejecución del programa**
![alt text](image.png)

![alt text](image-1.png)

![alt text](image-2.png)

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
Para crear un hilo en SDL2, se utiliza la función `SDL_CreateThread`. Esta función toma como argumentos el nombre de la función que se ejecutará en el hilo, un puntero a un argumento que se pasará a esa función, y un puntero para recibir la identificación del hilo.
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
En SDL2, la función `SDL_WaitThread` es el equivalente de join. Esta función espera a que el hilo termine, bloqueando el hilo principal hasta que el hilo específico finalice.
### Ejemplo
```C
#include <SDL.h>
#include <stdio.h>

int threadFunction(void *data) {
    int *num = (int *)data;
    for (int i = 0; i < 5; i++) {
        printf("Hilo en ejecución: %d\n", *num);
        SDL_Delay(500); 
    }
    return 0;
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING); 
    
    int threadData = 1;
    
    SDL_Thread *thread = SDL_CreateThread(threadFunction, "MiHilo", &threadData);
    
    // Esperar a que el hilo termine
    SDL_WaitThread(thread, NULL);
    
    SDL_Quit();
    return 0;
}
```
### ¿Para qué sirven los semáforos en SDL2?
En SDL2, los semáforos (mediante `SDL_Sem*`) ayudan a sincronizar el acceso a recursos compartidos entre hilos, evitando condiciones de carrera.
### ¿Qué es una condición de carrera?
 Es cuando múltiples hilos intentan acceder a la misma variable o recurso sin la debida sincronización, generando resultados inesperados.
### Ejemplo de semáforo que evita una condición de carrera
``` C
#include <SDL.h>
#include <stdio.h>

int contador = 0;
SDL_sem* semaforo;

int incrementarContador(void* data) {
    for (int i = 0; i < 5; ++i) {
        SDL_SemWait(semaforo);  // Entrada en sección crítica
        int temp = contador;
        SDL_Delay(50);          // Simula trabajo en sección crítica
        contador = temp + 1;
        printf("Contador: %d\n", contador);
        SDL_SemPost(semaforo);  // Salida de sección crítica
    }
    return 0;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    semaforo = SDL_CreateSemaphore(1);

    SDL_Thread* thread1 = SDL_CreateThread(incrementarContador, "Hilo1", NULL);
    SDL_Thread* thread2 = SDL_CreateThread(incrementarContador, "Hilo2", NULL);

    SDL_WaitThread(thread1, NULL);
    SDL_WaitThread(thread2, NULL);

    SDL_DestroySemaphore(semaforo);
    SDL_Quit();
    return 0;
}
```
El resultado de este programa, que es el esperado es:
![alt text](image-4.png)

Si en el ejemplo se comentan todas las líneas que tienen que ver con el semáforo, el resultado es el siguiente:
![alt text](image-3.png)

La condición de carrera que el semáforo evita en este ejemplo es la modificación concurrente de la variable contador. Sin el semáforo, los dos hilos (thread1 y thread2) podrían leer y escribir en contador al mismo tiempo (lo que se observa en la anterior imagen), lo cual generaría resultados incorrectos debido a los siguientes pasos que podrían ocurrir en paralelo:

- Un hilo lee el valor actual de contador.
- Otro hilo también lee el mismo valor de contador antes de que el primer hilo lo modifique.
- Ambos hilos incrementan el valor que leyeron y escriben el nuevo valor en contador.
- Esto provoca que una de las actualizaciones del contador se "pierda". Por ejemplo, si contador es 0, ambos hilos lo leen como 0, ambos lo incrementan y escriben 1 en lugar de llegar a 2, lo cual sería el valor correcto si cada hilo incrementara de manera independiente.

### Ejemplo de semáforo para sincronizar dos hilos
``` C
#include <SDL.h>
#include <stdio.h>

SDL_sem* semaforo;

int tarea1(void* data) {
    printf("Tarea 1 iniciada.\n");
    SDL_Delay(1000); // Simula trabajo
    printf("Tarea 1 completada.\n");
    SDL_SemPost(semaforo); // Señala que terminó la tarea 1
    return 0;
}

int tarea2(void* data) {
    printf("Tarea 2 esperando a que tarea 1 termine.\n");
    SDL_SemWait(semaforo); // Espera hasta que tarea 1 termine
    printf("Tarea 2 iniciada después de tarea 1.\n");
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
```

El resultado de este programa, que es el esperado es:
![alt text](image-6.png)

Si en el ejemplo se comentan todas las líneas que tienen que ver con el semáforo, el resultado es el siguiente:
![alt text](image-5.png)

En este ejemplo, el semáforo se usa para asegurar que el segundo hilo (tarea2) espere hasta que el primer hilo (tarea1) haya completado su trabajo antes de continuar. La sincronización se logra así:
1. El semáforo `semaforo` se inicializa con un valor de 0. Esto significa que `tarea2` no podrá proceder hasta que el semáforo tenga un valor mayor a 0.
2. `tarea1` ejecuta su trabajo y luego llama a `SDL_SemPost(semaforo)`. Esto incrementa el valor del semáforo a 1, desbloqueando cualquier hilo que esté esperando. Este paso indica que `tarea1` ha terminado y que `tarea2` puede avanzar.
3. `tarea2` llama a `SDL_SemWait(semaforo)` al inicio de su ejecución y se bloquea porque el valor inicial del semáforo es 0. `tarea2` queda en espera hasta que `tarea1` complete su trabajo y llame a `SDL_SemPost(semaforo)`, incrementando el valor del semáforo. Cuando el semáforo tiene un valor mayor a 0, `SDL_SemWait(semaforo)` lo decrementa a 0 y permite que `tarea2` continúe.

## Ejercicio 8: ejemplo de referencia para la evaluación

### constants.h
```C
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

#endif //  CONSTANTS_H
```

### Funcionamiento de PlayAudio() y AudioCallback()
**PlayAudio()**
```C
void play_audio(void) {
    // Marca si el dispositivo de audio ya se ha inicializado.
    static uint8_t isaudioDeviceInit = 0;
    // Define la especificación de audio.
    static SDL_AudioSpec audioSpec;
    // ID del dispositivo de audio que se abre con SDL.
    static SDL_AudioDeviceID audioDevice = 0;
    // Contexto que contiene el estado del audio y los datos.
    static AudioContext audioContext;

    // Inicializa el dispositivo de audio solo si aún no ha sido inicializado.
    if (isaudioDeviceInit == 0) {
        // Configuración de la especificación de audio:
        audioSpec.freq = 44100;           // Frecuencia de muestreo en Hz.
        audioSpec.format = AUDIO_S16SYS;  // Formato de audio (16 bits, sistema nativo).
        audioSpec.channels = 1;           // Canal (1 para mono).
        audioSpec.samples = 2048;         // Tamaño de buffer de audio.

        // Define la función de callback y pasa el contexto de audio como 'userdata'.
        audioSpec.callback = AudioCallback;
        audioSpec.userdata = &audioContext;

        // Abre el dispositivo de audio con la especificación dada.
        audioDevice = SDL_OpenAudioDevice(NULL, 0, &audioSpec, NULL, 0);
        // Verifica si el dispositivo de audio se abrió correctamente.
        if (audioDevice == 0) {
            printf("Unable to open audio device: %s\n", SDL_GetError());
        }
        // Marca el dispositivo como inicializado.
        isaudioDeviceInit = 1;
    }

    // Reinicia la posición de audio y marca el audio como no terminado.
    audioContext.audioPosition = 0;
    audioContext.audioFinished = SDL_FALSE;

    // Carga el archivo WAV en 'audioContext', configurando sus datos y longitud.
    if (SDL_LoadWAV("tap.wav", &audioSpec, &audioContext.audioData, &audioContext.audioLength) != NULL) {
        // Comienza la reproducción del audio despausando el dispositivo.
        SDL_PauseAudioDevice(audioDevice, 0);
    } else {
        // Muestra un mensaje si el archivo no pudo cargarse.
        printf("Unable to load WAV file: %s\n", SDL_GetError());
    }

    // Espera hasta que el audio haya terminado de reproducirse.
    while (audioContext.audioFinished != SDL_TRUE) {
        SDL_Delay(100);
    }

    // Informa que el audio ha finalizado.
    printf("Audio finished\n");

    // Cierra el dispositivo de audio.
    SDL_CloseAudioDevice(audioDevice);
    // Libera la memoria usada por el archivo WAV.
    SDL_FreeWAV(audioContext.audioData);
}
```

**AudioCallback()**
```C
void AudioCallback(void* userdata, Uint8* stream, int len) {
    // Convierte el puntero 'userdata' en un puntero a 'AudioContext' para acceder a la estructura de audio.
    AudioContext* audioContext = (AudioContext*)userdata;

    // Si la posición actual de audio supera o iguala la longitud total, marca el audio como finalizado y retorna.
    if (audioContext->audioPosition >= audioContext->audioLength) {
        audioContext->audioFinished = SDL_TRUE;
        return;
    }

    // Calcula cuántos bytes quedan por reproducir en el audio.
    int remainingBytes = audioContext->audioLength - audioContext->audioPosition;
    // Determina cuántos bytes copiar al 'stream' de salida; elige el menor entre 'len' y 'remainingBytes'.
    int bytesToCopy = (len < remainingBytes) ? len : remainingBytes;

    // Copia 'bytesToCopy' desde 'audioData' a la posición actual hacia 'stream'.
    SDL_memcpy(stream, audioContext->audioData + audioContext->audioPosition, bytesToCopy);

    // Actualiza la posición actual en el audio sumando los bytes copiados.
    audioContext->audioPosition += bytesToCopy;
}
```

### Observaciones antes de implementar un hilo adicional
Había un error relacionado con que una función que no debería recibir parámetros estaba recibiendo uno, por lo que no se podía observar el programa en acción. Consulté en ChatGPT y la sugerencia fue cambiar la función `SDL_CloseAudio(audioDevice)`  por `SDL_CloseAudioDevice(audioDevice)` para poder cerrar correctamente un dispositivo de audio específico.

Lo que se puede observar es que al oprimir la tecla p para reproducir el audio, el juego se pausa o bloquea. Además, si se intenta reproducir una segunda vez el audio, el juego se bloquea por completo y no se escucha ningún sonido.

### Problema con el comportamiento bloqueante en la reproducción del audio
La función play_audio está configurada para bloquear el hilo principal hasta que el audio termine de reproducirse, lo que también detiene el ciclo principal del juego mientras espera. Esto ocurre por el bucle `while (audioContext.audioFinished != SDL_TRUE) { SDL_Delay(100); }`, que detiene el juego hasta que se completa el audio.

### Solución mediante un hilo adicional para manejar playAudio
#### Versión 1
``` C
#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "constants.h"
#include <SDL_audio.h>

SDL_sem* semaforo;

typedef struct {
    Uint8* audioData; // Pointer to audio data    
    Uint32 audioLength; // Length of audio data in bytes    
    Uint32 audioPosition; // Current position in audio data    
    SDL_bool audioFinished; //flag set to True whether audio playback has finished
} AudioContext;

void AudioCallback(void* userdata, Uint8* stream, int len) {
    AudioContext* audioContext = (AudioContext*)userdata;

    if (audioContext->audioPosition >= audioContext->audioLength) {
        audioContext->audioFinished = SDL_TRUE;
        return;
    }

    // Calculate the amount of data to copy to the stream    
    int remainingBytes = audioContext->audioLength - audioContext->audioPosition;
    int bytesToCopy = (len < remainingBytes) ? len : remainingBytes;

    // Copy audio data to the stream    
    SDL_memcpy(stream, audioContext->audioData + audioContext->audioPosition, bytesToCopy);

    // Update the audio position    
    audioContext->audioPosition += bytesToCopy;
}


void play_audio(void) {
    static uint8_t isaudioDeviceInit = 0;
    static SDL_AudioSpec audioSpec;
    static SDL_AudioDeviceID audioDevice = 0;
    static AudioContext audioContext;

    if (isaudioDeviceInit == 0) {
        
              /*audioSpec.freq = 44100;
              audioSpec.format = AUDIO_S16SYS;
              audioSpec.channels = 1;
              audioSpec.samples = 2048;*/        

        audioSpec.callback = AudioCallback;
        audioSpec.userdata = &audioContext;

        audioDevice = SDL_OpenAudioDevice(NULL, 0, &audioSpec, NULL, 0);
        if (audioDevice == 0) {
            printf("Unable to open audio device: %s\n", SDL_GetError());
            return 1;
        }
        isaudioDeviceInit = 1;
    }

    audioContext.audioPosition = 0;
    audioContext.audioFinished = SDL_FALSE;
    if (SDL_LoadWAV("tap.wav", &audioSpec, &audioContext.audioData, &audioContext.audioLength) != NULL) {
        SDL_PauseAudioDevice(audioDevice, 0); // Start audio playback
        SDL_SemWait(semaforo);
    }
    else {
        printf("Unable to load WAV file: %s\n", SDL_GetError());
    }
    while (audioContext.audioFinished != SDL_TRUE) {
        SDL_Delay(100);
        SDL_SemPost(semaforo);
    }

    printf("Audio finished\n");
    SDL_CloseAudioDevice(audioDevice); // cambié la función
    SDL_FreeWAV(audioContext.audioData); // Free the loaded WAV data
}


int game_is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int last_frame_time = 0;

struct game_object {
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} ball, paddle;

int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return false;
    }
    return true;
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        game_is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
            game_is_running = false;
        if (event.key.keysym.sym == SDLK_LEFT)
            paddle.vel_x = -400;
        if (event.key.keysym.sym == SDLK_RIGHT)
            paddle.vel_x = +400;
        break;
    case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_LEFT)
            paddle.vel_x = 0;
        if (event.key.keysym.sym == SDLK_RIGHT)
            paddle.vel_x = 0;
        if (event.key.keysym.sym == SDLK_p) {
            SDL_Thread* thread = SDL_CreateThread(play_audio, "HiloAudio", NULL);
            
        }

        break;
    }
}

void setup(void) {
    // Initialize values for the the ball object    
    ball.width = 15;
    ball.height = 15;
    ball.x = 20;
    ball.y = 20;
    ball.vel_x = 300;
    ball.vel_y = 300;

    // Initialize the values for the paddle object    
    paddle.width = 100;
    paddle.height = 20;
    paddle.x = (WINDOW_WIDTH / 2) - (paddle.width / 2);
    paddle.y = WINDOW_HEIGHT - 40;
    paddle.vel_x = 0;
    paddle.vel_y = 0;
}

void update(void) {
    // Calculate how much we have to wait until we reach the target frame time    
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

    // Only delay if we are too fast too update this frame    
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        SDL_Delay(time_to_wait);

    // Get a delta time factor converted to seconds to be used to update my objects    
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;

    // Store the milliseconds of the current frame    
    last_frame_time = SDL_GetTicks();

    // update ball and paddle position    
    ball.x += ball.vel_x * delta_time;
    ball.y += ball.vel_y * delta_time;
    paddle.x += paddle.vel_x * delta_time;
    paddle.y += paddle.vel_y * delta_time;

    // Check for ball collision with the walls    
    if (ball.x <= 0 || ball.x + ball.width >= WINDOW_WIDTH)
        ball.vel_x = -ball.vel_x;
    if (ball.y < 0)
        ball.vel_y = -ball.vel_y;

    // Check for ball collision with the paddle    
    if (ball.y + ball.height >= paddle.y && ball.x + ball.width >= paddle.x && ball.x <= paddle.x + paddle.width)
        ball.vel_y = -ball.vel_y;

    // Prevent paddle from moving outside the boundaries of the window    
    if (paddle.x <= 0)
        paddle.x = 0;
    if (paddle.x >= WINDOW_WIDTH - paddle.width)
        paddle.x = WINDOW_WIDTH - paddle.width;

    // Check for game over    
    if (ball.y + ball.height > WINDOW_HEIGHT) {
        ball.x = WINDOW_WIDTH / 2;
        ball.y = 0;
    }
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw a rectangle for the ball object    
    SDL_Rect ball_rect = {
    (int)ball.x,
    (int)ball.y,
    (int)ball.width,
    (int)ball.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    // Draw a rectangle for the paddle object    
    SDL_Rect paddle_rect = {
    (int)paddle.x,
    (int)paddle.y,
    (int)paddle.width,
    (int)paddle.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle_rect);

    SDL_RenderPresent(renderer);
}

void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* args[]) {

    game_is_running = initialize_window();
    setup();

    semaforo = SDL_CreateSemaphore(1);

    while (game_is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    SDL_DestroySemaphore(semaforo);

    return 0;
}
```

#### Versión funcional
```C
#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "constants.h"
#include <SDL_audio.h>

SDL_sem* semaforo;

typedef struct {
    Uint8* audioData; // Pointer to audio data    
    Uint32 audioLength; // Length of audio data in bytes    
    Uint32 audioPosition; // Current position in audio data    
    SDL_bool audioFinished; //flag set to True whether audio playback has finished
} AudioContext;

void AudioCallback(void* userdata, Uint8* stream, int len) {
    AudioContext* audioContext = (AudioContext*)userdata;

    if (audioContext->audioPosition >= audioContext->audioLength) {
        audioContext->audioFinished = SDL_TRUE;
        return;
    }

    // Calculate the amount of data to copy to the stream    
    int remainingBytes = audioContext->audioLength - audioContext->audioPosition;
    int bytesToCopy = (len < remainingBytes) ? len : remainingBytes;

    // Copy audio data to the stream    
    SDL_memcpy(stream, audioContext->audioData + audioContext->audioPosition, bytesToCopy);

    // Update the audio position    
    audioContext->audioPosition += bytesToCopy;
}


void play_audio(void) {
    static uint8_t isaudioDeviceInit = 0;
    static SDL_AudioSpec audioSpec;
    static SDL_AudioDeviceID audioDevice = 0;
    static AudioContext audioContext;

    if (isaudioDeviceInit == 0) {
        
              /*audioSpec.freq = 44100;
              audioSpec.format = AUDIO_S16SYS;
              audioSpec.channels = 1;
              audioSpec.samples = 2048;*/        

        audioSpec.callback = AudioCallback;
        audioSpec.userdata = &audioContext;

        audioDevice = SDL_OpenAudioDevice(NULL, 0, &audioSpec, NULL, 0);
        if (audioDevice == 0) {
            printf("Unable to open audio device: %s\n", SDL_GetError());
            return 1;
        }
        isaudioDeviceInit = 1;
    }

    audioContext.audioPosition = 0;
    audioContext.audioFinished = SDL_FALSE;
    if (SDL_LoadWAV("tap.wav", &audioSpec, &audioContext.audioData, &audioContext.audioLength) != NULL) {
        SDL_PauseAudioDevice(audioDevice, 0); // Start audio playback
        SDL_SemWait(semaforo);
    }
    else {
        printf("Unable to load WAV file: %s\n", SDL_GetError());
    }
    while (audioContext.audioFinished != SDL_TRUE) {
        SDL_Delay(100);
        SDL_SemPost(semaforo);
    }

    printf("Audio finished\n");
    //SDL_CloseAudioDevice(audioDevice); // cambié la función
    //SDL_FreeWAV(audioContext.audioData); // Free the loaded WAV data
}


int game_is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int last_frame_time = 0;

struct game_object {
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} ball, paddle;

int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return false;
    }
    return true;
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
        game_is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
            game_is_running = false;
        if (event.key.keysym.sym == SDLK_LEFT)
            paddle.vel_x = -400;
        if (event.key.keysym.sym == SDLK_RIGHT)
            paddle.vel_x = +400;
        break;
    case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_LEFT)
            paddle.vel_x = 0;
        if (event.key.keysym.sym == SDLK_RIGHT)
            paddle.vel_x = 0;
        if (event.key.keysym.sym == SDLK_p) {
            SDL_Thread* thread = SDL_CreateThread(play_audio, "HiloAudio", NULL);
            
        }

        break;
    }
}

void setup(void) {
    // Initialize values for the the ball object    
    ball.width = 15;
    ball.height = 15;
    ball.x = 20;
    ball.y = 20;
    ball.vel_x = 300;
    ball.vel_y = 300;

    // Initialize the values for the paddle object    
    paddle.width = 100;
    paddle.height = 20;
    paddle.x = (WINDOW_WIDTH / 2) - (paddle.width / 2);
    paddle.y = WINDOW_HEIGHT - 40;
    paddle.vel_x = 0;
    paddle.vel_y = 0;
}

void update(void) {
    // Calculate how much we have to wait until we reach the target frame time    
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

    // Only delay if we are too fast too update this frame    
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        SDL_Delay(time_to_wait);

    // Get a delta time factor converted to seconds to be used to update my objects    
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;

    // Store the milliseconds of the current frame    
    last_frame_time = SDL_GetTicks();

    // update ball and paddle position    
    ball.x += ball.vel_x * delta_time;
    ball.y += ball.vel_y * delta_time;
    paddle.x += paddle.vel_x * delta_time;
    paddle.y += paddle.vel_y * delta_time;

    // Check for ball collision with the walls    
    if (ball.x <= 0 || ball.x + ball.width >= WINDOW_WIDTH)
        ball.vel_x = -ball.vel_x;
    if (ball.y < 0)
        ball.vel_y = -ball.vel_y;

    // Check for ball collision with the paddle    
    if (ball.y + ball.height >= paddle.y && ball.x + ball.width >= paddle.x && ball.x <= paddle.x + paddle.width)
        ball.vel_y = -ball.vel_y;

    // Prevent paddle from moving outside the boundaries of the window    
    if (paddle.x <= 0)
        paddle.x = 0;
    if (paddle.x >= WINDOW_WIDTH - paddle.width)
        paddle.x = WINDOW_WIDTH - paddle.width;

    // Check for game over    
    if (ball.y + ball.height > WINDOW_HEIGHT) {
        ball.x = WINDOW_WIDTH / 2;
        ball.y = 0;
    }
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw a rectangle for the ball object    
    SDL_Rect ball_rect = {
    (int)ball.x,
    (int)ball.y,
    (int)ball.width,
    (int)ball.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    // Draw a rectangle for the paddle object    
    SDL_Rect paddle_rect = {
    (int)paddle.x,
    (int)paddle.y,
    (int)paddle.width,
    (int)paddle.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle_rect);

    SDL_RenderPresent(renderer);
}

void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* args[]) {

    game_is_running = initialize_window();
    setup();

    semaforo = SDL_CreateSemaphore(1);

    while (game_is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    SDL_DestroySemaphore(semaforo);

    return 0;
}
```