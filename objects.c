#include <stdlib.h>
#include <stdio.h>

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
	point_3d r0;
	point_3d rd;
} line;

typedef struct {
	point_3d centre;
	float radius;
} sphere;

typedef struct {
	point_3d pt1;
	point_3d pt2;
	point_3d pt3;
} triangle;

typedef struct {
	
} box;

typedef struct {
	
} plane;

typedef struct {
	point_3d centre;
	float radius;
	plane p;
} circle;