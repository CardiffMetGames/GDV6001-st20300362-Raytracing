#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"

class quad : public hittable 
{
public:
    quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) : Q(Q), u(u), v(v), mat(mat)
    {
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n, n);

        set_bounding_box();
    }

    virtual void set_bounding_box();

    aabb bounding_box() const override;

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

    virtual bool is_interior(double a, double b, hit_record& rec) const;

private:
    point3 Q;
    vec3 u, v;
    vec3 w;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
};

#endif