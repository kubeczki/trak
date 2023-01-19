#pragma once
#include "math.hpp"

float float_rand() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

float lil_rand_float() {
    return (float_rand() - 0.5f) * 0.025f;
}

class Camera
{
    public:
        Camera(uint32_t _width, uint32_t _height, float _fov, Matrix4x4f _c2w);
        Ray get_ray(float u, float v);

    private:
        uint32_t width;
        uint32_t height;
        float fov;
        Matrix4x4f c2w;
        float scale;
        float image_aspect_ratio;
};

Camera::Camera(uint32_t _width, uint32_t _height, float _fov, Matrix4x4f _c2w)
{
    width = _width;
    height = _height;
    fov = _fov;
    c2w = _c2w;

    scale = tan (deg2rad(fov * 0.5f));
    image_aspect_ratio = width/ (float) height;
}

Ray Camera::get_ray(float u, float v)
{
    float ndc_x = (2.0f * (u + 0.5f + lil_rand_float())/(float)width - 1.0f)*scale;
    float ndc_y = (1.0f - 2.0f * (v  +0.5f + lil_rand_float())/(float)height)*scale*1.0f/image_aspect_ratio;
    Ray r;
    Vector3f orig = Vector3f(0.0f + lil_rand_float(),0.0f + lil_rand_float(), 0.0f);
    Vector3f dir = glm::normalize(Vector3f(ndc_x, ndc_y,-1.0f));
    
    r.o = TransformPointMatrix(c2w,orig);
    r.d = TransformDirMatrix(c2w,dir);
    r.t_max = 9999.9f;
    r.t_min = 0.0001f;

    return r;
}
