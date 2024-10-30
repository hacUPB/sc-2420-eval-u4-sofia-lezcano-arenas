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