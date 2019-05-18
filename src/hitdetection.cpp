struct Ray {
    Vec3 o;
    Vec3 d;
};

static Vec3
pointOnRay(const Ray& ray, float t) {
    return ray.o + t * ray.d;
}

static bool
hit(const World& world, const Ray& ray, f32 tMin, f32 tMax, HitInfo& info) {
    HitInfo tempInfo;
    bool hitSomething = false;
    f64 closestSoFar = tMax;
    Array<Sphere> spheres = world.spheres;
    for (int i = 0; i < spheres.count; i++) {
        Sphere* sphere = spheres.members + i;

        bool hit = false;
        tempInfo.material = sphere->material;

        Vec3 oc = ray.o - sphere->center;

        Vec3 rayD = ray.d;

        f32 a = HMM_Dot(rayD, rayD);
        f32 b = HMM_Dot(oc, rayD);
        f32 c = HMM_Dot(oc, oc) - sphere->radius * sphere->radius;

        f32 discriminant = b * b - a * c;
        if (discriminant > 0) {
            f32 discSqrt = sqrt(discriminant);

            f32 temp = (-b - discSqrt) / a;
            if (temp < closestSoFar && temp > tMin) {
                tempInfo.t = temp;
                tempInfo.point = pointOnRay(ray, tempInfo.t);
                tempInfo.normal = (tempInfo.point - sphere->center) / sphere->radius;
                hit = true;
                goto end;
            }
            temp = (-b + discSqrt) / a;
            if (temp < closestSoFar && temp > tMin) {
                tempInfo.t = temp;
                tempInfo.point = pointOnRay(ray, tempInfo.t);
                tempInfo.normal = (tempInfo.point - sphere->center) / sphere->radius;
                hit = true;
                goto end;
            }
        }

    end:
        if (hit) {
            hitSomething = true;
            closestSoFar = tempInfo.t;
            info = tempInfo;
        }
    }
    return hitSomething;
}