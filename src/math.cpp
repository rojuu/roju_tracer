struct _Random {
    pcg32 rng;
    std::uniform_real_distribution<f32> dist;

    _Random() {
        pcg_extras::seed_seq_from<std::random_device> seed_source;
        rng = pcg32(seed_source);
        dist = std::uniform_real_distribution<f32>(0, 1);
    }

    f32 next() {
        return dist(rng);
    }
};
static _Random Random;

static Vec3 randomInUnitSphere() {
    Vec3 p;
    do {
        p = 2.0 * vec3(Random.next(), Random.next(), Random.next()) - vec3(1, 1, 1);
    } while (HMM_LengthSquared(p) >= 1.0);
    return p;
}

static Vec3 randomInUnitDisk() {
    Vec3 p;
    do {
        p = 2.0 * vec3(Random.next(), Random.next(), 0) - vec3(1, 1, 0);
    } while (HMM_Dot(p, p) >= 1.0);
    return p;
}

static Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v - 2 * HMM_Dot(v, n) * n;
}

static bool refract(const Vec3& v, const Vec3& n, f32 niOverNt, Vec3& refracted) {
    Vec3 uv = HMM_FastNormalize(v);
    f32 dt = HMM_Dot(uv, n);
    f32 discriminant = 1.0 - niOverNt * niOverNt * (1 - dt * dt);
    if (discriminant > 0) {
        refracted = niOverNt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    } else {
        return false;
    }
}

static f32 schlick(f32 cosine, f32 refIdx) {
    f32 r0 = (1 - refIdx) / (1 + refIdx);
    r0 = r0 * r0;
    return r0 * (1 - r0) * pow((1 - cosine), 5);
}
