#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#define SPHERE 1
#define TRI 2
#define BOX 3
#define PLANE 4
#define CIRCLE 5


typedef struct {
	float x;
	float y;
} point_2d;

typedef struct {
	float x;
	float y;
	float z;
} point_3d;

typedef struct {
	float x;
	float y;
	float z;
} vector;

typedef struct {
	point_3d r0;
	point_3d rd;
} line;

typedef struct {
	point_3d centre;
	float radius;
} sphere;

typedef struct {
	int a = 3;
	point_3d pt1;
	point_3d pt2;
	point_3d pt3;
} triangle;

typedef struct {
	
} box;

typedef struct {
	float a;
	float b;
	float c;
	float d;
} plane;

typedef struct {
	point_3d centre;
	float radius;
	plane p;
} circle;

point_3d int_sph(line l, sphere s);
point_3d int_tri(line l, triangle tri);
point_3d int_box(line l, box b);
point_3d int_plane(line l, plane p);
point_3d int_circle(line l, circle c);

#endif