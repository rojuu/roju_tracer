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

