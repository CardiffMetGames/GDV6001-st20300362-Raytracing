#include "quad.h"

void quad::set_bounding_box() 
{
    // Compute the bounding box of all four vertices.
    auto bbox_diagonal1 = aabb(Q, Q + u + v);
    auto bbox_diagonal2 = aabb(Q + u, Q + v);
    bbox = aabb(bbox_diagonal1, bbox_diagonal2);
}

aabb quad::bounding_box() const
{ 
    return bbox; 
}

bool quad::hit(const ray& r, interval ray_t, hit_record& rec) const
{
    return false; // To be implemented
}