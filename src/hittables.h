
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