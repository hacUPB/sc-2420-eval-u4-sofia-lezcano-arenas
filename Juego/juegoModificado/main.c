#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define FPS 30
#define FRAME_TIME (1000/FPS)

//punteros
SDL_Window* window = NULL; //se crea una ventana
SDL_Renderer* renderer = NULL; //se crea un renderer para poder renderizar cosas en la ventana

//variables
int game_is_running = false;
int last_frame_time = 0;
int empezar = 0;
int teclaEspacio = 0;

int rectX, rectY, rectW, rectH, rectV;

struct linea {
    int x1;
    int y1;
    int x2;
    int y2;
    int width;
    int height;
} lh1, lh2, lh3, lh4, lh5, lh6, lh7, lh8, lv1, lv2, lv3, lv4, lv5, lv6, lv7, lv8, meta;

int estadoChoco1, estadoChoco2, estadoChoco3, estadoChoco4; //si está en 0, no han colisionado con él, si está en 1 ya colisionaron con él y debe dejar de dibujarse
int puntajeChoco1, puntajeChoco2, puntajeChoco3, puntajeChoco4; //si está en 0, no han colisionado con él, si está en 1 ya colisionaron con él y debe dejar de dibujarse
int puntaje = 0;
int indicadorPuntaje1, indicadorPuntaje2, indicadorPuntaje3, indicadorPuntaje4; // si está en cero, no se ha sumado su puntaje, si está en 1, ya se sumó

int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false; //el condicional nos ayuda a verificar si hay errores en la inicialización de SDL
    }
    window = SDL_CreateWindow(
        "A simple game loop using C & SDL",
        SDL_WINDOWPOS_CENTERED, //posición en x
        SDL_WINDOWPOS_CENTERED, //posición en y
        WINDOW_WIDTH, //ancho de la ventana
        WINDOW_HEIGHT, //alto de la ventana
        SDL_WINDOW_SHOWN //------------------------- este era el problema para la visualización de la ventana
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false; //el condicional nos ayuda a verificar si hay errores en la creación de la ventana
    }
    renderer = SDL_CreateRenderer(window, -1, 0); //(ventana a donde estará atado el rederer, -1 es para usar el display driver por defecto, banderas especiales para el render)
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return false; //el condicional nos ayuda a verificar si hay errores en la creación del renderer
    }
    return true;
}



