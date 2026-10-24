#include "triangle.h"

void triangle::set_bounding_box()
{
    const auto a = Q;
    const auto b = Q + u;
    const auto c = Q + v;

    point3 min_point(
        std::fmin(a.x(), std::fmin(b.x(), c.x())),
        std::fmin(a.y(), std::fmin(b.y(), c.y())),
        std::fmin(a.z(), std::fmin(b.z(), c.z())));
    point3 max_point(
        std::fmax(a.x(), std::fmax(b.x(), c.x())),
        std::fmax(a.y(), std::fmax(b.y(), c.y())),
        std::fmax(a.z(), std::fmax(b.z(), c.z())));

    bbox = aabb(min_point, max_point);
}

aabb triangle::bounding_box() const
{
    return bbox;
}

bool triangle::hit(const ray& r, interval ray_t, hit_record& rec) const
{
    auto denom = dot(normal, r.direction());

    if (std::fabs(denom) < 1e-8)
    {
        return false;
    }

    auto t = (D - dot(normal, r.origin())) / denom;
    if (!ray_t.contains(t))
    {
        return false;
    }

    auto intersection = r.at(t);
    vec3 planar_hitpt_vector = intersection - Q;
    auto alpha = dot(w, cross(planar_hitpt_vector, v));
    auto beta = dot(w, cross(u, planar_hitpt_vector));

    if (!is_interior(alpha, beta, rec))
    {
        return false;
    }

    rec.t = t;
    rec.p = intersection;
    rec.mat = mat;
    rec.set_face_normal(r, normal);

    return true;
}

bool triangle::is_interior(double a, double b, hit_record& rec) const
{
    if (a < 0 || b < 0 || a + b > 1)
    {
        return false;
    }

    rec.u = a;
    rec.v = b;
    return true;
}