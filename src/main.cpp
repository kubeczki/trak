#include "../headers/math.hpp"
#include "../headers/camera.hpp"
#include "../headers/scene.hpp"
#include "../headers/triangle.hpp"
#include "../headers/sphere.hpp"
#include "../headers/triangleMesh.hpp"
#include <chrono>
#include <vector>
#include <fstream>
struct Options 
{
    uint32_t width;
    uint32_t height;
    float fov;
    Matrix4x4f c2w;
};

Vector3f cast_ray(const Ray& r, const Shape& scene)
{
    SurfaceInteraction interaction;
    if(scene.intersect(r,interaction))
    {
        return Vector3f(interaction.AOV);
    }
    return Vector3f(0.18f);
}

int main()
{
    Options options;
    options.height = 1024;
    options.width = 768;
    options.fov = 6.65f;
    options.c2w = Matrix4x4f(1.0f, 0.0f,0.0f, 0.0f,
                                   0.0f, -1.0f,0.0f, 0.0f,
                                   0.0f, 0.0f,1.0f, 0.0f,
                                   0.0f, 0.0f,-60.0f,1.0f);
    Camera camera(options.width,options.height,options.fov,options.c2w);
    
    Scene scene;
    scene.Add(std::make_shared<TriangleMesh>("../obj/malaScenaPlus.obj"));
    
    //scene.Add(std::make_shared<Sphere>(Vector3f(0.0f,0.0f,-50.0f),0.1f));
    std::cout <<"RayTracing!" <<std::endl;
    std::ofstream ofs("render.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" <<options.width << " " << options.height << "\n255\n";

    const static float gamma = 1.0f;

    auto start = std::chrono::system_clock::now();
    std::cout <<"Rendering image ... \n";

    for(uint32_t j = 0; j< options.height; ++j)
    {
        for(uint32_t i = 0; i< options.width; ++i)
        {
            Ray ray = camera.get_ray(i,j);
            Vector3f colour = cast_ray(ray,scene);
            char r = char(255.99f*powf(colour.x,gamma));
            char g = char(255.99f*powf(colour.y,gamma));
            char b = char (255.99f*powf(colour.z,gamma));

            ofs <<r << g << b;
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
