#include <stdio.h>
#include <SDL2/SDL.h>

#define HANDMADE_MATH_IMPLEMENTATION
#include "HandmadeMath.h"

#include <assert.h>
#include <cstring>

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

//Scope exit from http://the-witness.net/news/2012/11/scopeexit-in-c11/
template <typename F>
struct ScopeExit {
    ScopeExit(F f) : f(f) {}
    ~ScopeExit() { f(); }
    F f;
};
template <typename F>
ScopeExit<F> MakeScopeExit(F f) {
    return ScopeExit<F>(f);
};
#define CONCAT_IMPL(arg1, arg2) arg1 ## arg2
#define CONCAT(arg1, arg2) CONCAT_IMPL(arg1, arg2)
#define atScopeExit(code) \
    auto CONCAT(scope_exit_, __LINE__) = MakeScopeExit([=](){code;})

template<typename T>
static inline T*
allocate(size_t count) {
    return (T*)calloc(count, sizeof(T));
}

const int WIDTH  = 640;
const int HEIGHT = 480;

struct Color32 {
    u32 value;
};

typedef hmm_vec3 Color;

static inline hmm_vec3
makeVec3(f32 a, f32 b, f32 c) {
    return HMM_Vec3(a, b, c);
}

static inline Color
makeColor(f32 r, f32 g, f32 b) {
    return makeVec3(r, g, b);
}

static inline Color32
makeColor32(u8 r, u8 g, u8 b, u8 a = 255) {
    Color32 result;
    result.value = (r << 24) + (g << 16) + (b << 8) + (a << 0);
    return result;
}

static inline Color32
makeColor32(Color color) {
    Color32 result;
    result = makeColor32(color.R * 255, color.G * 255, color.B * 255);
    return result;
}

static inline void
setPixelColor(Color32* pixels, i32 x, i32 y, Color color) {
    Color32 color32 = makeColor32(color);
    pixels[y * WIDTH + x] = color32;
}

static void
renderPixels(Color32* pixels) {
    memset(pixels, 0, sizeof(Color32) * WIDTH * HEIGHT);

    for(i32 y = 0; y < HEIGHT; y++) {
        for(i32 x = 0; x < WIDTH; x++) {
            Color color = makeColor((float)x / WIDTH, (float)y / HEIGHT, 0.2f);
            setPixelColor(pixels, x, y, color);
        }
    }
}

int
backgroundRenderThread(void* pixels) {
    renderPixels((Color32*)pixels);
    return 0;
}

int
main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        const char* error = SDL_GetError();
        assert("SDL_Error" == error);
        return -1;
    }
    atScopeExit(SDL_Quit());

    SDL_Window* window = SDL_CreateWindow(
        "roju_tracer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        0);
    atScopeExit(SDL_DestroyWindow(window));

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);

    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);

    if (!window || !renderer || !texture) {
        const char* error = SDL_GetError();
        printf("%s\n", error);
        assert("SDL_Error" == error);
        return -1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    Color32* pixels = allocate<Color32>(WIDTH * HEIGHT);

    SDL_Thread* renderThread = SDL_CreateThread(backgroundRenderThread, "backgroundRenderThread", (void *)pixels);

    b32 running = true;
    while (running) {

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
                    }
                    break;
                }
            }
        }

        SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(Color32));

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    return 0;
}
