struct Material {
    virtual bool scatter(const Ray& rIn,
                         const HitInfo& info,
                         Vec3& attenuation,
                         Ray& scattered) const = 0;
};

struct Lambertian : public Material {
    Color albedo;

    Lambertian(const Color& albedo) : albedo(albedo) {}

    virtual bool scatter(const Ray& rIn,
                         const HitInfo& info,
                         Vec3& attenuation,
                         Ray& scattered) const
    {
        Vec3 target = info.point + info.normal + randomInUnitSphere();
        scattered = Ray(info.point, target-info.point);
        attenuation = albedo;
        return true;
    }
};

struct Metal : public Material {
    Color albedo;
    f32 fuzz;

    Metal(const Vec3& albedo, float _fuzz) : albedo(albedo) {
        if (_fuzz < 1) fuzz = _fuzz;
        else fuzz = 1;
    }

    virtual bool scatter(const Ray& rIn,
                         const HitInfo& info,
                         Vec3& attenuation,
                         Ray& scattered) const
    {
        Vec3 reflected = reflect(HMM_FastNormalize(rIn.d), info.normal);
        scattered = Ray(info.point, reflected + fuzz*randomInUnitSphere());
        attenuation = albedo;
        return (HMM_Dot(scattered.d, info.normal) > 0);
    }
};

struct Dielectric : public Material {
    f32 refIdx;

    Dielectric(f32 refIdx) : refIdx(refIdx) {}

    virtual bool scatter(const Ray& rIn,
                         const HitInfo& info,
                         Vec3& attenuation,
                         Ray& scattered) const
    {
        Vec3 outwardNormal;
        Vec3 reflected = reflect(rIn.d, info.normal);
        f32 niOverNt;
        attenuation = vec3(1.0, 1.0, 1.0);
        Vec3 refracted;
        f32 reflectProb;
        f32 cosine;
        if (HMM_Dot(rIn.d, info.normal) > 0) {
            outwardNormal = -info.normal;
            niOverNt = refIdx;
            cosine = refIdx * HMM_Dot(rIn.d, info.normal) / HMM_Length(rIn.d);
        } else {
            outwardNormal = info.normal;
            niOverNt = 1.0/refIdx;
            cosine = -HMM_Dot(rIn.d, info.normal) / HMM_Length(rIn.d);
        }
        if (refract(rIn.d, outwardNormal, niOverNt, refracted)) {
            reflectProb = schlick(cosine, refIdx);
        } else {
            reflectProb = 1.0;
        }
        if(Random.next() < reflectProb) {
            scattered = Ray(info.point, reflected);
        } else {
            scattered = Ray(info.point, refracted);
        }
        return true;
    }
};