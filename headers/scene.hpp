#pragma once
#include <vector>
#include "shape.hpp"
#include "triangleMesh.hpp"
#include "../headers/binTree.hpp"
#include "../headers/bvh.hpp"
#include "../headers/kdTree.hpp"
double erand48(unsigned short xsubi[3])
{
    return (double)rand() / (double)RAND_MAX;
}

class Scene : public Shape
{
    public:
    Scene() {range = {FLT_MAX,FLT_MAX,FLT_MAX,FLT_MIN,FLT_MIN,FLT_MIN};biggest = {0,0,0};}
    Scene(std::shared_ptr <TriangleMesh> object)
    {
        range = {FLT_MAX,FLT_MAX,FLT_MAX,FLT_MIN,FLT_MIN,FLT_MIN};
        Add(object);
        const Range ran = objects.back()->getRange();
        if(ran.x_max>range.x_max){range.x_max = ran.x_max;}
        if(ran.y_max>range.y_max){range.y_max = ran.y_max;}
        if(ran.z_max>range.z_max){range.z_max = ran.z_max;}
        if(ran.x_min<range.x_min){range.x_min = ran.x_min;}
        if(ran.y_min<range.y_min){range.y_min = ran.y_min;}
        if(ran.z_min<range.z_min){range.z_min = ran.z_min;}
    }
    //virtual Range getRange() const override{return range;}
    void printBox()
    {
        std::cout <<"Scene Bounding Box: "<< std::endl;
        std::cout <<range.x_min << std::endl;
        std::cout <<range.y_min << std::endl;
        std::cout <<range.z_min << std::endl;
        std::cout <<range.x_max << std::endl;
        std::cout <<range.y_max << std::endl;
        std::cout <<range.z_max << std::endl;
    }
    void clear();
    void Add(std::shared_ptr<TriangleMesh> object);
    bool Scene::intersectGrid(const Ray &r, SurfaceInteraction &interaction,BinTree &bintre) const;
    bool Scene::intersectBVH(const Ray &r, SurfaceInteraction &interaction,BvhNode &bhv) const;
    bool Scene::intersectKD(const Ray &r, SurfaceInteraction &interaction,KdNode &kd) const;
    virtual bool intersect(const Ray& r, SurfaceInteraction &interaction) const;
    Vec Scene::trace_rayKD(const Ray& r, int depth, unsigned short* Xi,KdNode &kd);
    Vec Scene::trace_rayBVH(const Ray& r, int depth, unsigned short* Xi,BvhNode &bvh);
    Vec Scene::trace_rayGrid(const Ray& r, int depth, unsigned short* Xi,BinTree &bintre);
    Vec trace_ray(const Ray& r, int depth, unsigned short* Xi);
    public:
    Vector3f biggest;
    std::vector<std::shared_ptr<TriangleMesh>> objects;
    Range range;
};

void Scene::clear()
{
    objects.clear();
}

void Scene::Add(std::shared_ptr<TriangleMesh> object)
{
    objects.push_back(object);
     const Range ran = objects.back()->getRange();
        if(ran.x_max>range.x_max){range.x_max = ran.x_max;}
        if(ran.y_max>range.y_max){range.y_max = ran.y_max;}
        if(ran.z_max>range.z_max){range.z_max = ran.z_max;}
        if(ran.x_min<range.x_min){range.x_min = ran.x_min;}
        if(ran.y_min<range.y_min){range.y_min = ran.y_min;}
        if(ran.z_min<range.z_min){range.z_min = ran.z_min;}
        if(biggest.x<objects.back()->biggest.x)biggest.x = objects.back()->biggest.x;
        if(biggest.y<objects.back()->biggest.y)biggest.y = objects.back()->biggest.y;
        if(biggest.z<objects.back()->biggest.z)biggest.z = objects.back()->biggest.z;
}

//bool Scene::BVHIntersect(const Ray &r, SurfaceInteraction &interaction)
//bool Scene::KdTREEIntersect(const Ray &r, SurfaceInteraction &interaction) 
//bool Scene::BinTreeIntersect(const Ray &r, SurfaceInteraction &interaction)

bool Scene::intersectKD(const Ray &r, SurfaceInteraction &interaction,KdNode &kd) const
{
    std::pair<float, Triangle>p =  kd.findIntersection(r.o, r.d,interaction,r);
    if(p.first >0)
    return true;
    else
    return false;
}


