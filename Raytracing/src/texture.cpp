#include "texture.h"

colour solid_colour::value(double u, double v, const point3& p) const
{
    return albedo;
}

colour image_texture::value(double u, double v, const point3& p) const
{
    // If we have no texture data, then return solid cyan as a debugging aid.
    if (image.height() <= 0) return colour(0, 1, 1);

    // Clamp input texture coordinates to [0,1] x [1,0]
    u = interval(0, 1).clamp(u);
    v = 1.0 - interval(0, 1).clamp(v);  // Flip V to image coordinates

    auto i = int(u * image.width());
    auto j = int(v * image.height());
    auto pixel = image.pixel_data(i, j);

    auto color_scale = 1.0 / 255.0;
    return colour(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
}

colour checker_texture::value(double u, double v, const point3& p) const
{
    auto xInteger = int(std::floor(inv_scale * p.x()));
    auto yInteger = int(std::floor(inv_scale * p.y()));
    auto zInteger = int(std::floor(inv_scale * p.z()));

    bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

    return isEven ? even->value(u, v, p) : odd->value(u, v, p);
}

colour noise_texture::value(double u, double v, const point3& p) const
{
    return colour(.5, .5, .5) * (1 + std::sin(scale * p.z() + 10 * noise.turb(p, 7)));
}
