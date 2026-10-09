#ifndef COLOUR_H
#define COLOUR_H

#include "vec3.h"

#include <iostream>

using colour = vec3;

inline double linear_to_gamma(double linear_component)
{
    if (linear_component > 0)
        return std::sqrt(linear_component);

    return 0;
}

inline void write_colour(std::ostream& out, const colour& pixel_colour)
{
    auto r = pixel_colour.x();
    auto g = pixel_colour.y();
    auto b = pixel_colour.z();

    // Apply a linear to gamma transform for gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    auto clamp_intensity = [](double v) -> double {
        if (v < 0.0) return 0.0;
        if (v > 0.999) return 0.999;
        return v;
    };

    int rbyte = static_cast<int>(256 * clamp_intensity(r));
    int gbyte = static_cast<int>(256 * clamp_intensity(g));
    int bbyte = static_cast<int>(256 * clamp_intensity(b));

    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif
