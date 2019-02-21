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
    union {
        u32 value;
        struct {
            u8 a, b, g, r;
        };
    };
};

typedef hmm_vec3 Color;

static inline Color32
makeColor(u8 r, u8 g, u8 b, u8 a = 255) {
    Color32 result;
    result.r = r;
    result.g = g;
    result.b = b;
    result.a = a;
    return result;
}

static inline void
setPixelColor(Color32* pixels, i32 x, i32 y, Color32 color) {
    pixels[y * WIDTH + x] = color;
}

struct Ray {
    hmm_vec3 origin;
    hmm_vec3 direction;
};

struct Light {
    hmm_vec3 position;
    Color color;
};

struct Material {
    Color color;
    b32 isGlass;
    i32 refractionIndex;
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
struct Sphere : public Object {
    Sphere() { type = OT_SPHERE; }
    hmm_vec3 position;
    f32 radius;
};
struct Plane : public Object {
    Plane() { type = OT_PLANE; }
    hmm_vec3 p;
    hmm_vec3 n;
};
struct Disk : public Object {
    Disk() { type = OT_DISK; }
    hmm_vec3 position;
    hmm_vec3 normal;
    f32 radius;
};
struct AABBBox : public Object {
    AABBBox() { type = OT_AABB_BOX; }
    hmm_vec3 position;
    hmm_vec3 bounds; // Box width == x * 2;
};

static Ray
computeReflectionRay(hmm_vec3 rayDirection, hmm_vec3 hitNormal) {
    return {};
}

static Ray
computeRefractionRay(i32 refractionIndex, hmm_vec3 rayDirection, hmm_vec3 hitNormal) {
    return {};
}

static void
fresnel(i32 refractionIndex, hmm_vec3 hitNormal, hmm_vec3 rayDirection, f32* out_Kr, f32* out_Kt) {
}

static b32
intersect(Object* object, Ray ray, hmm_vec3* out_pHit, hmm_vec3* out_nHit) {
    if(!object) {
        return false;
    };
    switch(object->type){
        case OT_SPHERE: {
            Sphere* sphere = (Sphere*)object;
            if(sphere) {
                return true;
            }
            break;
        }
        case OT_PLANE: {
            break;
        }
        case OT_DISK:
        case OT_AABB_BOX:
        case OT_NONE:
        default: {
            SDL_Log("Supposedly nothing?");
            break;
        }
    }
    return false;
}

static Color
trace(Ray ray, int depth, int maxRayDepth, Light light, Object** objects, i32 objectCount) {
    Object *object = NULL;
    f32 minDistance = INFINITY;
    hmm_vec3 pHit;
    hmm_vec3 nHit;
    for (int i = 0; i < objectCount; ++i) {
        if (intersect(objects[i], ray, &pHit, &nHit)) {
            f32 distance = HMM_ABS(HMM_Length(pHit - ray.origin));
            if (distance < minDistance) {
                object = objects[i];
                minDistance = distance;
            }
        }
    }
    if (object == NULL)
        return HMM_Vec3(0,0,0);
    // if the object material is glass, split the ray into a reflection
    // and a refraction ray.
    if (object->material->isGlass && depth < maxRayDepth) {
        // compute reflection
        Ray reflectionRay = computeReflectionRay(ray.direction, nHit);
        // recurse
        Color reflectionColor = trace(reflectionRay, depth+1, maxRayDepth, light, objects, objectCount);

        Ray refractionRay = computeRefractionRay(
            object->material->refractionIndex,
            ray.direction,
            nHit);
        Color refractionColor = trace(refractionRay, depth+1, maxRayDepth, light, objects, objectCount);

        float Kr, Kt;
        fresnel(
            object->material->refractionIndex,
            nHit,
            ray.direction,
            &Kr,
            &Kt);
        return reflectionColor * Kr + refractionColor * (1-Kr);
    }
    // object is a diffuse opaque object
    // compute illumination
    Ray shadowRay;
    shadowRay.direction = light.position - pHit;
    bool isShadow = false;
    for (int i = 0; i < objectCount; ++i) {
        if (intersect(objects[i], shadowRay, 0, 0)) {
            // hit point is in shadow so just return
            return HMM_Vec3(0,0,0);
        }
    }
    // point is illuminated
    return object->material->color * light.color;
}

//TODO: Memory pool instead of calling allocate up front, probably should define the memory outside of this function anyway
static void
renderPixels(Color32* pixels) {
    memset(pixels, 0, sizeof(Color32) * WIDTH * HEIGHT);

    #if 0
    hmm_vec3 eyePosition = HMM_Vec3(0, 0, 10);

    const i32 sphereCount = 1;
    const i32 planeCount = 1;
    const i32 objectCapacity = sphereCount + planeCount;
    Object** objects = allocate<Object*>(objectCapacity);
    atScopeExit(free(objects));

    i32 objectCount = 0;
    #define ALLOC_OBJECT_ARRAY(typename, varname, count) \
        typename* varname = allocate<typename>(count); \
        for(i32 i = 0; i < count; i++){ \
            varname[i] = typename(); \
            objects[objectCount++] = &(varname[i]); \
        } \
        atScopeExit(free(varname));
    ALLOC_OBJECT_ARRAY(Sphere, spheres, sphereCount);
    ALLOC_OBJECT_ARRAY(Plane, planes, planeCount);
    #undef ALLOC_OBJECT_ARRAY

    Material sphereMaterial;
    sphereMaterial.color = HMM_Vec3(0, 1, 0);

    Material planeMaterial;
    planeMaterial.color = HMM_Vec3(0.5f, 0.5f, 0.5f);

    Sphere* sphere = &(spheres[0]);
    sphere->material = &sphereMaterial;
    sphere->position = HMM_Vec3(0,0,0);
    sphere->radius = 1;

    Plane* plane = &(planes[0]);
    plane->material = &planeMaterial;
    plane->p = HMM_Vec3(0, -10, 0);
    plane->n = HMM_Vec3(0, 1, 0);

    Light light;
    light.color = HMM_Vec3(1,1,1);
    light.position = HMM_Vec3(-10, 10, -10);

    Ray primaryRay;
    primaryRay.direction = HMM_Vec3(0, 0, -1);
    primaryRay.origin = eyePosition;

    trace(primaryRay, 0, 10, light, objects, objectCount);

    #else
    hmm_vec2 center = HMM_Vec2((f32)WIDTH/2.f, (f32)HEIGHT/2.f);
    for(i32 y = 0; y < HEIGHT; y++) {
        for(i32 x = 0; x < WIDTH; x++) {
            Color32 green = makeColor(0, 255, 0);
            f32 radius = (f32)HEIGHT/2 * 0.33f;
            hmm_vec2 point = HMM_Vec2(x, y);
            if(HMM_Length(center - point) < radius) {
                setPixelColor(pixels, x, y, green);
            }
        }
        SDL_Delay(1);
    }
    #endif
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

    char* windowTitle = (char*)"roju_tracer";
    SDL_Window* window = SDL_CreateWindow(
        windowTitle,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        0);
    atScopeExit(SDL_DestroyWindow(window));

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);

    if (!window || !renderer || !texture) {
        const char* error = SDL_GetError();
        assert("SDL_Error" == error);
        return -1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    Color32* pixels = allocate<Color32>(WIDTH * HEIGHT);

    SDL_Thread* renderThread = SDL_CreateThread(backgroundRenderThread, "backgroundRenderThread", (void *)pixels);

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
