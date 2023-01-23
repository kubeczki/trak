#pragma once
#include "triangle.hpp"
#include <vector>

struct Range
{
    float x_min;
    float y_min;
    float z_min;
    float x_max;
    float y_max;
    float z_max;
};

struct Pack
{
    Triangle triangle;
    Range range;
};

Pack makePack(Triangle tri)
{
    Range r;
    r.x_min = std::min(tri.v0.x, std::min(tri.v1.x, tri.v2.x));
    r.y_min = std::min(tri.v0.y, std::min(tri.v1.y, tri.v2.y));
    r.z_min = std::min(tri.v0.z, std::min(tri.v1.z, tri.v2.z));

    r.x_max = std::max(tri.v0.x, std::max(tri.v1.x, tri.v2.x));
    r.y_max = std::max(tri.v0.y, std::max(tri.v1.y, tri.v2.y));
    r.z_max = std::max(tri.v0.z, std::max(tri.v1.z, tri.v2.z));

    Pack p = { tri, r };
    return p;
};

bool comp_x(Pack a, Pack b)
{
    return (a.range.x_min < b.range.x_min);
}

bool comp_y(Pack a, Pack b)
{
    return (a.range.y_min < b.range.y_min);
}

bool comp_z(Pack a, Pack b)
{
    return (a.range.z_min < b.range.z_min);
}

Vector3f findBiggest(std::vector<Pack> list)
{
    Vector3f biggest = {0, 0, 0};
    for (int i = 0; i < list.size(); i++)
    {
        if (list[i].range.x_max - list[i].range.x_min > biggest.x)
        {
            biggest.x = list[i].range.x_max - list[i].range.x_min;
        }
        if (list[i].range.y_max - list[i].range.y_min > biggest.y)
        {
            biggest.y = list[i].range.y_max - list[i].range.y_min;
        }
        if (list[i].range.z_max - list[i].range.z_min > biggest.z)
        {
            biggest.z = list[i].range.z_max - list[i].range.z_min;
        }
    }
    return biggest;
}