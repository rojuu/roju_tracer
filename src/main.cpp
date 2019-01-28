#include <stdio.h>
#include <SDL2/SDL.h>

#define HANDMADE_MATH_IMPLEMENTATION
#include "HandmadeMath.h"

#include <assert.h>

#include <stdint.h>
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;

const int WIDTH  = 640;
const int HEIGHT = 480;

struct Color {
    union {
        u32 value;
        struct {
            u8 a, b, g, r;
        };
    };
};

static inline Color
color(u8 r, u8 g, u8 b, u8 a = 255) {
    Color result;
    result.r = r;
    result.g = g;
    result.b = b;
    result.a = a;
    return result;
}

static inline void
setPixelColor(Color* pixels, i32 x, i32 y, Color color) {
    pixels[y * WIDTH + x] = color;
}

static void
renderPixels(Color* pixels) {
    memset(pixels, 0, sizeof(Color) * WIDTH * HEIGHT);

    hmm_vec2 center = HMM_Vec2((f32)WIDTH/2.f, (f32)HEIGHT/2.f);
    for(i32 x = 0; x < WIDTH; x++) {
        for(i32 y = 0; y < HEIGHT; y++) {
            Color white = color(255, 255, 255);
            Color green = color(0, 255, 0);
            f32 radius = (f32)HEIGHT/2 * 0.33f;
            hmm_vec2 point = HMM_Vec2(x, y);
            if(HMM_Length(center - point) < radius) {
                setPixelColor(pixels, x, y, green);
            }
        }
    }
}

int
main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        const char* error = SDL_GetError();
        assert("SDL_Error" == error);
        return -1;
    }
    atexit(SDL_Quit);

    char* windowTitle = (char*)"roju_tracer";
    SDL_Window* window = SDL_CreateWindow(
        windowTitle,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);

    if (!window || !renderer || !texture) {
        const char* error = SDL_GetError();
        assert("SDL_Error" == error);
        return -1;
    }

    Color* pixels = (Color*)malloc(sizeof(Color) * WIDTH * HEIGHT);

    b32 running = true;
    f64 currentTime = (f64)SDL_GetPerformanceCounter() /
                       (f64)SDL_GetPerformanceFrequency();
    f64 lastTime = 0;
    f64 deltaTime = 0;
    while (running) {
        lastTime = currentTime;
        currentTime =  (f64)SDL_GetPerformanceCounter() /
                       (f64)SDL_GetPerformanceFrequency();
        deltaTime = (f64)(currentTime - lastTime);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    running = false;
                    break;
                }

                case SDL_KEYDOWN: {
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE: {
                            running = false;
                            break;
                        }

                        // Do the rendering thing
                        case SDLK_RETURN: {
                            SDL_SetWindowTitle(window, "Rendering");
                            renderPixels(pixels);

                            SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(Color));

                            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                            SDL_RenderClear(renderer);
                            SDL_RenderCopy(renderer, texture, NULL, NULL);
                            SDL_RenderPresent(renderer);

                            SDL_SetWindowTitle(window, windowTitle);
                        }
                    }
                    break;
                }
            }
        }
    }

    SDL_DestroyWindow(window);
}
