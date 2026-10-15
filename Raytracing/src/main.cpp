#include "common.h"

#include "bvh.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "texture.h"

#include <string>
#include <iostream>
#include <cstdlib>

void render(camera cam, hittable_list world)
{
    // Prompt for PNG or PPM
    std::string format;
    std::cout << "Choose output format ('png' or 'ppm') [png]: ";
    std::getline(std::cin, format);
    if (format.empty()) format = "png";
    for (auto& c : format) c = static_cast<char>(std::tolower(c));

    // Prompt for filename
    std::string filename;
    std::string default_name = (format == "ppm") ? "output.ppm" : "output.png";
    std::cout << "Enter filename [" << default_name << "]: ";
    std::getline(std::cin, filename);
    if (filename.empty()) filename = default_name;

    {
        auto pos = filename.find_last_of('.');
        if (pos == std::string::npos) {
            filename += "." + format;
        }
        else {
            std::string current_ext = filename.substr(pos + 1);
            for (auto& c : current_ext) c = static_cast<char>(std::tolower(c));
            if (current_ext != format) {
                filename = filename.substr(0, pos) + "." + format;
            }
        }
    }

    // Prompt for number of frames
    std::cout << "Number of frames to render [1]: ";
    std::string frames_str;
    std::getline(std::cin, frames_str);
    int frames = 1;
    if (!frames_str.empty())
    {
        try
        {
            frames = std::stoi(frames_str);
            if (frames < 1) frames = 1;
        }
        catch (...) { frames = 1; }
    }

    double fps = 30.0;

    if (frames > 1)
    {
        // Prompt for FPS
        std::cout << "Output frames per second (FPS) [30]: ";
        std::string fps_str;
        std::getline(std::cin, fps_str);
        if (!fps_str.empty())
        {
            try
            {
                fps = std::stod(fps_str);
                if (fps <= 0.0) fps = 30.0;
            }
            catch (...) { fps = 30.0; }
        }
    }

    if (frames <= 1) {
        // Single frame uses random time
        g_frame_time = random_double();
        cam.render(world, format, filename);

        // Open output when finished
        std::string cmd;
#ifdef _WIN32
        cmd = "start \"\" \"" + filename + "\"";
#endif
        std::system(cmd.c_str());
    }
    else
    {
        std::string base = filename;
        std::string ext = format;

        for (int f = 0; f < frames; ++f)
        {
            double frame_time_sec = static_cast<double>(f) / fps;

            double tnorm = (frames == 1) ? 0.0 : static_cast<double>(f) / static_cast<double>(frames - 1);

            double t_pingpong = (tnorm <= 0.5) ? (tnorm * 2.0) : (2.0 - tnorm * 2.0);

            g_frame_time = t_pingpong;

            char buf[512];
            std::snprintf(buf, sizeof(buf), "%s_%04d.%s", base.c_str(), f, ext.c_str());
            std::string outname(buf);

            std::cout << "Rendering frame " << f << " / " << (frames - 1) << "  (time=" << frame_time_sec << "s, norm=" << t_pingpong << ")\n";
            cam.render(world, format, outname);
        }
    }
}

void bouncing_spheres()
{
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, colour(.2, .3, .1), colour(.9, .9, .9));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = colour::random() * colour::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0, .5), 0);
                    world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = colour::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(colour(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    world = hittable_list(make_shared<bvh_node>(world));

    camera cam;

    // Control image output
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 10;
    cam.max_depth = 50;

    // Control camera angle
    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    // Control defocus blur
    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    render(cam, world);
}

void checkered_spheres() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, colour(.2, .3, .1), colour(.9, .9, .9));

    world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    render(cam, world);
}

void earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(0, 0, 12);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    render(cam, hittable_list(globe));
}

void perlin_spheres() 
{
    hittable_list world;

    auto pertext = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    render(cam, world);
}

int main()
{
    switch (4) 
    {
    case 1:  bouncing_spheres();  break;
    case 2:  checkered_spheres(); break;
    case 3:  earth();             break;
    case 4:  perlin_spheres(); break;
    }

    return 0;
}