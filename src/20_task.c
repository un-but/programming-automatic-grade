#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH  1000
#define SCREEN_HEIGHT 1000
#define IMAGE_WIDTH   200
#define IMAGE_HEIGHT  200

#define IMAGE_PATH "files/my_dog.jpg"

int create_image_from_jpg(SDL_Renderer *renderer,
                          SDL_Texture **texture) {
    SDL_Surface *surface = IMG_Load(IMAGE_PATH);

    if (surface == NULL) {
        fprintf(stderr, "Ошибка загрузки изображения\n");
        return 1;
    }

    *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture == NULL) {
        fprintf(stderr, "Ошибка создания текстуры");
        return 1;
    }

    SDL_FreeSurface(surface);
    return 0;
}

int main_process() {
    int result = 1;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Ошибка инициализации SDL\n");
        goto error;
    }

    if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
        fprintf(stderr, "Ошибка инициализации SDL_image\n");
        goto error;
    }

    window = SDL_CreateWindow("Прыгающая собака",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH,
                              SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);

    if (window == NULL) {
        fprintf(stderr, "Ошибка создания окна\n");
        goto error;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        fprintf(stderr, "Ошибка рендера\n");
        goto error;
    }

    if (create_image_from_jpg(renderer, &texture) != 0)
        goto error;

    bool is_running = true;
    SDL_Event event;

    int cur_x = 100, cur_y = 100;
    int vel_x = 5, vel_y = 4;

    while (is_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
                is_running = false;
        }

        SDL_SetRenderDrawColor(renderer, 34, 34, 34, 255);
        SDL_RenderClear(renderer);

        cur_x += vel_x;
        cur_y += vel_y;

        if (cur_x <= 0) {
            cur_x = 0;
            vel_x = -vel_x;
        }

        if (cur_x + IMAGE_WIDTH >= SCREEN_WIDTH) {
            cur_x = SCREEN_WIDTH - IMAGE_WIDTH;
            vel_x = -vel_x;
        }

        if (cur_y <= 0) {
            cur_y = 0;
            vel_y = -vel_y;
        }

        if (cur_y + IMAGE_HEIGHT >= SCREEN_HEIGHT) {
            cur_y = SCREEN_HEIGHT - IMAGE_HEIGHT;
            vel_y = -vel_y;
        }

        SDL_Rect dest_rect = {cur_x, cur_y, IMAGE_WIDTH, IMAGE_HEIGHT};
        SDL_RenderCopy(renderer, texture, NULL, &dest_rect);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    result = 0;

error:
    if (window)
        SDL_DestroyWindow(window);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (texture != NULL)
        SDL_DestroyTexture(texture);

    IMG_Quit();
    SDL_Quit();

    return result;
}

int main() {
    return main_process();
}
