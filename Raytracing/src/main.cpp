#include "common.h"

#include "bvh.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "quad.h"
#include "triangle.h"
#include "constant_medium.h"

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
    cam.background = colour(0.70, 0.80, 1.00);

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
    cam.background = colour(0.70, 0.80, 1.00);

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
    cam.background = colour(0.70, 0.80, 1.00);

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
    cam.background = colour(0.70, 0.80, 1.00);

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    render(cam, world);
}

void quads() 
{
    hittable_list world;

    // Materials
    auto left_red = make_shared<lambertian>(colour(1.0, 0.2, 0.2));
    auto back_green = make_shared<lambertian>(colour(0.2, 1.0, 0.2));
    auto right_blue = make_shared<lambertian>(colour(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(colour(1.0, 0.5, 0.0));
    auto lower_teal = make_shared<lambertian>(colour(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quad>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = colour(0.70, 0.80, 1.00);

    cam.vfov = 80;
    cam.lookfrom = point3(0, 0, 9);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    render(cam, world);
}

void simple_light() {
    hittable_list world;

    auto pertext = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(colour(4, 4, 4));
    world.add(make_shared<triangle>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 1, 0), difflight));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = colour(0, 0, 0);

    cam.vfov = 20;
    cam.lookfrom = point3(26, 3, 6);
    cam.lookat = point3(0, 2, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    render(cam, world);
}

void cornell_box() 
{
    hittable_list world;

    auto red = make_shared<lambertian>(colour(.65, .05, .05));
    auto white = make_shared<lambertian>(colour(.73, .73, .73));
    auto green = make_shared<lambertian>(colour(.12, .45, .15));
    auto light = make_shared<diffuse_light>(colour(15, 15, 15));
    auto glass = make_shared<dielectric>(1.5);
    auto water = make_shared<dielectric>(1.3);

    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), glass);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    world.add(box1);

    shared_ptr<hittable> box3 = box(point3(0, 0, 0), point3(145, 310, 145), water);
    box3 = make_shared<rotate_y>(box3, 15);
    box3 = make_shared<translate>(box3, vec3(265, 20, 295));
    world.add(box3);

    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    world.add(box2);

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;
    cam.background = colour(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    render(cam, world);
}

void cornell_smoke() 
{
    hittable_list world;

    auto red = make_shared<lambertian>(colour(.65, .05, .05));
    auto white = make_shared<lambertian>(colour(.73, .73, .73));
    auto green = make_shared<lambertian>(colour(.12, .45, .15));
    auto light = make_shared<diffuse_light>(colour(7, 7, 7));

    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(113, 554, 127), vec3(330, 0, 0), vec3(0, 0, 305), light));
    world.add(make_shared<quad>(point3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    world.add(make_shared<constant_medium>(box1, 0.01, colour(0, 0, 0)));
    world.add(make_shared<constant_medium>(box2, 0.01, colour(1, 1, 1)));

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth = 50;
    cam.background = colour(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    render(cam, world);
}

int main()
{
    switch (7) 
    {
    case 1:  bouncing_spheres();  break;
    case 2:  checkered_spheres(); break;
    case 3:  earth();             break;
    case 4:  perlin_spheres(); break;
    case 5: quads(); break;
    case 6: simple_light(); break;
    case 7: cornell_box(); break;
    case 8:  cornell_smoke(); break;
    }

    return 0;
}