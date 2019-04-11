struct Color32 {
    u32 value;
};

typedef Vec3 Color;

static Color
makeColor(f32 r, f32 g, f32 b) {
    return vec3(r, g, b);
}

static Color32
makeColor32(u8 r, u8 g, u8 b, u8 a = 255) {
    Color32 result;
    //result.value = (r << 24) + (g << 16) + (b << 8) + (a << 0);
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
setPixelColor(Color32* pixels, i32 x, i32 y, Color color) {
    Color32 color32 = makeColor32(color);
    pixels[y * WIDTH + x] = color32;
}

struct Ray {
    Vec3 o;
    Vec3 d;

    Ray(Vec3 o, Vec3 d) : o(o), d(d) { }

    Vec3 t(float t) const {
        return o + t*d;
    }
};

struct HitInfo {
    f32 t;
    Vec3 point;
    Vec3 normal;
};

struct Camera {
    Vec3 lowerLeftCorner;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 origin;

    Camera() {
        f32 w = (f32)WIDTH  / 100.f;
        f32 h = (f32)HEIGHT / 100.f;

        lowerLeftCorner = vec3(-w, -h, -1);
        horizontal = vec3( w*2, 0, 0);
        vertical = vec3(0, h*2, 0);
        origin = vec3(0, 0, 0);
    }

    Ray getRay(f32 u, f32 v) {
        Ray ray = Ray(origin, lowerLeftCorner + u*horizontal + v*vertical);
        return ray;
    }
};
static Camera camera;

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