void setup(void)
{
    //Pista
    lh1.x1 = 0; lh1.y1 = 500; lh1.x2 = 150; lh1.y2 = 500; lh1.width = lh1.x2 - lh1.x1; lh1.height = lh1.y2 - lh1.y1;

    lh2.x1 = 150; lh2.y1 = 400; lh2.x2 = 250; lh2.y2 = 400; lh2.width = lh2.x2 - lh2.x1; lh2.height = lh2.y2 - lh2.y1;

    lh3.x1 = 250; lh3.y1 = 100; lh3.x2 = 500; lh3.y2 = 100; lh3.width = lh3.x2 - lh3.x1; lh3.height = lh3.y2 - lh3.y1;

    lh4.x1 = 500; lh4.y1 = 450; lh4.x2 = 550; lh4.y2 = 450; lh4.width = lh4.x2 - lh4.x1; lh4.height = lh4.y2 - lh4.y1;

    lh5.x1 = 650; lh5.y1 = 100; lh5.x2 = 800; lh5.y2 = 100; lh5.width = lh5.x2 - lh5.x1; lh5.height = lh5.y2 - lh5.y1;

    lh6.x1 = 400; lh6.y1 = 550; lh6.x2 = 650; lh6.y2 = 550; lh6.width = lh6.x2 - lh6.x1; lh6.height = lh6.y2 - lh6.y1;

    lh7.x1 = 350; lh7.y1 = 200; lh7.x2 = 400; lh7.y2 = 200; lh7.width = lh7.x2 - lh7.x1; lh7.height = lh7.y2 - lh7.y1;

    lh8.x1 = 250; lh8.y1 = 500; lh8.x2 = 350; lh8.y2 = 500; lh8.width = lh8.x2 - lh8.x1; lh8.height = lh8.y2 - lh8.y1;

    lv1.x1 = 150; lv1.y1 = 500; lv1.x2 = 150; lv1.y2 = 400; lv1.width = lv1.x2 - lv1.x1; lv1.height = lv1.y2 - lv1.y1;

    lv2.x1 = 250; lv2.y1 = 400; lv2.x2 = 250; lv2.y2 = 100; lv2.width = lv2.x2 - lv2.x1; lv2.height = lv2.y2 - lv2.y1;

    lv3.x1 = 500; lv3.y1 = 100; lv3.x2 = 500; lv3.y2 = 450; lv3.width = lv3.x2 - lv3.x1; lv3.height = lv3.y2 - lv3.y1;

    lv4.x1 = 550; lv4.y1 = 450; lv4.x2 = 550; lv4.y2 = 0; lv4.width = lv4.x2 - lv4.x1; lv4.height = lv4.y2 - lv4.y1;

    lv5.x1 = 650; lv5.y1 = 100; lv5.x2 = 650; lv5.y2 =550; lv5.width = lv5.x2 - lv5.x1; lv5.height = lv5.y2 - lv5.y1;

    lv6.x1 = 400; lv6.y1 = 550; lv6.x2 = 400; lv6.y2 = 200; lv6.width = lv6.x2 - lv6.x1; lv6.height = lv6.y2 - lv6.y1;

    lv7.x1 = 350; lv7.y1 = 200; lv7.x2 = 350; lv7.y2 = 500; lv7.width = lv7.x2 - lv7.x1; lv7.height = lv7.y2 - lv7.y1;

    lv8.x1 = 250; lv8.y1 = 500; lv8.x2 = 250; lv8.y2 = 600; lv8.width = lv8.x2 - lv8.x1; lv8.height = lv8.y2 - lv8.y1;

    //Meta
    meta.x1 = 700; meta.y1 = 0; meta.x2 = 700; meta.y2 = 100; meta.width = meta.x2 - meta.x1; meta.height = meta.y2 - meta.y1;

    //Cuadrado
    rectX = 50;
    rectY = 540;
    rectW = 30;
    rectH = 30;
    rectV = 120;

    //Estado chocolates
    estadoChoco1 = 0;
    estadoChoco2 = 0;
    estadoChoco3 = 0;
    estadoChoco4 = 0;

    //Puntaje chocolates
    puntajeChoco1 = 10;
    puntajeChoco2 = 20;
    puntajeChoco3 = 30;
    puntajeChoco4 = 40;

    //Indicadores puntaje sumado
    indicadorPuntaje1 = 0;
    indicadorPuntaje2 = 0;
    indicadorPuntaje3 = 0;
    indicadorPuntaje4 = 0;
}

