#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"

class texture 
{
public:
    virtual ~texture() = default;

    virtual colour value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture 
{
public:
    solid_color(const colour& albedo) : albedo(albedo) {}

    solid_color(double red, double green, double blue) : solid_color(colour(red, green, blue)) {}

    colour value(double u, double v, const point3& p) const override;

private:
    colour albedo;
};

class checker_texture : public texture {
public:
    checker_texture(double scale, shared_ptr<texture> even, shared_ptr<texture> odd) : inv_scale(1.0 / scale), even(even), odd(odd) {}

    checker_texture(double scale, const colour& c1, const colour& c2)
        : checker_texture(scale, make_shared<solid_color>(c1), make_shared<solid_color>(c2)) 
    {
    }

    colour value(double u, double v, const point3& p) const override 
    {
        auto xInteger = int(std::floor(inv_scale * p.x()));
        auto yInteger = int(std::floor(inv_scale * p.y()));
        auto zInteger = int(std::floor(inv_scale * p.z()));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};

#endif