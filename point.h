#ifndef postion_H
#define postion_H

#include "parameters.h"


class point
{
	public:
		double x;
		double y;
		point(double _x, double _y);
		point() {}
		double distance(point other);
		double squaredDistance(point other);

};

point :: point(double _x, double _y)
{
	x = _x;
	y = _y;
}

double point :: distance(point other)
{
	return sqrt((x-other.x)*(x-other.x) + (y-other.y)*(y-other.y));
}

double point :: squaredDistance(point other)
{
	return (x-other.x)*(x-other.x) + (y-other.y)*(y-other.y);
}


#endif