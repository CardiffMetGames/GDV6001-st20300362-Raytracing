#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "common.h"

class material 
{
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const;
};

class lambertian : public material 
{
public:
    lambertian(const colour& albedo) : albedo(albedo) {}

    bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override;

private:
    colour albedo;
};

class metal : public material 
{
public:
    metal(const colour& albedo) : albedo(albedo) {}

    bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override;

private:
    colour albedo;
};

#endif