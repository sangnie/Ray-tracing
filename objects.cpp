#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "objects.h"
#include <math.h>
using namespace std;

Point_3d Point_3d::subtract(Point_3d p){
	Point_3d p1;

	p1.x = this->x - p.x;
	p1.y = this->y - p.y;
	p1.z = this->z - p.z;

	return p1;
}

float Point_3d::dot(Point_3d p){
	return this->x*p.x + this->y*p.y + this->z*p.z; 
}

Point_3d Sphere::intersection(Line l1){
	Point_3d l = this->centre.subtract(l1.ro);

	float t_ca = l.dot(l1.rd);
	cout <<t_ca<<endl;

	if(t_ca < 0){
		cout<<"yay"<<endl;
		throw "No intersection";
	} else {
		float dsqr = l.dot(l) - t_ca*t_ca;
		float r = this->radius;
		cout<<"yay2"<< dsqr<< " " << sqrt(dsqr)<<" "<< r<<endl;
		if(sqrt(dsqr) > r){
			cout<<"yay3"<<endl;
			throw "No intersection";
		} else {
			float t_hc = sqrt(r*r - dsqr);
			float t = t_ca - t_hc;

			return Point_3d(l1.ro.x + t*l1.rd.x, l1.ro.y + t*l1.rd.y, l1.ro.z + t*l1.rd.z);
		}
	}
}

int main(){
	Point_3d p1(2.0,3.0,4.0);
	Point_3d p2(0,0,1.0);
	Point_3d p0(0,0,0);
	Point_3d p4 = p2;
	Point_3d p3 = p1.subtract(p2);
	Point_3d p5 = p2.subtract(p4);

	// cout << p3.x << endl;
	// cout << p3.y << endl;
	// cout << p3.z << endl;

	// cout << p5.x << endl;
	// cout << p5.y << endl;
	// cout << p5.z << endl;

	try{
		Sphere s(p1,4.0);
		Line l(p0,p2);
		Point_3d p = s.intersection(l);
		cout << p.x << p.y << p.z << endl;
	} catch(const char* msg){
		cout << msg << endl;
	}

	return 0;
}