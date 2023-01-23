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
#include "../headers/renderer.hpp"
int main()
{
    Matrix4x4f c2w = Matrix4x4f(1.0f, 0.0f, 0.0f, 0.0f,//kierunek osi x(boki) 
                                0.0f, 1.0f, 0.0f, 0.0f,//kierunek osi y (dol-gora)
                                0.0f, 0.0f, 1.0f, 0.0f,//kierunek patrzenia z
                                0.0f, 0.0f, 72.0f, 1.0f);//od lewej: przesunięcie na boki, przesunięcie dół-góra, odległość my-centrum;
    Options options(1024, 768, 6.65f, c2w);
    Renderer renderer(options);
    do
    {
        std::string modelpath;
        int rtrak = -1;
        uint32_t samples = 5;
        std::string answer;
        std::cout << "Add path to 3D scene" << std::endl;
        std::cin >> modelpath;
        std::cout << "Choose number of samples" << std::endl;
        std::cin >> samples;
        if(samples<1 || samples >100)
        {
            samples = 5;
        }
        std::cout << "Choose an accelerating structure: 0 -raw PATHTracing 1 - BVH, 2 - K-D Tree, 3 - grid" << std::endl;
        std::cin >> rtrak;
        while (rtrak != 0 &&rtrak != 1 && rtrak != 2 && rtrak != 3)
        {
            std::cout << "There is no such accelerating strusture.\nChoose an accelerating structure: 1 - BVH, 2 - K-D Tree, 3 - grid" << std::endl;
            std::cin >> rtrak;
        }
        std::string render;
        std::cout << "Add name to save file" << std::endl;
        std::cin >> render;
        std::cout << "Model path: " << modelpath << "\nAccelerating strusture: " << rtrak << "\nDo you want to continue? (y/n)" << std::endl;
        std::cin >> answer;
        if (answer == "y")
        {
            renderer.load_scene(modelpath);
            if(rtrak == 0)
            {
                renderer.render(render,samples);
            }
            else if(rtrak == 1)
            {
                renderer.renderBVHtree(render,samples);
            }
            else if(rtrak == 2)
            {
                renderer.renderKDTree(render,samples);
            }
            else if(rtrak == 3)
            {
                renderer.renderBinTree(render,samples);
            }
        }
        std::cout <<"\nDo you want to continue with new simulations? (y/n)" << std::endl;
        std::cin>>answer;
        if(answer =="n")
        {
            break;
        }
    } while (true);
    return 0;
}


