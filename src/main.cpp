#include "../headers/math.hpp"
#include "../headers/camera.hpp"
#include "../headers/scene.hpp"
#include "../headers/triangle.hpp"
#include "../headers/sphere.hpp"
#include "../headers/triangleMesh.hpp"
#include <chrono>
#include <vector>
#include <fstream>
#include <stdlib.h>

struct Options 
{
    uint32_t width;
    uint32_t height;
    float fov;
    Matrix4x4f c2w;

    Options(uint32_t width, uint32_t height, float fov, Matrix4x4f c2w)
        :width(width), height(height), fov(fov), c2w(c2w) {};
};

int main()
{
    Matrix4x4f c2w = Matrix4x4f(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, -60.0f, 1.0f);
    Options options(1024, 768, 6.65f, c2w);
    Camera camera(options.width,options.height,options.fov,options.c2w);
    Ray camera_dir = camera.get_ray(options.width / 2, options.height / 2);
    
    Scene scene;
    scene.Add(std::make_shared<TriangleMesh>("../obj/malaScenaPlus.obj"));
    
    //scene.Add(std::make_shared<Sphere>(Vector3f(0.0f,0.0f,-50.0f),0.1f));
    std::cout <<"Ray Tracing!" <<std::endl;
    std::ofstream ofs("render.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << options.width << " " << options.height << "\n255\n";
    Vec cx = Vec(options.width * .5135 / options.height);		// x-direction increment (uses implicit 0 for y&z)
    Vec cy = (cx % (Vec(camera_dir.d)).norm()) * .5135;						// y-direction increment (note cross pdt - '%')

    const static float gamma = 1.0f;

    auto start = std::chrono::system_clock::now();
    std::cout <<"Rendering image ... \n";

    uint32_t samples = 100;
    uint32_t depth = 5;

    uint64_t total_pixels = uint64_t(options.height) * uint64_t(options.width);
    uint64_t current_pixel = 0;
    int current_progress = 0;

#pragma omp parallel for schedule(dynamic, 1) private(colour)
    for(uint32_t y = 0; y< options.height; ++y)
    {
        if (current_progress != int(100 * current_pixel / total_pixels)) {
            std::cout << "Current progress: " << 100 * current_pixel / total_pixels << "%\n";
            current_progress = int(100 * current_pixel / total_pixels);
        }
        unsigned short Xi[3] = { 0,0,y * y * y };
        for (uint32_t x = 0; x < options.width; ++x)
        {
            Vector3f colour = Vector3f();
            for (uint32_t s = 0; s < samples; s++) {
                Ray ray = camera.get_ray(x, y);
                colour += (scene.trace_ray(ray, depth, Xi) * (1. / samples)).cast_to_3f();
            }

            char r = char(255.99f * powf(colour.x, gamma));
            char g = char(255.99f * powf(colour.y, gamma));
            char b = char(255.99f * powf(colour.z, gamma));

            ofs << r << g << b;
            
            current_pixel++;
        }
    }

    auto end = std::chrono::system_clock::now();

    std::cout <<"Rendering Complete!\n";
    ofs.close();

    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Rendering Completed on " << std::ctime(&end_time) << "\nTime taken to render: " << elapsed_seconds.count() << "s\n";
    return 0;
}
