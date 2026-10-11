#include "common.h"

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

#include <string>
#include <iostream>

int main()
{
    hittable_list world;

    auto ground_material = make_shared<lambertian>(colour(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    // Glass
    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(1.0, 0.5, -1.0), 0.5, material1));

    // Diffuse
    auto material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(-3.0, 0.5, 0.0), 0.5, material3));

    // Metal
    auto material2 = make_shared<lambertian>(colour(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-1.0, 0.5, -0.5), 0.5, material2));

    camera cam;

    // Control image output
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1200;
    cam.samples_per_pixel = 20;
    cam.max_depth = 50;

    // Control camera angle
    cam.vfov = 10;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 1, 0);
    cam.vup = vec3(0, 1, 0);

    // Control defocus blur
    cam.defocus_angle = 0.1;
    cam.focus_dist = 15.0;

    // Prompt user to choose PNG or PPM
    std::string format;
    std::cout << "Choose output format ('png' or 'ppm') [png]: ";
    std::getline(std::cin, format);
    if (format.empty()) format = "png";
    for (auto &c : format) c = static_cast<char>(std::tolower(c));

    std::string filename;
    std::string default_name = (format == "ppm") ? "output.ppm" : "output.png";
    std::cout << "Enter filename [" << default_name << "]: ";
    std::getline(std::cin, filename);
    if (filename.empty()) filename = default_name;

    cam.render(world, format, filename);

    // Open output when finished
    std::string cmd;
#ifdef _WIN32
    cmd = "start \"\" \"" + filename + "\"";
#endif
    std::system(cmd.c_str());
}