struct Material {
    virtual bool scatter(const Ray& rIn,
                         const HitInfo& info,
                         Vec3& attenuation,
                         Ray& scattered) const = 0;
};

struct Lambertian : public Material {
    Color albedo;

    Lambertian(const Vec3& albedo) : albedo(albedo) {}

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