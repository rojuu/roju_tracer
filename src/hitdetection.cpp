struct Ray {
    Vec3 o;
    Vec3 d;

    Ray() {}
    Ray(Vec3 o, Vec3 d) : o(o), d(d) { }

    Vec3 t(float t) const {
        return o + t*d;
    }
};

struct Material;

struct HitInfo {
    f32 t;
    Vec3 point;
    Vec3 normal;
    Material *material;
};

struct Hittable {
    virtual bool hit(const Ray& ray, f32 tMin, f32 tMax, HitInfo& info) const = 0;
};

struct Sphere : public Hittable {
    Vec3 center;
    f32 radius;
    Material* material;

    Sphere() { }
    Sphere(Vec3 center, f32 radius, Material* material)
        : center(center),
          radius(radius),
          material(material)
    {
    }

    virtual bool
    hit(const Ray& ray, f32 tMin, f32 tMax, HitInfo& info) const {
        printf("All spheres should be in a SphereList");
        assert(false);
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
            auto hittable = list[i];
            if (hittable->hit(ray, tMin, closestSoFar, tempInfo)) {
                hitSomething = true;
                closestSoFar = tempInfo.t;
                info = tempInfo;
            }
        }
        return hitSomething;
    }
};

struct SphereList : public Hittable {
    Sphere** list;
    size_t listSize;

    SphereList() { }
    SphereList(Sphere** _list, size_t size)
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
            Sphere* sphere = list[i];

            bool hit = false;
            tempInfo.material = sphere->material;

            Vec3 oc = ray.o - sphere->center;

            Vec3 rayD = ray.d;
            f32 a = //dot(rayD, rayD)
                ((rayD.x * rayD.x) + (rayD.y * rayD.y) + (rayD.z * rayD.z));
            f32 b = //dot(oc, rayD)
                ((oc.x * rayD.x) + (oc.y * rayD.y) + (oc.z * rayD.z));
            f32 c = //dot(oc, oc) - r^2
                ((oc.x * oc.x) + (oc.y * oc.y) + (oc.z * oc.z))
                    - sphere->radius*sphere->radius;

            f32 discriminant = b*b - a*c;
            if (discriminant > 0) {
                f32 discSqrt = sqrt(discriminant);

                f32 temp = (-b - discSqrt)/a;
                if (temp < closestSoFar && temp > tMin) {
                    tempInfo.t = temp;
                    tempInfo.point = ray.t(tempInfo.t);
                    tempInfo.normal = (tempInfo.point - sphere->center) / sphere->radius;
                    hit = true;
                    goto end;
                }
                temp = (-b + discSqrt)/a;
                if (temp < closestSoFar && temp > tMin) {
                    tempInfo.t = temp;
                    tempInfo.point = ray.t(tempInfo.t);
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
};
