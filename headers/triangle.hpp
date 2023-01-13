#pragma once
#include "shape.hpp"

struct Vertex
{
    Vector3f Pos;
    Vector3f Normal;
    Vector2f UV;
};

struct Color
{
    Vector3f ambient;
    Vector3f diffuse;
    Vector3f specular;
    Vector3f emissive;
};
class Triangle : public Shape
{public:
    Triangle(Vertex _v0, Vertex _v1, Vertex _v2, Color _color)
    {
        v0 = _v0.Pos;
        v1 = _v1.Pos;
        v2 = _v2.Pos;
        edge1 = v1 - v0;
        edge2 = v2 - v0;
        n= glm::normalize(glm::cross(edge1,edge2));
        uv[0] = _v0.UV;
        uv[1] = _v1.UV;
        uv[2] = _v2.UV;
        N[0] = _v0.Normal;
        N[1] = _v1.Normal;
        N[2] = _v2.Normal;
        color = _color;
    }
    void Triangle::printColor() const
    {
        std::cout <<color.ambient[0] << std::endl;
        std::cout <<color.emissive[2] << std::endl;
        std::cout <<color.specular[1] << std::endl;
        std::cout <<color.diffuse[2] << std::endl;
    }
    bool Triangle::intersect(const Ray &r,SurfaceInteraction &interaction) const
    {
        float u,v,t_temp = 0.0f;
        Vector3f pvec = glm::cross(r.d, edge2);
        float det = glm::dot(edge1,pvec);
        if(det == 0.0f) return false;
        float inv_det = 1.0f/det;
        Vector3f tvec = r.o - v0;
        u = glm::dot(tvec,pvec) * inv_det;
        if(u < 0.0f || u> 1.0f) return false;
        Vector3f qvec =glm::cross(tvec,edge1);
        v = glm::dot(r.d,qvec)*inv_det;
        if(v < 0.0f || u+ v > 1.0f) return false;
        t_temp = glm::dot(edge2,qvec)*inv_det;
        if(t_temp < r.t_max)
        {
            if(t_temp < r.t_min)
            {
                interaction.t = t_temp;
                interaction.p = r.o + interaction.t * r.d;
                Vector3f outward_normal = this -> n;
                interaction.Ng = this->n;
                
                Vector3f bary = get_barycentric(interaction.p);
                interaction.Ng = glm::normalize((bary.x*N[0])+(bary.y*N[1]) + bary.z * N[2]);
                interaction.set_face_normal(r, outward_normal);
                interaction.AOV = color.diffuse;
                //Vector2f ST = bary.x * uv[0] + bary.y * uv[1] + bary.z * uv[2];
                //interaction.AOV = Vector3f(ST.x, ST.y, 0.0f);
                //interaction.AOV = glm::normalize((bary.x * N[0]) + (bary.y * N[1]) + bary.z * N[2]);
                //interaction.AOV = bary;
                return true;
            }
        }
        return false;
    }

    Vector3f Triangle::get_barycentric(Vector3f &p) const
    {
        Vector3f v2_ = p- v0;
        float d00 = glm::dot(edge1,edge1);
        float d01 = glm::dot(edge1,edge2);
        float d11 = glm::dot(edge2,edge2);
        float d20 = glm::dot(v2_,edge1);
        float d21 = glm::dot(v2_,edge2);
        float d = d00*d11 - d01*d01;
        float v = (d11*d20-d01*d21)/d;
        float w = (d00 * d21 - d01 *d20)/d;
        float u = 1 - v - w;
        return Vector3f(u,v,w);
    }
    public:
        Vector3f v0,v1,v2,edge1,edge2,n,N[3];
        Vector2f uv[3];
        Color color;
};
