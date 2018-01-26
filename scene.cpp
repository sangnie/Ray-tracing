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

Color illumination(Line l, vector<Object*> objects, vector<Light*> sources, int depth){

	float min = std::numeric_limits<float>::max();
	Point_3d closest = l.ro;
	Object *nearest;

	for (std::vector<Object*>::iterator it = objects.begin() ; it != objects.end(); ++it){
		// cout << typeid(*it).name() << endl;
		try{
			// intersect = true;
			Point_3d p = (*it)->intersection(l);
			cout<< p <<endl;
			float d = distance(l.ro,p);
			if(d<min){
				min = d;
				closest = p;	//Point of intersection
				nearest = (*it);	//Intersecting object
				// cout<<"yay"<<endl;
			}
			// cout << distance(l.ro,p)<<endl;
		} catch(const char* msg){
			// cout << "No intersection" << endl;
		}
	}
	// if((*nearest).type == LIGHT_POINT){
	// 	return (*nearest).intensity;
	// }

	Color diff(0,0,0);
	Color spec(0,0,0);

	for (std::vector<Light*>::iterator it = sources.begin() ; it != sources.end(); ++it)
	{
		// cout << typeid(*it).name() << endl;
		if((*it)->type = LIGHT_POINT)
		{
			Line light_ray(closest,(*it)->location.subtract(closest));
			
			float min_int = std::numeric_limits<float>::max();
			Point_3d closest_int = light_ray.ro;
			Object *nearest_int;

			for (std::vector<Object*>::iterator it = objects.begin() ; it != objects.end(); ++it)
			{
				// cout << typeid(*it).name() << endl;
				try{
					// intersect = true;
					Point_3d p = (*it)->intersection(light_ray);
					cout<< p <<endl;
					float d = distance(light_ray.ro,p);
					if(d<min_int){
						min_int = d;
						closest_int = p;	//Point of intersection
						nearest_int = (*it);	//Intersecting object
						// cout<<"yay"<<endl;
					}
					// cout << distance(l.ro,p)<<endl;
				} catch(const char* msg){
					// cout << "No intersection" << endl;
				}
			}
			float source_dist = distance(closest,(*it)->location);
			if(min_int < source_dist)
				continue;	// Light blocked

		}

		
	}



	Color c(0,0,0);
	return c;

}

Color diffusion(Point_3d light, Color intensity, Point_3d normal, Color k)
{
	return intensity.multiply(k.multiply(light.dot(normal)));
}

Color specular(Point_3d light, Color intensity, Point_3d normal, Color k, int n, Point_3d view)
{
	Point_3d ref = light.reflected(normal);
	return intensity.multiply(k.multiply(pow(ref.dot(view),n)));
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