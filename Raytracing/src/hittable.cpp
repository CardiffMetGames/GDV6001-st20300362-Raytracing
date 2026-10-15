#include "hittable.h"

void hit_record::set_face_normal(const ray& r, const vec3& outward_normal)
{
	front_face = dot(r.direction(), outward_normal) < 0;
	normal = front_face ? outward_normal : -outward_normal;
}

bool translate::hit(const ray& r, interval ray_t, hit_record& rec) const
{
    // Move the ray backwards by the offset
    ray offset_r(r.origin() - offset, r.direction(), r.time());

    // Determine whether an intersection exists along the offset ray (and if so, where)
    if (!object->hit(offset_r, ray_t, rec))
        return false;

    // Move the intersection point forwards by the offset
    rec.p += offset;

    return true;
}

aabb translate::bounding_box() const 
{ 
    return bbox; 
}


bool rotate_y::hit(const ray& r, interval ray_t, hit_record& rec) const
{

    // Transform the ray from world space to object space.

    auto origin = point3(
        (cos_theta * r.origin().x()) - (sin_theta * r.origin().z()),
        r.origin().y(),
        (sin_theta * r.origin().x()) + (cos_theta * r.origin().z())
    );

    auto direction = vec3(
        (cos_theta * r.direction().x()) - (sin_theta * r.direction().z()),
        r.direction().y(),
        (sin_theta * r.direction().x()) + (cos_theta * r.direction().z())
    );

    ray rotated_r(origin, direction, r.time());

    // Determine whether an intersection exists in object space (and if so, where).

    if (!object->hit(rotated_r, ray_t, rec))
        return false;

    // Transform the intersection from object space back to world space.

    rec.p = point3(
        (cos_theta * rec.p.x()) + (sin_theta * rec.p.z()),
        rec.p.y(),
        (-sin_theta * rec.p.x()) + (cos_theta * rec.p.z())
    );

    rec.normal = vec3(
        (cos_theta * rec.normal.x()) + (sin_theta * rec.normal.z()),
        rec.normal.y(),
        (-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.z())
    );

    return true;
}

aabb rotate_y::bounding_box() const 
{ 
    return bbox; 
}
