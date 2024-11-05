#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "constants.h"
#include <SDL_audio.h>
#include <SDL_thread.h>

// Declaración del semáforo
SDL_sem* audioSemaphore;

typedef struct {
    Uint8* audioData;          // Puntero a los datos de audio
    Uint32 audioLength;        // Longitud de los datos de audio en bytes
    Uint32 audioPosition;      // Posición actual en los datos de audio
} AudioContext;

void AudioCallback(void* userdata, Uint8* stream, int len) {
    AudioContext* audioContext = (AudioContext*)userdata;

    if (audioContext->audioPosition >= audioContext->audioLength) {
        // Libera el semáforo cuando el audio termina
        SDL_SemPost(audioSemaphore);
        return;
    }

    int remainingBytes = audioContext->audioLength - audioContext->audioPosition;
    int bytesToCopy = (len < remainingBytes) ? len : remainingBytes;

    // Copia los datos de audio al stream de salida
    SDL_memcpy(stream, audioContext->audioData + audioContext->audioPosition, bytesToCopy);
    audioContext->audioPosition += bytesToCopy;
}

int play_audio(void* arg) {
    static uint8_t isaudioDeviceInit = 0;
    static SDL_AudioSpec audioSpec;
    static SDL_AudioDeviceID audioDevice = 0;
    static AudioContext audioContext;

    if (!isaudioDeviceInit) {
        audioSpec.callback = AudioCallback;
        audioSpec.userdata = &audioContext;

        // Inicializa el dispositivo de audio
        audioDevice = SDL_OpenAudioDevice(NULL, 0, &audioSpec, NULL, 0);
        if (audioDevice == 0) {
            printf("No se pudo abrir el dispositivo de audio: %s\n", SDL_GetError());
            return 1;
        }
        isaudioDeviceInit = 1;
    }

    if (SDL_LoadWAV("tap.wav", &audioSpec, &audioContext.audioData, &audioContext.audioLength) != NULL) {
        audioContext.audioPosition = 0;
        SDL_PauseAudioDevice(audioDevice, 0); // Inicia la reproducción de audio

        // Espera a que el audio termine antes de salir de la función
        SDL_SemWait(audioSemaphore);

        // Libera el archivo de audio una vez que termina
        SDL_FreeWAV(audioContext.audioData);
    }
    else {
        printf("No se pudo cargar el archivo WAV: %s\n", SDL_GetError());
    }

    return 0;
}

int game_is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int last_frame_time = 0;

// Objetos del juego: bola y plataforma
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
        fprintf(stderr, "Error al inicializar SDL.\n");
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
        fprintf(stderr, "Error al crear la ventana SDL.\n");
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error al crear el renderizador SDL.\n");
        return false;
    }

    // Inicializa el semáforo con un valor inicial de 1
    audioSemaphore = SDL_CreateSemaphore(1);

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
            paddle.vel_x = 400;
        break;
    case SDL_KEYUP:
        if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_RIGHT)
            paddle.vel_x = 0;
        if (event.key.keysym.sym == SDLK_p) {
            // Intenta bloquear el semáforo antes de reproducir el audio
            if (SDL_SemTryWait(audioSemaphore) == 0) {
                SDL_Thread* thread = SDL_CreateThread(play_audio, "HiloAudio", NULL);
                SDL_DetachThread(thread);
            }
        }
        break;
    }
}

void setup(void) {
    ball.width = 15;
    ball.height = 15;
    ball.x = 20;
    ball.y = 20;
    ball.vel_x = 300;
    ball.vel_y = 300;

    paddle.width = 100;
    paddle.height = 20;
    paddle.x = (WINDOW_WIDTH / 2) - (paddle.width / 2);
    paddle.y = WINDOW_HEIGHT - 40;
    paddle.vel_x = 0;
    paddle.vel_y = 0;
}

void update(void) {
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        SDL_Delay(time_to_wait);

    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;
    last_frame_time = SDL_GetTicks();

    ball.x += ball.vel_x * delta_time;
    ball.y += ball.vel_y * delta_time;
    paddle.x += paddle.vel_x * delta_time;
    paddle.y += paddle.vel_y * delta_time;

    if (ball.x <= 0 || ball.x + ball.width >= WINDOW_WIDTH)
        ball.vel_x = -ball.vel_x;
    if (ball.y < 0)
        ball.vel_y = -ball.vel_y;

    if (ball.y + ball.height >= paddle.y && ball.x + ball.width >= paddle.x && ball.x <= paddle.x + paddle.width)
        ball.vel_y = -ball.vel_y;

    if (paddle.x <= 0) paddle.x = 0;
    if (paddle.x >= WINDOW_WIDTH - paddle.width) paddle.x = WINDOW_WIDTH - paddle.width;

    if (ball.y + ball.height > WINDOW_HEIGHT) {
        ball.x = WINDOW_WIDTH / 2;
        ball.y = 0;
    }
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect ball_rect = { (int)ball.x, (int)ball.y, (int)ball.width, (int)ball.height };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    SDL_Rect paddle_rect = { (int)paddle.x, (int)paddle.y, (int)paddle.width, (int)paddle.height };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle_rect);

    SDL_RenderPresent(renderer);
}

void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroySemaphore(audioSemaphore); // Destruye el semáforo
    SDL_Quit();
}

int main(int argc, char* args[]) {
    game_is_running = initialize_window();
    setup();

    while (game_is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    return 0;
}
