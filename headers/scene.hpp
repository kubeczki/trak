#pragma once
#include <vector>
#include "shape.hpp"
#include "triangleMesh.hpp"

double erand48(unsigned short xsubi[3])
{
    return (double)rand() / (double)RAND_MAX;
}

class Scene : public Shape
{
    public:
    Scene() {}
    Scene(std::shared_ptr <TriangleMesh> object){Add(object);}

    void clear();
    void Add(std::shared_ptr<TriangleMesh> object);

    virtual bool intersect(const Ray& r, SurfaceInteraction &interaction) const;
    Vec trace_ray(const Ray& r, int depth, unsigned short* Xi);
    public:
    std::vector<std::shared_ptr<TriangleMesh>> objects;
};

void Scene::clear()
{
    objects.clear();
}

void Scene::Add(std::shared_ptr<TriangleMesh> object)
{
    objects.push_back(object);
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