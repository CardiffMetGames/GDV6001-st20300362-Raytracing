#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"

class quad : public hittable 
{
public:
    quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat) : Q(Q), u(u), v(v), mat(mat)
    {
        set_bounding_box();
    }

    virtual void set_bounding_box();

    aabb bounding_box() const override;

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

private:
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox;
};

#endif