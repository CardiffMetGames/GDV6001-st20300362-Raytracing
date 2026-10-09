#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include <random>

class vec3
{
public:
	double e[3];

	vec3() : e{ 0,0,0 } {}
	vec3(double e0, double e1, double e2) : e{ e0, e1, e2} {}

	double x() const;
	double y() const;
	double z() const;

	// https://www.geeksforgeeks.org/cpp/operator-overloading-cpp/

	vec3 operator-() const;
	double operator[](int i) const;
	double& operator[](int i);

	vec3& operator+=(const vec3& v);

	vec3& operator*=(double t);
	vec3& operator/=(double t);

	double length() const;
	double length_squared() const;

	static vec3 random();

	static vec3 random(double min, double max);

private:
	static double random_double();

	static double random_double(double min, double max);
};

using point3 = vec3;

// Utility Functions - https://www.geeksforgeeks.org/cpp/inline-functions-cpp/

inline std::ostream& operator<<(std::ostream& out, const vec3& v)
{
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) 
{
	return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) 
{
	return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) 
{
	return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) 
{
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) 
{
	return t * v;
}

inline vec3 operator/(const vec3& v, double t) 
{
	return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v) 
{
	return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) 
{
	return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1], u.e[2] * v.e[0] - u.e[0] * v.e[2], u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(const vec3& v) 
{
	return v / v.length();
}

inline vec3 random_unit_vector() 
{
	while (true) 
	{
		auto p = vec3::random(-1, 1);
		auto lensq = p.length_squared();

		if (1e-160 < lensq && lensq <= 1)
		{
			return p / sqrt(lensq);
		}
	}
}

inline vec3 random_on_hemisphere(const vec3& normal) 
{
	vec3 on_unit_sphere = random_unit_vector();

	if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
	{
		return on_unit_sphere;
	}
	else
	{
		return -on_unit_sphere;
	}
}

#endif