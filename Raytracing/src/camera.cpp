#include "camera.h"

#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cmath>
#include "colour.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external\stb_image_write.h"

void camera::render(const hittable& world, const std::string& format, const std::string& filename)
{
    initialize();

    std::string fmt = format;
    for (auto &c : fmt) c = static_cast<char>(std::tolower(c));

    if (fmt == "png") {
        std::vector<unsigned char> image;
        image.resize(image_width * image_height * 3);

        for (int j = 0; j < image_height; j++)
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

            for (int i = 0; i < image_width; i++)
            {
                colour pixel_colour(0, 0, 0);

                for (int sample = 0; sample < samples_per_pixel; sample++)
                {
                    ray r = get_ray(i, j);
                    pixel_colour += ray_colour(r, max_depth, world);
                }

                auto gamma_correct = [](double v) 
                {
                    if (v <= 0.0)
                    {
                         return 0.0;
                    }

                    return std::sqrt(v);
                };

                auto clamp_intensity = [](double v) -> double 
                {
                    if (v < 0.0)
                    {
                        return 0.0;
                    }

                    if (v > 0.999)
                    {
                        return 0.999;
                    }

                    return v;
                };

                double r = pixel_samples_scale * pixel_colour.x();
                double g = pixel_samples_scale * pixel_colour.y();
                double b = pixel_samples_scale * pixel_colour.z();

                r = gamma_correct(r);
                g = gamma_correct(g);
                b = gamma_correct(b);

                int rbyte = static_cast<int>(256 * clamp_intensity(r));
                int gbyte = static_cast<int>(256 * clamp_intensity(g));
                int bbyte = static_cast<int>(256 * clamp_intensity(b));

                int index = (j * image_width + i) * 3;
                image[index + 0] = static_cast<unsigned char>(rbyte);
                image[index + 1] = static_cast<unsigned char>(gbyte);
                image[index + 2] = static_cast<unsigned char>(bbyte);
            }
        }

        // PNG
        if (!stbi_write_png(filename.c_str(), image_width, image_height, 3, image.data(), image_width * 3))
        {
            std::cerr << "Failed to write PNG file\n";
        }
        else
        {
            std::clog << "\nWrote " << filename << '\n';
        }
    }
    else {
        // PMM
        std::ofstream out(filename, std::ios::out | std::ios::binary);
        if (!out)
        {
            std::cerr << "Failed to open output file for PPM\n";
            return;
        }

        out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++)
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

            for (int i = 0; i < image_width; i++)
            {
                colour pixel_colour(0, 0, 0);

                for (int sample = 0; sample < samples_per_pixel; sample++)
                {
                    ray r = get_ray(i, j);
                    pixel_colour += ray_colour(r, max_depth, world);
                }

                write_colour(out, pixel_samples_scale * pixel_colour);
            }
        }

        out.close();
        std::clog << "\nWrote " << filename << '\n';
    }
}

void camera::initialize()
{
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    pixel_samples_scale = 1.0 / samples_per_pixel;

    center = lookfrom;

    // Determine viewport dimensions.
    auto theta = degrees_to_radians(vfov);
    auto h = std::tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;
    auto viewport_width = viewport_height * (double(image_width) / image_height);

    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
    vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Calculate the camera defocus disk basis vectors.
    auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
}

colour camera::ray_colour(const ray& r, int depth, const hittable& world) const 
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
    {
        return colour(0, 0, 0);
    }

    hit_record rec;

    // If the ray hits nothing, return the background colour.
    if (!world.hit(r, interval(0.001, infinity), rec))
    {
        return background;
    }

    ray scattered;
    colour attenuation;
    colour colour_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat->scatter(r, rec, attenuation, scattered))
    {
        return colour_from_emission;
    }

    colour colour_from_scatter = attenuation * ray_colour(scattered, depth - 1, world);

    return colour_from_emission + colour_from_scatter;
}

// Global frame time variable
extern double g_frame_time; 

ray camera::get_ray(int i, int j) const 
{
    // Construct a camera ray originating from the origin and directed at randomly sampled
    // point around the pixel location i, j.

    auto offset = sample_square();
    auto pixel_sample = pixel00_loc
        + ((i + offset.x()) * pixel_delta_u)
        + ((j + offset.y()) * pixel_delta_v);

    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;
    auto ray_time = g_frame_time;

    return ray(ray_origin, ray_direction, ray_time);

    return ray(ray_origin, ray_direction);
}

vec3 camera::sample_square() const 
{
    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
}

point3 camera::defocus_disk_sample() const 
{
    // Returns a random point in the camera defocus disk.
    auto p = vec3::random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
}
