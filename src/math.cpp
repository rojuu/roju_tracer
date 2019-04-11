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

Vec3 randomInUnitSphere() {
    Vec3 p;
    do {
        p = 2.0*vec3(Random.next(), Random.next(), Random.next()) - vec3(1,1,1);
    } while(HMM_LengthSquared(p) >= 1.0);
    return p;
}