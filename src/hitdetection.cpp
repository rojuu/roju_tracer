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
        f32 discSqrt = sqrt(discriminant);
        f32 temp;

        //
        // Calculate the two possible results
        //
        temp = (-b - discSqrt) / a;
        u32 mask1 = (temp < closestSoFar && temp > tMin);
        HitInfo tempInfo1 = tempInfo;
        tempInfo1.t = temp;
        tempInfo1.point = pointOnRay(ray, temp);
        tempInfo1.normal = (tempInfo1.point - sphere->center) / sphere->radius;

        temp = (-b + discSqrt) / a;
        u32 mask2 = (temp < closestSoFar && temp > tMin);
        HitInfo tempInfo2 = tempInfo;
        tempInfo2.t = temp;
        tempInfo2.point = pointOnRay(ray, temp);
        tempInfo2.normal = (tempInfo2.point - sphere->center) / sphere->radius;

        // Pick the correct result, or none
        //   if discriminant > 0:
        //     Pick tempInfo1 if that is valid, otherwise tempInfo2,
        //     otherwise the one we already had
        //   else
        //     pick the one we already had
        HitInfo temp1 = mask2 ? tempInfo2 : tempInfo;
        HitInfo temp2 = mask1 ? tempInfo1 : temp1;
        tempInfo = (discriminant > 0) ? temp2 : tempInfo;

        hit = mask1 || mask2;
        hitSomething = hit || hitSomething;
        closestSoFar = hit ? tempInfo.t : closestSoFar;
        info = hit ? tempInfo : info;
    }

    return hitSomething;
}
