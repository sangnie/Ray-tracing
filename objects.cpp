#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "objects.h"

using namespace std;

// Point_3d int_sph(line l, sphere s){
// 	point_3d l1 = 
// }

// point_3d int_tri(line l, triangle tri);
// point_3d int_box(line l, box b);
// point_3d int_plane(line l, plane p);
// point_3d int_circle(line l, circle c);


Point_3d Point_3d::subtract(Point_3d p){
	Point_3d p1;

	p1.x = this->x - p.x;
	p1.y = this->y - p.y;
	p1.z = this->z - p.z;

	return p1;
}



int main(){
	Point_3d p1(2.0,3.0,4.0);
	Point_3d p2(1.0,1.0,1.0);

	Point_3d p4 = p2;
	Point_3d p3 = p1.subtract(p2);
	Point_3d p5 = p2.subtract(p4);

	cout << p3.x << endl;
	cout << p3.y << endl;
	cout << p3.z << endl;

	cout << p5.x << endl;
	cout << p5.y << endl;
	cout << p5.z << endl;


	return 0;
}