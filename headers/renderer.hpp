#pragma once
#include "scene.hpp"
#include "camera.hpp"
#include <chrono>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include "../headers/binTree.hpp"
#include "../headers/bvh.hpp"
#include "../headers/kdTree.hpp"
struct Options 
{
    uint32_t width;
    uint32_t height;
    float fov;
    Matrix4x4f c2w;

    Options(uint32_t width, uint32_t height, float fov, Matrix4x4f c2w)
        :width(width), height(height), fov(fov), c2w(c2w) {};
};

class Renderer
{
    public:
        Camera camera;
        Scene scene;
         std::vector<std::string>scene_obj;
         Options options;
         //BvhNode bvhNode;
         int strukt;
    public:
        Renderer(Options _options): camera(_options.width,_options.height,_options.fov,_options.c2w),options{_options.width,_options.height,_options.fov,_options.c2w}
        {};
        int load_scene(const std::string filename)
        {   
            scene_obj.clear();
            scene.clear();
            std::ifstream myfile (filename);
            if (myfile.is_open())
            {
                std::string line;
                while ( std::getline (myfile,line) )
                {
                 scene_obj.push_back(line);
                }
                myfile.close();
                if(scene_obj.size() == 0)
                {
                    std::cout << "No objects in scene! Aborting" << std::endl;
                    return -1;
                }
                 for(int i =0; i<scene_obj.size(); i++)
                {
                    scene.Add(std::make_shared<TriangleMesh>(scene_obj[i].c_str()));
                }
                scene.printBox();
                return 0;
            }
            else
            {
                std::cout << "No such scene: " <<filename<< " found!" << std::endl;
                return -1;
            }
            
        }
        //int render(int strukt,string filename, sample) 
        //render_BVH(); // oblicza czas ładowania struktury + oblicza czas renderowania
        
        
        int renderKDTree(std::string filename,uint32_t _sample)
        {
            int nTriangles = 0;
           std::vector<Triangle> tri;
           for(int i =0; i<scene.objects.size();i++)
           {
               nTriangles += scene.objects[i]->tris.size();
               for(int j = 0; j<scene.objects[i]->tris.size();j++)
               {
                   tri.push_back(scene.objects[i]->tris[j]);
               }
           }
            auto startStr = std::chrono::system_clock::now();
           
           std::vector<Pack> packs;
           
           for(int i = 0; i< nTriangles;i++)
           {
                packs.push_back(makePack(tri[i]));
           }
            KdNode kd(packs,scene.range,scene.biggest);
            auto endStr = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_secondsStr = endStr - startStr;
            std::cout <<"Struktura KDTree: czas ladowania: "<< elapsed_secondsStr.count() <<"s." <<std::endl;
            std::cout <<"PATH Tracing!" <<std::endl;
            Ray camera_dir = camera.get_ray(options.width / 2, options.height / 2);
            std::ofstream ofs(filename, std::ios::out | std::ios::binary);
            ofs << "P6\n" << options.width << " " << options.height << "\n255\n";
            Vec cx = Vec(options.width * .5135 / options.height);		// x-direction increment (uses implicit 0 for y&z)
            Vec cy = (cx % (Vec(camera_dir.d)).norm()) * .5135;						// y-direction increment (note cross pdt - '%')

            const static float gamma = 1.0f;
            auto start = std::chrono::system_clock::now();
            std::cout <<"Rendering image ... \n";

            uint32_t samples = _sample;
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
                        colour += (scene.trace_rayKD(ray, depth, Xi,kd) * (1. / samples)).cast_to_3f();
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
            scene.clear();
           return 0;
        }


