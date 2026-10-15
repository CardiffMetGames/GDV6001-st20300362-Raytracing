#ifndef TEXTURE_H
#define TEXTURE_H

#include "common.h"
#include "interval.h"
#include "perlin.h"
#include "rtw_stb_image.h"

class texture 
{
public:
    virtual ~texture() = default;

    virtual colour value(double u, double v, const point3& p) const = 0;

};

class solid_colour : public texture 
{
public:
    solid_colour(const colour& albedo) : albedo(albedo) {}

    solid_colour(double red, double green, double blue) : solid_colour(colour(red, green, blue)) {}

    colour value(double u, double v, const point3& p) const override;

private:
    colour albedo;
};

class checker_texture : public texture {
public:
    checker_texture(double scale, shared_ptr<texture> even, shared_ptr<texture> odd) : inv_scale(1.0 / scale), even(even), odd(odd) {}

    checker_texture(double scale, const colour& c1, const colour& c2) : checker_texture(scale, make_shared<solid_colour>(c1), make_shared<solid_colour>(c2)) {}

    colour value(double u, double v, const point3& p) const override;

private:
    double inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};

class image_texture : public texture 
{
public:
    image_texture(const char* filename) : image(filename) {}

    colour value(double u, double v, const point3& p) const override;

private:
    rtw_image image;
};

class noise_texture : public texture 
{
public:
    noise_texture(double scale) : scale(scale) {}

    colour value(double u, double v, const point3& p) const override;

private:
    perlin noise;
    double scale;
};

#endif