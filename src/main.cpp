#include <stdio.h>
#include <math.h>
#include <atomic>
#include <random>
#include <cstring>
#include <assert.h>

#include <SDL2/SDL.h>

#include "HandmadeMath.cpp"
#include "stb_image_write.cpp"
#include "pcg_random.hpp"

#include "config.cpp"
#include "types.cpp"
#include "math.cpp"
#include "hitdetection.cpp"
#include "materials.cpp"

struct Camera {
    Vec3 lowerLeftCorner;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 origin;
    Vec3 u, v, w;
    f32 lensRadius;

    Camera(Vec3 lookFrom, Vec3 lookAt, Vec3 vup, f32 vfov, f32 aspect, f32 aperture, f32 focusDist) {
        lensRadius = aperture / 2;
        f32 theta = vfov*M_PI/180;
        f32 halfHeight = tan(theta/2);
        f32 halfWidth = aspect * halfHeight;
        origin = lookFrom;
        w = HMM_FastNormalize(lookFrom - lookAt);
        u = HMM_FastNormalize(HMM_Cross(vup, w));
        v = HMM_Cross(w, u);
        lowerLeftCorner = origin - halfWidth*focusDist*u - halfHeight*focusDist*v - focusDist*w;
        horizontal = 2*halfWidth*focusDist*u;
        vertical = 2*halfHeight*focusDist*v;
    }

    Ray getRay(f32 s, f32 t) {
        Vec3 rd = lensRadius * randomInUnitDisk();
        Vec3 offset = u * rd.x + v * rd.y;
        Ray ray = Ray(origin + offset, lowerLeftCorner + s*horizontal + t*vertical - origin - offset);
        return ray;
    }
};

static Color
calcColor(Ray& ray, Hittable* world, i32 depth) {
    HitInfo info;
    if (world->hit(ray, 0.001, MAXFLOAT, info)) {
        Ray scattered;
        Vec3 attenuation;
        if (depth < TRACING_MAX_DEPTH && info.material->scatter(ray, info, attenuation, scattered)) {
            return attenuation * calcColor(scattered, world, depth+1);
        } else {
            return vec3(0,0,0);
        }
    } else {
        Vec3 unitDirection = HMM_FastNormalize(ray.d);
        f32 t = 0.5f * (unitDirection.y + 1.0f);
        return (1.0f-t)*vec3(1.0f, 1.0f, 1.0f) + t*vec3(0.5f, 0.7f, 1.0f);
    }
}

static Hittable*
randomScene() {
    int n = 500;
    Hittable** list = new Hittable* [n+1];
    list[0] = new Sphere(vec3(0,-1000,0), 1000, new Lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float chooseMat = Random.next();
            Vec3 center = vec3(a+0.9*Random.next(), 0.2, b+0.9*Random.next());
            if (HMM_Length(center-vec3(4,0.2,0)) > 0.9) {
                if (chooseMat < 0.8) { //diffuse
                    list[i++] = new Sphere(center, 0.2, new Lambertian(vec3(Random.next()*Random.next(), Random.next()*Random.next(), Random.next()*Random.next())));
                } else if (chooseMat < 0.95) { //metal
                    list[i++] = new Sphere(center, 0.2, new Metal(vec3(0.5*(1 + Random.next()), 0.5*(1+ Random.next()), 0.5*(1+ Random.next())), 0.5*Random.next()));
                } else { //glass
                    list[i++] = new Sphere(center, 0.2, new Dielectric(1.5));
                }
            }
        }
    }
end:
    list[i++] = new Sphere(vec3(0, 1, 0), 1.0, new Dielectric(1.5));
    list[i++] = new Sphere(vec3(-4, 1, 0), 1.0, new Lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new Sphere(vec3(4, 1, 0), 1.0, new Metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new HittableList(list, i);
}

static void
renderPixels(Color32* pixels) {
    memset(pixels, 0, sizeof(Color32) * WIDTH * HEIGHT);

    Vec3 lookFrom = vec3(13,2,3);
    Vec3 lookAt = vec3(0,0,0);
    f32 distToFocus = 10;
    f32 aperture = 0.1;
    Camera camera(lookFrom, lookAt, vec3(0,1,0), 20, float(WIDTH)/float(HEIGHT), aperture, distToFocus);

    Hittable* world = randomScene();

    for (i32 y = 0; y < HEIGHT; y++) {
        for (i32 x = 0; x < WIDTH; x++) {
            Vec3 color = vec3(0,0,0);
            for (i32 s = 0; s < SUBSTEPS; s++) {
                f32 u =       ((f32)x + Random.next()) / (f32)WIDTH;
                f32 v = 1.0 - ((f32)y + Random.next()) / (f32)HEIGHT; // Flipping the V so we go from bottom to top

                Ray r = camera.getRay(u, v);
                color += calcColor(r, world, 0);
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
