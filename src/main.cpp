#include <stdio.h>
#include <SDL2/SDL.h>

#include <functional>

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

template<typename T>
static inline T*
allocate(size_t count) {
    return (T*)calloc(count, sizeof(T));
}

class DeferredTask {
    std::function<void()> func_;
public:
    DeferredTask(std::function<void()> func) :
    func_(func) {
    }
    ~DeferredTask() {
        func_();
    }
    DeferredTask& operator=(const DeferredTask&) = delete;
    DeferredTask(const DeferredTask&) = delete;
};

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

struct Ray {
    hmm_vec3 origin;
    hmm_vec3 direction;
};

struct Material {
    Color color;
};

//TODO: Object types might be a good candidate for code generation
enum ObjectType {
    OT_NONE,
    OT_SPHERE,
    OT_PLANE,
    OT_DISK,
    OT_AABB_BOX,
};
struct Object {
    Object() { type = OT_NONE; }
    ObjectType type;
    Material* material;
};
struct Sphere : Object {
    Sphere() { type = OT_SPHERE; }
    hmm_vec3 position;
    f32 radius;
};
struct Plane : Object {
    Plane() { type = OT_PLANE; }
    hmm_vec3 p;
    hmm_vec3 n;
};
struct Disk : Object {
    Disk() { type = OT_DISK; }
    hmm_vec3 position;
    hmm_vec3 normal;
    f32 radius;
};
struct AABBBox : Object {
    AABBBox() { type = OT_AABB_BOX; }
    hmm_vec3 position;
    hmm_vec3 bounds; // Box width == x * 2;
};

#if 0
static Color
trace(const Ray& ray, int depth, const std::vector<Object*>& objects) {
    Object *object = NULL;
    float minDist = INFINITY;
    Point pHit;
    Normal nHit;
    for (int k = 0; k < objects.size(); ++k) {
        if (Intersect(objects[k], ray, &pHit, &nHit)) {
            // ray origin = eye position of it's the prim ray
            float distance = Distance(ray.origin, pHit);
            if (distance < minDistance) {
                object = objects[i];
                minDistance = distance;
            }
        }
    }
    if (object == NULL)
        return 0;
    // if the object material is glass, split the ray into a reflection
    // and a refraction ray.
    if (object->isGlass && depth < MAX_RAY_DEPTH) {
        // compute reflection
        Ray reflectionRay;
        reflectionRay = computeReflectionRay(ray.direction, nHit);
        // recurse
        color reflectionColor = Trace(reflectionRay, depth + 1);
        Ray refractioRay;
        refractionRay = computeRefractionRay(
            object->indexOfRefraction,
            ray.direction,
            nHit);
        // recurse
        color refractionColor = Trace(refractionRay, depth + 1);
        float Kr, Kt;
        fresnel(
            object->indexOfRefraction,
            nHit,
            ray.direction,
            &Kr,
            &Kt);
        return reflectionColor * Kr + refractionColor * (1-Kr);
    }
    // object is a diffuse opaque object
    // compute illumination
    Ray shadowRay;
    shadowRay.direction = lightPosition - pHit;
    bool isShadow = false;
    for (int k = 0; k < objects.size(); ++k) {
        if (Intersect(objects[k], shadowRay)) {
            // hit point is in shadow so just return
            return 0;
        }
    }
    // point is illuminated
    return object->color * light.brightness;
}
#endif

static void
renderPixels(Color* pixels) {
    memset(pixels, 0, sizeof(Color) * WIDTH * HEIGHT);

    const i32 sphereCount = 1;
    Object** objects = allocate<Object*>(sphereCount);

    #define ALLOC_OBJECT_ARRAY(typename, varname, count) \
        typename* varname = allocate<typename>(count); \
        for(i32 i = 0; i < count; i++){ \
            varname[i] = typename(); \
            objects[i] = &(varname[i]); \
        } \
        typename ## _deferred = DeferredTask([](){ free(varname) });
    ALLOC_OBJECT_ARRAY(Sphere, spheres, sphereCount)
    #undef ALLOC_OBJECT_ARRAY

    for(i32 i = 0; i < sphereCount; i++){
        Sphere* sphere = &(spheres[i]);
        hmm_vec3 pos = HMM_Vec3(0,0,0);
        sphere->position = pos;
    }

    hmm_vec2 center = HMM_Vec2((f32)WIDTH/2.f, (f32)HEIGHT/2.f);
    for(i32 y = 0; y < HEIGHT; y++) {
        for(i32 x = 0; x < WIDTH; x++) {
            Color green = color(0, 255, 0);
            f32 radius = (f32)HEIGHT/2 * 0.33f;
            hmm_vec2 point = HMM_Vec2(x, y);
            if(HMM_Length(center - point) < radius) {
                setPixelColor(pixels, x, y, green);
            }
        }
    }

    free(objects);
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

    Color* pixels = allocate<Color>(WIDTH * HEIGHT);

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