bool Scene::intersectBVH(const Ray &r, SurfaceInteraction &interaction,BvhNode &bhv) const
{
    std::pair<float, Triangle>p =  bhv.findIntersection(r.o, r.d,interaction,r);
    if(p.first >0)
    return true;
    else
    return false;
}

 Vec Scene::trace_rayKD(const Ray& r, int depth, unsigned short* Xi,KdNode &kd)
 {
    SurfaceInteraction interaction;

    // if no hit, return world colour
    if (!intersectKD(r, interaction,kd))
    {
        return Vec();
    }

    // if hit something, calc radiance
    if (depth > 10) return Vec();


    Vec x = Vec(interaction.p);						// ray intersection point
    Vec n = Vec(interaction.Ng);					// sphere normal
    Vec n1 = Vec(interaction.Ng);           		// properly oriented surface normal
    Vec f = Vec(interaction.AOV);					// object color (BRDF modulator)

    Vec emissive = Vec(interaction.color.emissive); // w mtb spodziewalem sie emissive, ale zamiast tego bylo ambient
    Vec diffuse = Vec(interaction.color.diffuse);
    Vec specular = Vec(interaction.color.specular);
    Vec ambient = Vec(interaction.color.ambient);

    double t = double(interaction.t);

    float p = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z; // max refl
    if (++depth > 5 || !p)
        if (erand48(Xi) < p)
            f = Vec(f.x / p, f.y / p, f.z / p);
        else
            return emissive; // emission, tam bylo 0 dla wszystkich oprocz tej lampki, olac na razie
    
    if (diffuse != Vec())
    {
        double r1 = 2 * M_PI * erand48(Xi);				// angle around
        double r2 = erand48(Xi), r2s = sqrt(r2);		// distance from center
        Vec w = n1;										// w=normal
        Vec u = ((fabs(w.x) > .1 ? Vec(0.0f, 1.0f, 0.0f) : Vec(1.0f, 0.0f, 0.0f)) % w).norm(); // u is perpendicular to w
        Vec v = w % u;									// v is perpendicular to u and w
        Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm(); // d is random reflection ray

        return emissive + f.mult(trace_ray(Ray(x, d), depth, Xi));
    }
    // IDEAL SPECULAR REFLECTION
    else if (specular != Vec())
    {
        return emissive + f.mult(trace_ray(Ray(x, r.d - n * 2 * n.dot(r.d)), depth, Xi));
    }
    // OTHERWISE WE HAVE A DIELECTRIC(GLASS) SURFACE
    Ray reflRay(x, r.d - n * 2 * n.dot(Vec(r.d).cast_to_3f()));	// Ideal dielectric reflection
    bool into = n.dot(n1) > 0;					// Ray from outside going in check?
    double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = Vec(r.d).dot(n1), cos2t;
    // if total internal reflection, REFLECT
    if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0) // Total internal reflection
        return emissive + f.mult(trace_ray(reflRay, depth, Xi));


    // otherwise, choose REFLECTION or REFRACTION
    Vec tdir = (Vec(r.d) * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).norm();
    double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : tdir.dot(n));
    double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);
    return emissive + f.mult(depth > 2 ? (erand48(Xi) < P ?   // Russian roulette
        trace_ray(reflRay, depth, Xi) * RP : trace_ray(Ray(x, tdir), depth, Xi) * TP) :
        trace_ray(reflRay, depth, Xi) * Re + trace_ray(Ray(x, tdir), depth, Xi) * Tr);
}


Vec Scene::trace_rayBVH(const Ray& r, int depth, unsigned short* Xi,BvhNode &bvh)
{
    SurfaceInteraction interaction;

    // if no hit, return world colour
    if (!intersectBVH(r, interaction,bvh))
    {
        return Vec();
    }

    // if hit something, calc radiance
    if (depth > 10) return Vec();


    Vec x = Vec(interaction.p);						// ray intersection point
    Vec n = Vec(interaction.Ng);					// sphere normal
    Vec n1 = Vec(interaction.Ng);           		// properly oriented surface normal
    Vec f = Vec(interaction.AOV);					// object color (BRDF modulator)

    Vec emissive = Vec(interaction.color.emissive); // w mtb spodziewalem sie emissive, ale zamiast tego bylo ambient
    Vec diffuse = Vec(interaction.color.diffuse);
    Vec specular = Vec(interaction.color.specular);
    Vec ambient = Vec(interaction.color.ambient);

    double t = double(interaction.t);

    float p = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z; // max refl
    if (++depth > 5 || !p)
        if (erand48(Xi) < p)
            f = Vec(f.x / p, f.y / p, f.z / p);
        else
            return emissive; // emission, tam bylo 0 dla wszystkich oprocz tej lampki, olac na razie
    
    if (diffuse != Vec())
    {
        double r1 = 2 * M_PI * erand48(Xi);				// angle around
        double r2 = erand48(Xi), r2s = sqrt(r2);		// distance from center
        Vec w = n1;										// w=normal
        Vec u = ((fabs(w.x) > .1 ? Vec(0.0f, 1.0f, 0.0f) : Vec(1.0f, 0.0f, 0.0f)) % w).norm(); // u is perpendicular to w
        Vec v = w % u;									// v is perpendicular to u and w
        Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm(); // d is random reflection ray

        return emissive + f.mult(trace_ray(Ray(x, d), depth, Xi));
    }
    // IDEAL SPECULAR REFLECTION
    else if (specular != Vec())
    {
        return emissive + f.mult(trace_ray(Ray(x, r.d - n * 2 * n.dot(r.d)), depth, Xi));
    }
    // OTHERWISE WE HAVE A DIELECTRIC(GLASS) SURFACE
    Ray reflRay(x, r.d - n * 2 * n.dot(Vec(r.d).cast_to_3f()));	// Ideal dielectric reflection
    bool into = n.dot(n1) > 0;					// Ray from outside going in check?
    double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = Vec(r.d).dot(n1), cos2t;
    // if total internal reflection, REFLECT
    if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0) // Total internal reflection
        return emissive + f.mult(trace_ray(reflRay, depth, Xi));


    // otherwise, choose REFLECTION or REFRACTION
    Vec tdir = (Vec(r.d) * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).norm();
    double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : tdir.dot(n));
    double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);
    return emissive + f.mult(depth > 2 ? (erand48(Xi) < P ?   // Russian roulette
        trace_ray(reflRay, depth, Xi) * RP : trace_ray(Ray(x, tdir), depth, Xi) * TP) :
        trace_ray(reflRay, depth, Xi) * Re + trace_ray(Ray(x, tdir), depth, Xi) * Tr);
}

