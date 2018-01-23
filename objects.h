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
	Point_3d r0;
	Point_3d rd;
} 

class Sphere{
	public:
	Point_3d centre;
	float radius;

	Sphere(Point_3d p, float r){
		this->centre = p;
		this->radius = r;
	}
	Point_3d intersection(line l1);
	Vector normal(Point_3d p);
}

class Triangle{
	public:
	Point_3d pt1;
	Point_3d pt2;
	Point_3d pt3;

	Triangle(Point_3d p1, Point_3d p2, Point_3d p3){
		this->pt1 = p1;
		this->pt2 = p2;
		this->pt3 = p3;
	}

	Point_3d intersection(line l1);
	Vector normal(Point_3d p);
} 

// class Box{
// public:
	
// }

class Plane{
	public:
	float a;
	float b;
	float c;
	float d;

	Plane(float x, float y, float z, float w){
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Point_3d intersection(line l1);
	Vector normal(Point_3d p);
}

// class Circle{
// 	public:
// 	Point_3d centre;
// 	float radius;
// 	plane p;

// 	Circle(Point_3d p, float r, plane pl1){

// 	}

// 	Point_3d intersection(line l1);
// 	Vector normal(Point_3d p);
// }

#endif