void update(void)
{
    int time_to_wait = FRAME_TIME - (SDL_GetTicks() - last_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TIME)
    {
        SDL_Delay(time_to_wait);
    }

    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f; // Get delta_time factor converted to seconds to be used to update objects

    last_frame_time = SDL_GetTicks(); // Store the milliseconds of the current frame to be used in the next one

    SDL_Event event; //se crea un evento SDL llamado evento
    while (SDL_PollEvent(&event)) //mientras que haya un evento
    {
        switch (event.type)
        {
        case SDL_QUIT: // si se hace clic en la x de la ventana o alt+4. Nota: aquí no funciona automáticamente
            game_is_running = false; //el game loop se detendrá
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) //si se presiona Escape
            {
                game_is_running = false; //el game loop se detendrá
            }
            if (event.key.keysym.sym == SDLK_SPACE && teclaEspacio == 0)
            {
                teclaEspacio = 1;
                empezar = 1;
            }
            if (event.key.keysym.sym == SDLK_UP && empezar != 0)
            {
                empezar = 2;
            }
            if (event.key.keysym.sym == SDLK_DOWN && empezar != 0)
            {
                empezar = 3;
            }
            if (event.key.keysym.sym == SDLK_RIGHT && empezar != 0)
            {
                empezar = 4;
            }
            if (event.key.keysym.sym == SDLK_r) //si se presiona Escape
            {
                teclaEspacio = 0;
                empezar = 0;
                //reestablecer posición inicial cuadrado
                rectX = 50;
                rectY = 540;
                rectW = 30;
                rectH = 30;
                rectV = 120;
                //reestablecer chocolates
                estadoChoco1 = 0;
                estadoChoco2 = 0;
                estadoChoco3 = 0;
                estadoChoco4 = 0;
                //reestablecer puntaje
                puntaje = 0;
                //reestablecer indicadores de puntaje
                indicadorPuntaje1 = 0;
                indicadorPuntaje2 = 0;
                indicadorPuntaje3 = 0;
                indicadorPuntaje4 = 0;
            }
            break;
        }
    }

    //Movimiento cuadrado
    if (empezar == 1)
    {
        rectX += (rectV * delta_time);
    }
    if (teclaEspacio == 1 && empezar == 2)
    {
        rectY -= (rectV * delta_time);
    }
    if (teclaEspacio == 1 && empezar == 3)
    {
        rectY += (rectV * delta_time);
    }
    if (teclaEspacio == 1 && empezar == 4)
    {
        rectX += (rectV * delta_time);
    }

    //Detectar la meta
    if (rectX > (meta.x1 + 10) && rectY > meta.y1 && rectY < (meta.y2 - rectH) && puntaje == 100)
    {
        empezar = 0;
    }
    else
    {
        if (puntaje < 100 && (rectX + rectW) > (meta.x1 - 5))
        {
            empezar = 0;
        }
    }
    

    //Colisiones
    //lh1
    if (rectX >= lh1.x1 && rectX <= lh1.x2 && rectY <= lh1.y1 )
    {
        rectX = 50;
        rectY = 540;
        empezar = 0;
        teclaEspacio = 0;
        estadoChoco1 = 0;
        estadoChoco2 = 0;
        estadoChoco3 = 0;
        estadoChoco4 = 0;
        puntaje = 0;
        indicadorPuntaje1 = 0;
        indicadorPuntaje2 = 0;
        indicadorPuntaje3 = 0;
        indicadorPuntaje4 = 0;
    }
    //lh2
    if (rectX >= lh2.x1 && rectX <= lh2.x2 && rectY <= lh2.y1)
    {
        rectX = 50;
        rectY = 540;
        empezar = 0;
        teclaEspacio = 0;
        estadoChoco1 = 0;
        estadoChoco2 = 0;
        estadoChoco3 = 0;
        estadoChoco4 = 0;
        puntaje = 0;
        indicadorPuntaje1 = 0;
        indicadorPuntaje2 = 0;
        indicadorPuntaje3 = 0;
        indicadorPuntaje4 = 0;
    }
    //lh3
    if (rectX >= lh3.x1 && rectX <= lh3.x2 && rectY <= lh3.y1)
    {
        rectX = 50;
        rectY = 540;
        empezar = 0;
        teclaEspacio = 0;
        estadoChoco1 = 0;
        estadoChoco2 = 0;
        estadoChoco3 = 0;
        estadoChoco4 = 0;
        puntaje = 0;
        indicadorPuntaje1 = 0;
        indicadorPuntaje2 = 0;
        indicadorPuntaje3 = 0;
        indicadorPuntaje4 = 0;
    }
    //lh4
    if ((rectX + rectW) >= lh4.x1 && rectX <= lh4.x2 && rectY <= lh4.y1)
    {
        rectX = 50;
        rectY = 540;
        empezar = 0;
        teclaEspacio = 0;
        estadoChoco1 = 0;
        estadoChoco2 = 0;
        estadoChoco3 = 0;
        estadoChoco4 = 0;
        puntaje = 0;
        indicadorPuntaje1 = 0;
        indicadorPuntaje2 = 0;
        indicadorPuntaje3 = 0;
        indicadorPuntaje4 = 0;
    }
    //lh5
    if ((rectX + rectW) >= lh5.x1 && rectX <= lh5.x2 && (rectY + rectH) >= lh5.y1)
    {
        rectX = 50;
        rectY = 540;
        empezar = 0;
        teclaEspacio = 0;
        estadoChoco1 = 0;
        estadoChoco2 = 0;
        estadoChoco3 = 0;
        estadoChoco4 = 0;
        puntaje = 0;
        indicadorPuntaje1 = 0;
        indicadorPuntaje2 = 0;
        indicadorPuntaje3 = 0;
        indicadorPuntaje4 = 0;
    }
    //lh6
    if ((rectX + rectW) >= lh6.x1 && rectX <= lh6.x2 && (rectY + rectH) >= lh6.y1)
    {
        rectX = 50;
        rectY = 540;
        empezar = 0;
        teclaEspacio = 0;
        estadoChoco1 = 0;
        estadoChoco2 = 0;
        estadoChoco3 = 0;
        estadoChoco4 = 0;
        puntaje = 0;
        indicadorPuntaje1 = 0;
        indicadorPuntaje2 = 0;
        indicadorPuntaje3 = 0;
        indicadorPuntaje4 = 0;
    }
    //lh7
    if ((rectX + rectW) >= lh7.x1 && rectX <= lh7.x2 && (rectY + rectH) >= lh7.y1)
    {
        rectX = 50;
        rectY = 540;
        empezar = 0;
        teclaEspacio = 0;
        estadoChoco1 = 0;
        estadoChoco2 = 0;
        estadoChoco3 = 0;
        estadoChoco4 = 0;
        puntaje = 0;
        indicadorPuntaje1 = 0;
        indicadorPuntaje2 = 0;
        indicadorPuntaje3 = 0;
        indicadorPuntaje4 = 0;
    }
    //lh8
    if ((rectX + rectW) >= lh8.x1 && rectX <= lh8.x2 && (rectY + rectH) >= lh8.y1)
    {
        rectX = 50;
        rectY = 540;
        empezar = 0;
        teclaEspacio = 0;
        estadoChoco1 = 0;
        estadoChoco2 = 0;
        estadoChoco3 = 0;
        estadoChoco4 = 0;
        puntaje = 0;
        indicadorPuntaje1 = 0;
        indicadorPuntaje2 = 0;
        indicadorPuntaje3 = 0;
        indicadorPuntaje4 = 0;
    }

    //colisiones chocolates
    if ((rectX + rectW) >= 200 && rectY <= 475 && rectX <= 225 && (rectY + rectH) >= 450)
    {
        estadoChoco1 = 1;
    }
    if ((rectX + rectW) >= 310 && rectY <= 220 && rectX <= 330 && (rectY + rectH) >= 200)
    {
        estadoChoco2 = 1;
    }
    if ((rectX + rectW) >= 410 && (rectY + rectH) >= 520 && rectX <= 425)
    {
        estadoChoco3 = 1;
    }
    if ((rectX + rectW) >= 555 && rectY <= 310 && rectX <= 565 && (rectY + rectH) >= 300)
    {
        estadoChoco4 = 1;
    }
    //puntaje
    if (estadoChoco1 == 1 && indicadorPuntaje1 == 0)
    {
        puntaje += puntajeChoco1;
        indicadorPuntaje1 = 1;
    }
    if (estadoChoco2 == 1 && indicadorPuntaje2 == 0)
    {
        puntaje += puntajeChoco2;
        indicadorPuntaje2 = 1;
    }
    if (estadoChoco3 == 1 && indicadorPuntaje3 == 0)
    {
        puntaje += puntajeChoco3;
        indicadorPuntaje3 = 1;
    }
    if (estadoChoco4 == 1 && indicadorPuntaje4 == 0)
    {
        puntaje += puntajeChoco4;
        indicadorPuntaje4 = 1;
    }
    printf("puntaje: %i", puntaje);
}