bool Scene::intersectGrid(const Ray &r, SurfaceInteraction &interaction,BinTree &bintre) const
{
    SurfaceInteraction temp;
    bool any_hit = false;
    float t_near = r.t_max;
    int constant = bintre.getDim();
    std::vector<int>inters = bintre.intersectionsWithBoxes(r.o,r.d);
    for (int i = 0 ;i < inters.size()-1;i++)
    {
        int current[3] = {inters[i]%(constant),((inters[i]-(inters[i]%constant))/constant)%constant,((inters[i]-inters[i])%(constant*constant))/constant};
        for(int j = 0; j<bintre.get(current[0],current[1],current[2]).size(); j++)
        if (bintre.get(current[0],current[1],current[2])[j].intersect(r, temp))
        {
            any_hit = true;
            t_near = temp.t;
            interaction = temp;
        }
    }

    return any_hit;
}


bool Scene::intersect(const Ray &r, SurfaceInteraction &interaction) const
{
    SurfaceInteraction temp;
    bool any_hit = false;
    float t_near = r.t_max;

    for (const auto& object : objects)
    {
        if (object->intersect(r, temp))
        {
            any_hit = true;
            t_near = temp.t;
            interaction = temp;
        }
    }

    return any_hit;
}
Vec Scene::trace_rayGrid(const Ray& r, int depth, unsigned short* Xi,BinTree & bintre)
{
    SurfaceInteraction interaction;

    // if no hit, return world colour
    if (!intersectGrid(r, interaction,bintre))
    {
        return Vec();
    }

    // if hit something, calc radiance
    if (depth > 10) return Vec();


    Vec x = Vec(interaction.p);						// ray intersection point
    Vec n = Vec(interaction.Ng);					// sphere normal
    Vec n1 = Vec(interaction.Ng);           		// properly oriented surface normal
    Vec f = Vec(interaction.AOV);					// object color (BRDF modulator)

    Vec emissive = Vec(interaction.color.emissive); // w mtb spodziewalem sie emissive, ale zamiast tego bylo ambient
    Vec diffuse = Vec(interaction.color.diffuse);
    Vec specular = Vec(interaction.color.specular);
    Vec ambient = Vec(interaction.color.ambient);

    double t = double(interaction.t);

    float p = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z; // max refl
    if (++depth > 5 || !p)
        if (erand48(Xi) < p)
            f = Vec(f.x / p, f.y / p, f.z / p);
        else
            return emissive; // emission, tam bylo 0 dla wszystkich oprocz tej lampki, olac na razie
    
    if (diffuse != Vec())
    {
        double r1 = 2 * M_PI * erand48(Xi);				// angle around
        double r2 = erand48(Xi), r2s = sqrt(r2);		// distance from center
        Vec w = n1;										// w=normal
        Vec u = ((fabs(w.x) > .1 ? Vec(0.0f, 1.0f, 0.0f) : Vec(1.0f, 0.0f, 0.0f)) % w).norm(); // u is perpendicular to w
        Vec v = w % u;									// v is perpendicular to u and w
        Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm(); // d is random reflection ray

        return emissive + f.mult(trace_ray(Ray(x, d), depth, Xi));
    }
    // IDEAL SPECULAR REFLECTION
    else if (specular != Vec())
    {
        return emissive + f.mult(trace_ray(Ray(x, r.d - n * 2 * n.dot(r.d)), depth, Xi));
    }
    // OTHERWISE WE HAVE A DIELECTRIC(GLASS) SURFACE
    Ray reflRay(x, r.d - n * 2 * n.dot(Vec(r.d).cast_to_3f()));	// Ideal dielectric reflection
    bool into = n.dot(n1) > 0;					// Ray from outside going in check?
    double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = Vec(r.d).dot(n1), cos2t;
    // if total internal reflection, REFLECT
    if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0) // Total internal reflection
        return emissive + f.mult(trace_ray(reflRay, depth, Xi));


    // otherwise, choose REFLECTION or REFRACTION
    Vec tdir = (Vec(r.d) * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).norm();
    double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : tdir.dot(n));
    double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);
    return emissive + f.mult(depth > 2 ? (erand48(Xi) < P ?   // Russian roulette
        trace_ray(reflRay, depth, Xi) * RP : trace_ray(Ray(x, tdir), depth, Xi) * TP) :
        trace_ray(reflRay, depth, Xi) * Re + trace_ray(Ray(x, tdir), depth, Xi) * Tr);
}
Vec Scene::trace_ray(const Ray& r, int depth, unsigned short* Xi)
{
    SurfaceInteraction interaction;

    // if no hit, return world colour
    if (!intersect(r, interaction))
    {
        return Vec();
    }

    // if hit something, calc radiance
    if (depth > 10) return Vec();


    Vec x = Vec(interaction.p);						// ray intersection point
    Vec n = Vec(interaction.Ng);					// sphere normal
    Vec n1 = Vec(interaction.Ng);           		// properly oriented surface normal
    Vec f = Vec(interaction.AOV);					// object color (BRDF modulator)

    Vec emissive = Vec(interaction.color.emissive); // w mtb spodziewalem sie emissive, ale zamiast tego bylo ambient
    Vec diffuse = Vec(interaction.color.diffuse);
    Vec specular = Vec(interaction.color.specular);
    Vec ambient = Vec(interaction.color.ambient);

    double t = double(interaction.t);

    float p = f.x > f.y && f.x > f.z ? f.x : f.y > f.z ? f.y : f.z; // max refl
    if (++depth > 5 || !p)
        if (erand48(Xi) < p)
            f = Vec(f.x / p, f.y / p, f.z / p);
        else
            return emissive; // emission, tam bylo 0 dla wszystkich oprocz tej lampki, olac na razie
    
    if (diffuse != Vec())
    {
        double r1 = 2 * M_PI * erand48(Xi);				// angle around
        double r2 = erand48(Xi), r2s = sqrt(r2);		// distance from center
        Vec w = n1;										// w=normal
        Vec u = ((fabs(w.x) > .1 ? Vec(0.0f, 1.0f, 0.0f) : Vec(1.0f, 0.0f, 0.0f)) % w).norm(); // u is perpendicular to w
        Vec v = w % u;									// v is perpendicular to u and w
        Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm(); // d is random reflection ray

        return emissive + f.mult(trace_ray(Ray(x, d), depth, Xi));
    }
    // IDEAL SPECULAR REFLECTION
    else if (specular != Vec())
    {
        return emissive + f.mult(trace_ray(Ray(x, r.d - n * 2 * n.dot(r.d)), depth, Xi));
    }
    // OTHERWISE WE HAVE A DIELECTRIC(GLASS) SURFACE
    Ray reflRay(x, r.d - n * 2 * n.dot(Vec(r.d).cast_to_3f()));	// Ideal dielectric reflection
    bool into = n.dot(n1) > 0;					// Ray from outside going in check?
    double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = Vec(r.d).dot(n1), cos2t;
    // if total internal reflection, REFLECT
    if ((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0) // Total internal reflection
        return emissive + f.mult(trace_ray(reflRay, depth, Xi));


    // otherwise, choose REFLECTION or REFRACTION
    Vec tdir = (Vec(r.d) * nnt - n * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t)))).norm();
    double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : tdir.dot(n));
    double Re = R0 + (1 - R0) * c * c * c * c * c, Tr = 1 - Re, P = .25 + .5 * Re, RP = Re / P, TP = Tr / (1 - P);
    return emissive + f.mult(depth > 2 ? (erand48(Xi) < P ?   // Russian roulette
        trace_ray(reflRay, depth, Xi) * RP : trace_ray(Ray(x, tdir), depth, Xi) * TP) :
        trace_ray(reflRay, depth, Xi) * Re + trace_ray(Ray(x, tdir), depth, Xi) * Tr);
}