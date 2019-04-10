#include <stdio.h>
#include <SDL2/SDL.h>

#include "HandmadeMath.cpp"
#include "stb_image_write.cpp"

#include <atomic>

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
static T*
allocate(size_t count) {
    return (T*)calloc(count, sizeof(T));
}

const int WIDTH  = 640;
const int HEIGHT = 480;

typedef hmm_vec3 Vec3;
typedef hmm_vec4 Vec4;

static Vec3
vec3(f32 a, f32 b, f32 c) {
    return HMM_Vec3(a, b, c);
}

static Vec4
vec4(f32 a, f32 b, f32 c, f32 d) {
    return HMM_Vec4(a, b, c, d);
}


struct Color32 {
    u32 value;
};

typedef Vec3 Color;

static Color
makeColor(f32 r, f32 g, f32 b) {
    return vec3(r, g, b);
}

static Color32
makeColor32(u8 r, u8 g, u8 b, u8 a = 255) {
    Color32 result;
    //result.value = (r << 24) + (g << 16) + (b << 8) + (a << 0);
    result.value = (a << 24) + (b << 16) + (g << 8) + (r << 0);
    return result;
}

static Color32
makeColor32(Color color) {
    Color32 result;
    result = makeColor32(color.r * 255, color.g * 255, color.b * 255);
    return result;
}

static void
setPixelColor(Color32* pixels, i32 x, i32 y, Color color) {
    Color32 color32 = makeColor32(color);
    pixels[y * WIDTH + x] = color32;
}

struct Ray {
    Vec3 o;
    Vec3 d;

    Ray(Vec3 o, Vec3 d) : o(o), d(d) { }

    Vec3 t(float t) const {
        return o + t*d;
    }
};

struct HitInfo {
    f32 t;
    Vec3 point;
    Vec3 normal;
};

struct Hittable {
    virtual bool hit(const Ray& ray, f32 tMin, f32 tMax, HitInfo& info) const = 0;
};

struct Sphere : public Hittable {
    Vec3 center;
    f32 radius;

    Sphere() { }
    Sphere(Vec3 center, f32 radius)
        : center(center), radius(radius)
    {
    }

    virtual bool
    hit(const Ray& ray, f32 tMin, f32 tMax, HitInfo& info) const {
        Vec3 oc = ray.o - center;
        f32 a = HMM_Dot(ray.d, ray.d);
        f32 b = HMM_Dot(oc, ray.d);
        f32 c = HMM_Dot(oc, oc) - radius*radius;
        f32 discriminant = b*b - a*c;
        if (discriminant > 0) {
            f32 temp = (-b - sqrt(b*b-a*c))/a;
            if (temp < tMax && temp > tMin) {
                info.t = temp;
                info.point = ray.t(info.t);
                info.normal = (info.point - center) / radius;
                return true;
            }
            temp = (-b + sqrt(b*b-a*c))/a;
            if (temp < tMax && temp > tMin) {
                info.t = temp;
                info.point = ray.t(info.t);
                info.normal = (info.point - center) / radius;
                return true;
            }
        }
        return false;
    }
};

struct HittableList : public Hittable {
    Hittable** list;
    size_t listSize;

    HittableList() { }
    HittableList(Hittable** _list, size_t size)
    {
        list = _list;
        listSize = size;
    }

    virtual bool
    hit(const Ray& ray, f32 tMin, f32 tMax, HitInfo& info) const {
        HitInfo tempInfo;
        bool hitSomething = false;
        f64 closestSoFar = tMax;
        for (int i = 0; i < listSize; i++) {
            if (list[i]->hit(ray, tMin, closestSoFar, tempInfo)) {
                hitSomething = true;
                closestSoFar = tempInfo.t;
                info = tempInfo;
            }
        }
        return hitSomething;
    }
};

static Color
colorFromRay(Ray& ray, Hittable* world) {
    HitInfo info;
    if (world->hit(ray, 0, MAXFLOAT, info)) {
        return 0.5f*vec3(info.normal.x+1, info.normal.y+1, info.normal.z+1);
    } else {
        Vec3 unitDirection = HMM_FastNormalize(ray.d);
        f32 t = 0.5f * (unitDirection.y + 1.0f);
        return (1.0f-t)*vec3(1.0f, 1.0f, 1.0f) + t*vec3(0.5f, 0.7f, 1.0f);
    }
}

static void
renderPixels(Color32* pixels) {
    memset(pixels, 0, sizeof(Color32) * WIDTH * HEIGHT);

    f32 w = (f32)WIDTH  / 100.f;
    f32 h = (f32)HEIGHT / 100.f;

    Vec3 lowerLeftCorner = vec3(-w, -h, -1);
    Vec3 horizontal = vec3( w*2, 0, 0);
    Vec3 vertical = vec3(0, h*2, 0);
    Vec3 origin = vec3(0, 0, 0);

    Hittable* list[2];
    list[0] = new Sphere(vec3(0,0,-1), 0.5);
    list[1] = new Sphere(vec3(0,-100.5, -1), 100);

    Hittable* world = new HittableList(list, 2);

    for (i32 y = 0; y < HEIGHT; y++) {
        for (i32 x = 0; x < WIDTH; x++) {
            f32 u =       (f32)x / (f32)WIDTH;
            f32 v = 1.0 - (f32)y / (f32)HEIGHT; // Flipping the V so we go from bottom to top

            Ray r = Ray(origin, lowerLeftCorner + u*horizontal + v*vertical);
            Color color = colorFromRay(r, world);
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

    Color32* pixels = allocate<Color32>(WIDTH * HEIGHT);

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

        SDL_RenderCopy(renderer, texture, NULL, NULL);

        SDL_RenderPresent(renderer);
    }

    return 0;
}
