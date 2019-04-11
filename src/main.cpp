#include <stdio.h>
#include <SDL2/SDL.h>

#include <atomic>
#include <random>

#include <assert.h>
#include <cstring>

#include "HandmadeMath.cpp"
#include "stb_image_write.cpp"
#include "pcg_random.hpp"

#include "config.cpp"
#include "types.cpp"
#include "math.cpp"
#include "hittables.cpp"

struct Camera {
    Vec3 lowerLeftCorner;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 origin;

    Camera() {
        f32 w = (f32)WIDTH  / 100.f;
        f32 h = (f32)HEIGHT / 100.f;

        lowerLeftCorner = vec3(-w, -h, -1);
        horizontal = vec3( w*2, 0, 0);
        vertical = vec3(0, h*2, 0);
        origin = vec3(0, 0, 0);
    }

    Ray getRay(f32 u, f32 v) {
        Ray ray = Ray(origin, lowerLeftCorner + u*horizontal + v*vertical);
        return ray;
    }
};
static Camera camera;

static Color
colorFromRay(Ray& ray, Hittable* world) {
    HitInfo info;
    if (world->hit(ray, 0.001, MAXFLOAT, info)) {
        Vec3 target = info.point + info.normal + randomInUnitSphere();
        Ray ray = Ray(info.point, target-info.point);
        return 0.5f*colorFromRay(ray, world);
    } else {
        Vec3 unitDirection = HMM_FastNormalize(ray.d);
        f32 t = 0.5f * (unitDirection.y + 1.0f);
        return (1.0f-t)*vec3(1.0f, 1.0f, 1.0f) + t*vec3(0.5f, 0.7f, 1.0f);
    }
}

static void
renderPixels(Color32* pixels) {
    memset(pixels, 0, sizeof(Color32) * WIDTH * HEIGHT);

    Hittable* list[2];
    list[0] = new Sphere(vec3(0,0,-1), 0.5);
    list[1] = new Sphere(vec3(0,-100.5, -1), 100);

    Hittable* world = new HittableList(list, 2);

    for (i32 y = 0; y < HEIGHT; y++) {
        for (i32 x = 0; x < WIDTH; x++) {
            Vec3 color = vec3(0,0,0);
            for (i32 s = 0; s < SUBSTEPS; s++) {
                f32 u =       ((f32)x + Random.next()) / (f32)WIDTH;
                f32 v = 1.0 - ((f32)y + Random.next()) / (f32)HEIGHT; // Flipping the V so we go from bottom to top

                Ray r = camera.getRay(u, v);
                color += colorFromRay(r, world);
            }
            color /= (f32)SUBSTEPS;
            color = vec3(sqrt(color.r), sqrt(color.g), sqrt(color.b));
            setPixelColor(pixels, x, y, color);
        }
    }
}

static void
savePixels(Color32* pixels) {
    stbi_write_png("render.png", WIDTH, HEIGHT, 4, pixels, WIDTH*sizeof(Color32));
}

static std::atomic<bool> gBackgroundThreadDone;

int
backgroundRenderAndSaveThread(void* data) {
    Color32* pixels = (Color32*)data;
    renderPixels(pixels);
    savePixels(pixels);
    gBackgroundThreadDone = true;
    return 0;
}

int
main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        const char* error = SDL_GetError();
        assert("SDL_Error" == error);
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "roju_tracer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH*WINDOW_SCALE, HEIGHT*WINDOW_SCALE,
        0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);

    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);

    if (!window || !renderer || !texture) {
        const char* error = SDL_GetError();
        printf("%s\n", error);
        assert("SDL_Error" == error);
        return -1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    Color32* pixels = (Color32*)calloc(WIDTH * HEIGHT, sizeof(Color32));

    gBackgroundThreadDone = false;
    SDL_Thread* backgroundThread = SDL_CreateThread(backgroundRenderAndSaveThread, "backgroundRenderAndSaveThread", (void *)pixels);

    bool backgroundThreadExpectedState = !gBackgroundThreadDone;

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

        if (backgroundThreadExpectedState != gBackgroundThreadDone) {
            backgroundThreadExpectedState = gBackgroundThreadDone;
            if(backgroundThreadExpectedState) {
                SDL_SetWindowTitle(window, "Done rendering");
            } else {
                SDL_SetWindowTitle(window, "Rendering...");
            }
        }

        SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(Color32));

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RendererFlip renderFlip = SDL_FLIP_NONE;
        SDL_Rect srcrect = { 0, 0,
                            WIDTH,
                            HEIGHT };
        SDL_Rect dstrect = { 0, 0,
                            WIDTH * WINDOW_SCALE,
                            HEIGHT * WINDOW_SCALE };

        SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, 0, renderFlip);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
