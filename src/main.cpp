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
#include <iostream>
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
    //Cameratoview: to ustawienie jest dość dobre dla scen w blenderze exportowanych z ustawieniami jak w teamsach
    Matrix4x4f c2w = Matrix4x4f(1.0f, 0.0f, 0.0f, 0.0f,//kierunek osi x(boki) 
                                0.0f, 1.0f, 0.0f, 0.0f,//kierunek osi y (dol-gora)
                                0.0f, 0.0f, 1.0f, 0.0f,//kierunek patrzenia z
                                0.0f, 0.0f, 72.0f, 1.0f);//od lewej: przesunięcie na boki, przesunięcie dół-góra, odległość my-centrum;
    Options options(1024, 768, 6.65f, c2w);
    Camera camera(options.width,options.height,options.fov,options.c2w);
    Ray camera_dir = camera.get_ray(options.width / 2, options.height / 2);
    
    std::vector<std::string>scene_obj;
    std::ifstream myfile ("../obj/ScenaMala.txt");
    if (myfile.is_open())
    {
        std::string line;
        while ( std::getline (myfile,line) )
        {
         scene_obj.push_back(line);
        }
        myfile.close();
    }
    else
    {
        std::cout << "No such scene found!" << std::endl;
    }

    Scene scene;
    //scene.Add(std::make_shared<TriangleMesh>("../obj/cornellBox/MalaScenaSwiatlo.obj"));
    for(int i =0; i<scene_obj.size(); i++)
    {
        scene.Add(std::make_shared<TriangleMesh>(scene_obj[i].c_str()));
    }
    std::cout <<"Ray Tracing!" <<std::endl;
    std::ofstream ofs("render.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << options.width << " " << options.height << "\n255\n";
    Vec cx = Vec(options.width * .5135 / options.height);		// x-direction increment (uses implicit 0 for y&z)
    Vec cy = (cx % (Vec(camera_dir.d)).norm()) * .5135;						// y-direction increment (note cross pdt - '%')

    const static float gamma = 1.0f;

    auto start = std::chrono::system_clock::now();
    std::cout <<"Rendering image ... \n";

    uint32_t samples = 5;
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
