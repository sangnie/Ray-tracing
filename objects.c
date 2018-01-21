#include <stdlib.h>
#include <stdio.h>

typedef struct {
	int x;
	int y;
} point_2d;

typedef struct {
	int x;
	int y;
	int z;
} point_3d;

typedef struct {
	point_2d centre;
	int radius;
} circle;

typedef struct {
	point_3d r0;
	point_3d rd;
};

typedef struct {
	point_3d centre;
	int radius;
} sphere;

typedef struct {
	point_3d pt1;
	point_3d pt2;
	point_3d pt3;
} triangle;

typedef struct {
	
} box;