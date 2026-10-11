#ifndef AABB_H
#define AABB_H

#include "interval.h"

// Axis-aligned bounding box

class aabb 
{
public:
    interval x, y, z;

    aabb() {} // The default AABB is empty, since intervals are empty by default.

    aabb(const interval& x, const interval& y, const interval& z) : x(x), y(y), z(z) {}

    aabb(const point3& a, const point3& b);

    aabb(const aabb& box0, const aabb& box1);

    const interval& axis_interval(int n) const;

    bool hit(const ray& r, interval ray_t) const;
};

#endif