void render(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // (R, G, B, alpha) para definir el color a usar en lo próximo a dibujar
    SDL_RenderClear(renderer);

    //Dibujo pista
    SDL_SetRenderDrawColor(renderer, 0, 100, 200, 255);
    SDL_RenderDrawLine(renderer, lh1.x1, lh1.y1, lh1.x2, lh1.y2); //(renderer, x1, y1, x2, y2)
    SDL_RenderDrawLine(renderer, lh2.x1, lh2.y1, lh2.x2, lh2.y2);
    SDL_RenderDrawLine(renderer, lh3.x1, lh3.y1, lh3.x2, lh3.y2);
    SDL_RenderDrawLine(renderer, lh4.x1, lh4.y1, lh4.x2, lh4.y2);
    SDL_RenderDrawLine(renderer, lh5.x1, lh5.y1, lh5.x2, lh5.y2);
    SDL_RenderDrawLine(renderer, lh6.x1, lh6.y1, lh6.x2, lh6.y2);
    SDL_RenderDrawLine(renderer, lh7.x1, lh7.y1, lh7.x2, lh7.y2);
    SDL_RenderDrawLine(renderer, lh8.x1, lh8.y1, lh8.x2, lh8.y2);
    SDL_RenderDrawLine(renderer, lv1.x1, lv1.y1, lv1.x2, lv1.y2);
    SDL_RenderDrawLine(renderer, lv2.x1, lv2.y1, lv2.x2, lv2.y2);
    SDL_RenderDrawLine(renderer, lv3.x1, lv3.y1, lv3.x2, lv3.y2);
    SDL_RenderDrawLine(renderer, lv4.x1, lv4.y1, lv4.x2, lv4.y2);
    SDL_RenderDrawLine(renderer, lv5.x1, lv5.y1, lv5.x2, lv5.y2);
    SDL_RenderDrawLine(renderer, lv6.x1, lv6.y1, lv6.x2, lv6.y2);
    SDL_RenderDrawLine(renderer, lv7.x1, lv7.y1, lv7.x2, lv7.y2);
    SDL_RenderDrawLine(renderer, lv8.x1, lv8.y1, lv8.x2, lv8.y2);
    //Dibujo meta
    SDL_SetRenderDrawColor(renderer, 50, 200, 100, 255);
    SDL_RenderDrawLine(renderer, meta.x1, meta.y1, meta.x2, meta.y2);
    // Rectángulo
    SDL_SetRenderDrawColor(renderer, 200, 0, 100, 255);
    SDL_Rect rect =
    {
        rectX, //posición en x
        rectY, //posición en y
        rectW, //ancho
        rectH //alto
    };
    SDL_RenderDrawRect(renderer, &rect); // si se quiere relleno, puede usarse SDL_RenderFillRect(renderer, referencia)
    //Dibujo chocolates
    //choco1
    if (estadoChoco1 == 0)
    {
        SDL_SetRenderDrawColor(renderer, 74, 47, 12, 255);
        SDL_Rect choco1 =
        {
            200, //posición en x
            450, //posición en y
            25, //ancho
            25 //alto
        };
        SDL_RenderFillRect(renderer, &choco1);
    }
    //choco2
    if (estadoChoco2 == 0)
    {
        SDL_SetRenderDrawColor(renderer, 74, 47, 12, 255);
        SDL_Rect choco2 =
        {
            310, //posición en x
            200, //posición en y
            20, //ancho
            20 //alto
        };
        SDL_RenderFillRect(renderer, &choco2);
    }
    //choco3
    if (estadoChoco3 == 0)
    {
        SDL_SetRenderDrawColor(renderer, 74, 47, 12, 255);
        SDL_Rect choco3 =
        {
            410, //posición en x
            520, //posición en y
            15, //ancho
            15 //alto
        };
        SDL_RenderFillRect(renderer, &choco3);
    }
    //choco4
    if (estadoChoco4 == 0)
    {
        SDL_SetRenderDrawColor(renderer, 74, 47, 12, 255);
        SDL_Rect choco4 =
        {
            555, //posición en x
            300, //posición en y
            10, //ancho
            10 //alto
        };
        SDL_RenderFillRect(renderer, &choco4);
    }

    SDL_RenderPresent(renderer); //cambio de buffer para evitar efecto glitch
}

void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* args[]) {
    game_is_running = initialize_window();

    setup();

    while (game_is_running) {
        update();
        render();
    }

    destroy_window();

    return 0;
}