#ifndef TRIANGLEMESH_HPP
#define TRIANGLEMESH_HPP
#define TINYOBJLOADER_IMPLEMENTATION
#include "triangle.hpp"
#include "../external/tinyobjloader-release/tiny_obj_loader.h"
#include <iostream>
#include <vector>


class TriangleMesh : public Shape {
public:
    TriangleMesh(const char *filepath);
    bool intersect(const Ray& r, SurfaceInteraction &interaction) const override;
    Range getRange() const{return range;} //override;
public:
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<Triangle> tris;
    Range range;
    Vector3f biggest;
};
TriangleMesh::TriangleMesh(const char *filepath) {
    biggest = {0,0,0};
    range = {FLT_MAX,FLT_MAX,FLT_MAX,FLT_MIN,FLT_MIN,FLT_MIN};
    std::string inputfile = filepath;
    unsigned long pos = inputfile.find_last_of("/");
    std::string mtlbasepath = inputfile.substr(0, pos + 1);  

    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warnings;
    std::string errors;

    bool ret = tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings, &errors, inputfile.c_str(), mtlbasepath.c_str());
    if (!warnings.empty()) {
    std::cout << "Warning: " << warnings << std::endl;
    }

    if (!errors.empty()) {
    std::cerr << "Error: " << errors << std::endl;
    }

    if (!ret) {
    exit(1);
    }

    std::vector<Vertex> vertices;
    std::vector<Colour> colors;
    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(triangles)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];
            int idm = shapes[s].mesh.material_ids[f];
            tinyobj::real_t Ka[3] = {materials[idm].ambient[0],materials[idm].ambient[1],materials[idm].ambient[2],};
            tinyobj::real_t Kd[3] = {materials[idm].diffuse[0],materials[idm].diffuse[1],materials[idm].diffuse[2]};
            tinyobj::real_t Ks[3] = {materials[idm].specular[0],materials[idm].specular[1],materials[idm].specular[2]};
            tinyobj::real_t Ke[3] = {materials[idm].emission[0],materials[idm].emission[1],materials[idm].emission[2]};
            Colour color;
            color.ambient = Vector3f(Ka[0],Ka[1],Ka[2]);
            color.diffuse = Vector3f(Kd[0],Kd[1],Kd[2]);
            color.specular = Vector3f(Ks[0],Ks[1],Ks[2]);
            color.emissive = Vector3f(Ke[0],Ke[1],Ke[2]);
            colors.push_back(color);
            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                Vertex vert;
                



                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attributes.vertices[3*idx.vertex_index+0]; 
                tinyobj::real_t vy = attributes.vertices[3*idx.vertex_index+1];
                tinyobj::real_t vz = attributes.vertices[3*idx.vertex_index+2];
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = attributes.normals[3 * idx.normal_index + 0];
                    tinyobj::real_t ny = attributes.normals[3 * idx.normal_index + 1];
                    tinyobj::real_t nz = attributes.normals[3 * idx.normal_index + 2];
                    vert.Normal = Vector3f(nx, ny, nz);
                }
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = attributes.texcoords[2 * idx.texcoord_index + 0];
                    tinyobj::real_t ty = attributes.texcoords[2 * idx.texcoord_index + 1];
                    vert.UV = Vector2f(tx, ty);
                }
                if(vx < range.x_min)
                {
                    range.x_min = vx;
                }    else if(vx>range.x_max)
                {
                    range.x_max = vx;
                }
                if(vy < range.y_min)
                {
                    range.y_min = vy;
                }    else if(vy>range.y_max)
                {
                    range.y_max = vy;
                }
                if(vz < range.z_min)
                {
                    range.z_min = vz;
                }    else if(vz>range.z_max)
                {
                    range.z_max = vz;
                }
                vert.Pos = Vector3f(vx, vy, vz);
                vertices.push_back(vert);
                
            }
            index_offset += fv;


        }
    }

    // Loops vertices
    for (int i = 0; i < vertices.size() / 3; ++i) {
        tris.push_back(Triangle(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2],colors[i]));
        Vector3f check;
        check.x = abs(vertices[i*3].Pos.x-vertices[i*3+1].Pos.x)>abs(vertices[i * 3].Pos.x-vertices[i * 3 + 2].Pos.x) ? abs(vertices[i*3].Pos.x-vertices[i*3+1].Pos.x):abs(vertices[i * 3].Pos.x-vertices[i * 3 + 2].Pos.x);
        check.y = abs(vertices[i*3].Pos.y-vertices[i*3+1].Pos.y)>abs(vertices[i * 3].Pos.y-vertices[i * 3 + 2].Pos.y) ? abs(vertices[i*3].Pos.y-vertices[i*3+1].Pos.y):abs(vertices[i * 3].Pos.y-vertices[i * 3 + 2].Pos.y);
        check.z = abs(vertices[i*3].Pos.z-vertices[i*3+1].Pos.z)>abs(vertices[i * 3].Pos.z-vertices[i * 3 + 2].Pos.z) ? abs(vertices[i*3].Pos.z-vertices[i*3+1].Pos.z):abs(vertices[i * 3].Pos.z-vertices[i * 3 + 2].Pos.z);
        if(biggest.x<check.x) biggest.x = check.x;
        if(biggest.y<check.y) biggest.y = check.y;
        if(biggest.z<check.z) biggest.z = check.z;
    }

    std::cout<<"> Successfully opened "<<inputfile<<"! \n\n";
    
    shapes.clear();
    materials.clear();
}

bool TriangleMesh::intersect(const Ray &r, SurfaceInteraction &interaction) const {
    
    bool hit_tri = false;
    long tri_idx;
    float t = r.t_max;
    SurfaceInteraction temp;
    long triangles_size = tris.size();
    for (long i = 0; i < triangles_size; ++i) {
        if (tris[i].intersect(r, temp)) {
            hit_tri = true;
            tri_idx = i;
            r.t_max = temp.t;
            interaction = temp;
        }
    }
    return hit_tri;
}

#endif