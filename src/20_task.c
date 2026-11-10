#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define IMAGE_PATH "files/my_dog.jpg"

typedef struct {
    SDL_Texture *texture;
    int width;
    int height;
} Image;

int create_image_from_jpg(SDL_Renderer *renderer,
                          Image *image) {
    SDL_Surface *surface = IMG_Load(IMAGE_PATH);

    if (surface == NULL) {
        fprintf(stderr, "Ошибка загрузки изображения\n");
        return 1;
    }

    image->texture = SDL_CreateTextureFromSurface(renderer, surface);
    image->width = surface->w;
    image->height = surface->h;

    if (image->texture == NULL) {
        fprintf(stderr, "Ошибка создания текстуры");
    }

    SDL_FreeSurface(surface);
    return 0;
}

int main_process() {
    int result = 0;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    Image image = {0};

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Ошибка инициализации SDL\n");
        goto error;
    }

    if ((!IMG_Init(IMG_INIT_JPG)) & IMG_INIT_JPG) {
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

    if (create_image_from_jpg(renderer, &image) != 0)
        goto error;

    bool is_running = true;
    SDL_Event event;

    while (is_running) {
        // Обработка
    }

    result = 0;

error:
    if (window)
        SDL_DestroyWindow(window);
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (image.texture != NULL)
        SDL_DestroyTexture(image.texture);

    IMG_Quit();
    SDL_Quit();

    return result;
}

int main(int argc,
         char *argv[]) {
    return main_process();
}
