#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#define SPHERE 1
#define TRI 2
#define BOX 3
#define PLANE 4
#define CIRCLE 5


class Point_2d{
	public:
	float x;
	float y;
};

class Point_3d{
	public:
	float x;
	float y;
	float z;

	Point_3d(){}

	Point_3d(float x, float y, float z){
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Point_3d subtract(Point_3d p);
}; 

class Vector{
	public:
	float x;
	float y;
	float z;
}

class Line{
	public:
	point_3d r0;
	point_3d rd;
} 

class Sphere{
	public:
	point_3d centre;
	float radius;

	Sphere(point_3d p, float r){
		this->centre = p;
		this->radius = r;
	}
	point_3d intersection(line l);
	vector normal(point_3d p);
}

// class Triangle{
// 	public:
// 	point_3d pt1;
// 	point_3d pt2;
// 	point_3d pt3;

// 	point_3d intersection(line l);
// 	vector normal(point_3d p);
// } 

// // class Box{
// public:
	
// // }

// class Plane{
// 	public:
// 	float a;
// 	float b;
// 	float c;
// 	float d;

// 	point_3d intersection(line l);
// 	vector normal(point_3d p);
// }

// class Circle{
// 	public:
// 	point_3d centre;
// 	float radius;
// 	plane p;

// 	point_3d intersection(line l);
// 	vector normal(point_3d p);
// }

#endif