#pragma once
#include "math.hpp"
class Shape
{
public:
    virtual bool intersect(const Ray& r, SurfaceInteraction& interaction) const = 0;
    //virtual Range getRange() const = 0;
};