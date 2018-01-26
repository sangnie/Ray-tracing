#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <ostream>
#include "objects.h"
#include <math.h>
#include <vector>
#include <limits>
#include <typeinfo>
using namespace std;

float distance(Point_3d p1, Point_3d p2){
	return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y) + (p1.z - p2.z)*(p1.z - p2.z);
}

Point_3d closest_intersection(Line l, vector<Object*> objects){
	// bool intersect = false;
	float min = std::numeric_limits<float>::max();
	Point_3d closest = l.ro;

	for (std::vector<Object*>::iterator it = objects.begin() ; it != objects.end(); ++it){
		// cout << typeid(*it).name() << endl;
		try{
			// intersect = true;
			Point_3d p = (*it)->intersection(l);
			cout<< p <<endl;
			float d = distance(l.ro,p);
			if(d<min){
				min = d;
				closest = p;
				// cout<<"yay"<<endl;
			}
			// cout << distance(l.ro,p)<<endl;
		} catch(const char* msg){
			// cout << "No intersection" << endl;
		}
	}
	return closest;
}

Color diffusion(Point_3d light, Color intensity, Point_3d normal, float k)
{
	return intensity.multiply(k * light.dot(normal))
}

Color specular(Point_3d light, Color intensity, Point_3d normal, float k, int n, Point_3d view)
{
	Point_3d ref = light.reflected(normal);
	return intensity.multiply(k * pow(ref.dot(v),n));
}



int main(){
	Point_3d eye(0,0,0);
	Point_3d dirn(0,0,1.0);
	Point_3d pt1(2,0,1);
	Point_3d pt2(-2,0,1);
	Point_3d pt3(0,4,1);

	Triangle* t = new Triangle(pt1,pt2,pt3);
	Plane pl = t->p;
	Sphere* s = new Sphere(pt1,0.1);


	std::vector<Object*> objects;
	objects.push_back(t);
	objects.push_back(s);

	Line l(eye,dirn);
	// cout << s.intersection(l)<<endl;
	Point_3d p = closest_intersection(l,objects);
	cout << p.x << " " << p.y << " " << p.z << endl;
	return 0;
}