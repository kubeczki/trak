#pragma once
#include <vector>
#include "shape.hpp"

class Scene : public Shape
{
    public:
    Scene() {}
    Scene(std::shared_ptr <Shape> object){Add(object);}

    void clear();
    void Add(std::shared_ptr<Shape>object);

    virtual bool intersect(const Ray& r, SurfaceInteraction &interaction) const;
    public:
    std::vector<std::shared_ptr<Shape>> objects;
};

void Scene::clear()
{
    objects.clear();
}

void Scene::Add(std::shared_ptr<Shape> object)
{
    objects.push_back(object);
}

bool Scene::intersect(const Ray &r, SurfaceInteraction &interaction) const
{
    SurfaceInteraction temp;
    bool any_hit = false;
    float t_near = r.t_max;

    for(const auto& object : objects)
    {
        if(object->intersect(r,temp))
        {
            any_hit = true;
            t_near = temp.t;
            interaction = temp;
        }
    }

    return any_hit;
}
