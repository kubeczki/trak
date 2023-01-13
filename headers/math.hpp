#pragma once

#define _USE_MATH_DEFINES
#include <iostream>
#include "../external/glm/glm/glm.hpp"
#include "../external/glm/glm/gtx/string_cast.hpp"
#include "../external/glm/glm/gtx/norm.hpp"
typedef glm::vec2 Vector2f;
typedef glm::vec3 Vector3f;
typedef glm::vec4 Vector4f;
typedef glm::mat4 Matrix4x4f;

inline Vector3f TransformDirMatrix(const Matrix4x4f &x, const Vector3f &src)
{
    Vector3f dst;
    Vector4f temp = glm::normalize(x*Vector4f(src,0.0f));

    dst.x = temp.x;
    dst.y = temp.y;
    dst.z = temp.z;
    return dst;
}

inline Vector3f TransformPointMatrix(const Matrix4x4f &x, const Vector3f &src)
{
    Vector3f dst;
    Vector4f temp = x*Vector4f(src,1.0f);

    dst.x = temp.x;
    dst.y = temp.y;
    dst.z = temp.z;
    return dst;
}

class Ray 
{
public:
    Ray() {}
    Ray(const Vector3f& orig, const Vector3f& dir,const float& tmin, const float& tmax) : o(orig), d(dir), t_min(tmin), t_max(tmax){}
public :
    Vector3f o;
    Vector3f d;
    mutable float t_min;
    mutable float t_max;
};

struct SurfaceInteraction
{
    Vector3f p; 
    Vector3f Ng; 
    float t; 
    bool front_facing;
    Vector3f AOV;
    Vector2f st;
    inline void set_face_normal(const Ray& r, const Vector3f& outward_normal)
    {
        front_facing = glm::dot(r.d, outward_normal) <0.0f;
        Ng = front_facing ? outward_normal : - outward_normal;
    }
};

float ray_sphere_intersection(const Vector3f& center, float radius, const Ray& r)
{
    Vector3f oc = r.o-center;
    float a = glm::dot(r.d,r.d);
    float b = 2.0f * glm::dot(oc,r.d);
    float c = glm::dot(oc,oc) - radius * radius;
    float discriminant = b*b - 4*a*c;
    if(discriminant < 0.0f)
    {
        return -1.0f;
    }
    else
    {
        return (-b - sqrt(discriminant))/(2.0f*a);
    }
}

Vector3f cast_ray(const Ray& r)
{
    float hit = ray_sphere_intersection(Vector3f(0.0f,0.0f,-1.0f),0.5f,r);
    if(hit> 0.0f)
    {
        Vector3f normal = glm::normalize((r.o +hit*r.d)-Vector3f(0.0f,0.0f,-1.0f));
        return normal;
    }
    return Vector3f(0.18f);
}

inline float deg2rad(const float &deg) { return deg * M_PI / 180.0f; } 
