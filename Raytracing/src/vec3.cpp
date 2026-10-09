#include "vec3.h"

double vec3::x() const
{
	return e[0];
}

double vec3::y() const
{
	return e[1];
}

double vec3::z() const
{
	return e[2];
}

vec3 vec3::operator-() const 
{ 
    return vec3(-e[0], -e[1], -e[2]); 
}

double vec3::operator[](int i) const 
{ 
    return e[i]; 
}

double& vec3::operator[](int i) 
{ 
    return e[i]; 
}

vec3& vec3::operator+=(const vec3& v) 
{
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];

    return *this;
}

vec3& vec3::operator*=(double t) 
{
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;

    return *this;
}

vec3& vec3::operator/=(double t)
{
    return *this *= 1 / t;
}

double vec3::length() const 
{
    return std::sqrt(length_squared());
}

double vec3::length_squared() const 
{
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
}

double vec3::random_double() {
    // Returns a random real in [0,1).
    return std::rand() / (RAND_MAX + 1.0);
}

double vec3::random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

vec3 vec3::random() 
{
    return vec3(random_double(), random_double(), random_double());
}

vec3 vec3::random(double min, double max) 
{
    return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

bool vec3::near_zero() const 
{
    // Return true if the vector is close to zero in all dimensions.
    auto s = 1e-8;
    return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
}

vec3 vec3::random_in_unit_disk()
{
    while (true)
    {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}