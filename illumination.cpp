#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "objects.h"
#include <math.h>

using namespace std;

class Color
{
	public:
	int r;
	int g;
	int b;
	Color(){}
	Color(int red,int green,int blue)
	{
		r = red;
		g = green;
		b = blue;
	}

	Color multiply(Color c)
	{
		Color s;
		s.r = this->r * c.r;
		s.g = this->g * c.g;
		s.b = this->b * c.b;
	}
}

class Point_source{
	public:
	Point_3d location;
	Color intensity;

	Point_source(Point_3d p, float i){
		this->location = p;
		this->intensity = i;
	}	
};

class Direction_source{
	public:
	Point_3d direction;
	Color intensity;

	Direction_source(Point_3d p, float i){
		this->direction = p;
		this->intensity = i;
	}	
};

Color diffusion(Point_3d light, Color intensity, Point_3d normal, float k)
{
	return intensity.multiply(k * light.dot(normal))
}

Color specular(Point_3d light, Color intensity, Point_3d normal, float k, int n, Point_3d view)
{
	Point_3d ref = light.reflected(normal);
	return intensity.multiply(k * pow(ref.dot(v),n));
}

