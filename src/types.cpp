#include <stdint.h>
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef s8 b8;
typedef s16 b16;
typedef s32 b32;
typedef s64 b64;

typedef hmm_vec3 Vec3;
typedef hmm_vec4 Vec4;

struct Color32 {
    u32 value;
};

typedef Vec3 Color;

struct Material;

struct Sphere {
    Vec3 center;
    f32 radius;
    Material* material;
};
struct World {
    Array<Sphere> spheres;
};

struct HitInfo {
    f32 t;
    Vec3 point;
    Vec3 normal;
    Material* material;
};

static Vec3
vec3(f32 a, f32 b, f32 c) {
    return HMM_Vec3(a, b, c);
}

static Vec4
vec4(f32 a, f32 b, f32 c, f32 d) {
    return HMM_Vec4(a, b, c, d);
}

static Color
makeColor(f32 r, f32 g, f32 b) {
    return vec3(r, g, b);
}

static Color32
makeColor32(u8 r, u8 g, u8 b, u8 a = 255) {
    Color32 result;
    // result.value = (r << 24) + (g << 16) + (b << 8) + (a << 0);
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
setPixelColor(Color32* pixels, s32 x, s32 y, Color color) {
    Color32 color32 = makeColor32(color);
    pixels[y * WIDTH + x] = color32;
}
