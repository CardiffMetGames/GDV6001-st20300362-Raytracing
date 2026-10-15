#ifndef INTERVAL_H
#define INTERVAL_H

#include "common.h"

class interval 
{
public:
    double min, max;

    interval() : min(+infinity), max(-infinity) {} // Default interval is empty

    interval(double min, double max) : min(min), max(max) {}

    interval(const interval& a, const interval& b);


    double size() const;

    bool contains(double x) const;

    bool surrounds(double x) const;

    double clamp(double x) const;

    interval expand(double delta) const;

    static const interval empty, universe;
    

    friend interval operator+(const interval& ival, double displacement)
    {
        return interval(ival.min + displacement, ival.max + displacement);
    }

    friend interval operator+(double displacement, const interval& ival)
    {
        return ival + displacement;
    }
};


#endif