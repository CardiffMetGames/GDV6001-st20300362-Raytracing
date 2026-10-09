#include "material.h"

bool material::scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const
{
    return false;
}

bool lambertian::scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const
{
    auto scatter_direction = rec.normal + random_unit_vector();

    // Catch degenerate scatter direction
    if (scatter_direction.near_zero())
    {
        scatter_direction = rec.normal;
    }

    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
}

bool metal::scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const
{
    vec3 reflected = reflect(r_in.direction(), rec.normal);
    scattered = ray(rec.p, reflected);
    attenuation = albedo;
    return true;
}