        int renderBVHtree(std::string filename,uint32_t _sample)
        {
            int nTriangles = 0;
           std::vector<Triangle> tri;
           for(int i =0; i<scene.objects.size();i++)
           {
               nTriangles += scene.objects[i]->tris.size();
               for(int j = 0; j<scene.objects[i]->tris.size();j++)
               {
                   tri.push_back(scene.objects[i]->tris[j]);
               }
           }
            auto startStr = std::chrono::system_clock::now();
           
           std::vector<Pack> packs;
           
           for(int i = 0; i< nTriangles;i++)
           {
                packs.push_back(makePack(tri[i]));
           }
            BvhNode bvh(packs,scene.range,scene.biggest);
            auto endStr = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_secondsStr = endStr - startStr;
            std::cout <<"Struktura BVH: czas ladowania: "<< elapsed_secondsStr.count() <<"s." <<std::endl;
            std::cout <<"PATH Tracing!" <<std::endl;
            Ray camera_dir = camera.get_ray(options.width / 2, options.height / 2);
            std::ofstream ofs(filename, std::ios::out | std::ios::binary);
            ofs << "P6\n" << options.width << " " << options.height << "\n255\n";
            Vec cx = Vec(options.width * .5135 / options.height);		// x-direction increment (uses implicit 0 for y&z)
            Vec cy = (cx % (Vec(camera_dir.d)).norm()) * .5135;						// y-direction increment (note cross pdt - '%')

            const static float gamma = 1.0f;
            auto start = std::chrono::system_clock::now();
            std::cout <<"Rendering image ... \n";

            uint32_t samples = _sample;
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
                        colour += (scene.trace_rayBVH(ray, depth, Xi,bvh) * (1. / samples)).cast_to_3f();
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
            scene.clear();
           return 0;
        }



        int renderBinTree(std::string filename,uint32_t _sample)
       {
           int nTriangles = 0;
           std::vector<Triangle> tri;
           for(int i =0; i<scene.objects.size();i++)
           {
               nTriangles += scene.objects[i]->tris.size();
               for(int j = 0; j<scene.objects[i]->tris.size();j++)
               {
                   tri.push_back(scene.objects[i]->tris[j]);
               }
           }
            auto startStr = std::chrono::system_clock::now();
           BinTree bintree(nTriangles, scene.range);
           std::vector<Pack> packs;
           for(int i = 0; i< nTriangles;i++)
           {
                packs.push_back(makePack(tri[i]));
           }

           bintree.fill(packs);
           auto endStr = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_secondsStr = endStr - startStr;
            std::cout <<"Struktura kraty: czas ladowania: "<< elapsed_secondsStr.count() <<"s." <<std::endl;
            std::cout <<"PATH Tracing!" <<std::endl;
            Ray camera_dir = camera.get_ray(options.width / 2, options.height / 2);
            std::ofstream ofs(filename, std::ios::out | std::ios::binary);
            ofs << "P6\n" << options.width << " " << options.height << "\n255\n";
            Vec cx = Vec(options.width * .5135 / options.height);		// x-direction increment (uses implicit 0 for y&z)
            Vec cy = (cx % (Vec(camera_dir.d)).norm()) * .5135;						// y-direction increment (note cross pdt - '%')

            const static float gamma = 1.0f;
            std::cout<<bintree.getDim()<<std::endl;
            auto start = std::chrono::system_clock::now();
            std::cout <<"Rendering image ... \n";

            uint32_t samples = _sample;
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
                        colour += (scene.trace_rayGrid(ray, depth, Xi,bintree) * (1. / samples)).cast_to_3f();
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
            scene.clear();
           return 0;
       }
        
        int render(std::string filename,uint32_t _sample)
        {
            if(scene_obj.size() == 0)
            {
                std::cout << "No objects in scene! Aborting" << std::endl;
                return -1;
            }
                std::cout <<"PATH Tracing!" <<std::endl;
                Ray camera_dir = camera.get_ray(options.width / 2, options.height / 2);
            std::ofstream ofs(filename, std::ios::out | std::ios::binary);
            ofs << "P6\n" << options.width << " " << options.height << "\n255\n";
            Vec cx = Vec(options.width * .5135 / options.height);		// x-direction increment (uses implicit 0 for y&z)
            Vec cy = (cx % (Vec(camera_dir.d)).norm()) * .5135;						// y-direction increment (note cross pdt - '%')

            const static float gamma = 1.0f;

            auto start = std::chrono::system_clock::now();
            std::cout <<"Rendering image ... \n";

            uint32_t samples = _sample;
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
            scene.clear();
            return 0;
        }

};