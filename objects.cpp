#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "objects.h"
#include <math.h>
using namespace std;

std::ostream& operator<<(std::ostream &strm, const Point_3d &a) {
	return strm << "Point(" << a.x << "," << a.y << "," << a.z << ")";
}

// std::ostream& operator<<(std::ostream &strm, const Sphere &a) {
// 	return strm << "Sphere(" << a.radius << ")";
// }

// std::ostream& operator<<(std::ostream &strm, const Plane &a) {
// 	return strm << "Plane("<< a.a << "," << a.b << "," << a.c << "," <<a.d << ")";
// }

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

Point_3d Point_3d::cross(Point_3d p)
{
	Point_3d cr;
	cr.x = (this->y)*(p.z) - (this->z)*(p.y);
	cr.y = (this->z)*(p.x) - (this->x)*(p.z);
	cr.z = (this->x)*(p.y) - (this->y)*(p.x);
	return cr;
}

Point_3d Sphere::intersection(Line l1){
	Point_3d l = this->centre.subtract(l1.ro);

	float t_ca = l.dot(l1.rd);
	// cout <<t_ca<<endl;

	if(t_ca < 0){
		// cout<<"yay"<<endl;
		throw "Behind eye";
	} else {
		float dsqr = l.dot(l) - t_ca*t_ca;
		float r = this->radius;
		// cout<<"yay2"<< dsqr<< " " << sqrt(dsqr)<<" "<< r<<endl;
		if(sqrt(dsqr) > r){
			// cout<<"yay3"<<endl;
			throw "No intersection";
		} else {
			float t_hc = sqrt(r*r - dsqr);
			float t = t_ca - t_hc;

			if(t < 0.000001 && t > -0.0000001){
				throw "intersecting at eye";
			}
			return Point_3d(l1.ro.x + t*l1.rd.x, l1.ro.y + t*l1.rd.y, l1.ro.z + t*l1.rd.z);
		}
	}
}

Point_3d Plane::intersection(Line l){
	float a = this->a;
	float b = this->b;
	float c = this->c;
	float d = this->d;

	float vd = a*l.rd.x + b*l.rd.y + c*l.rd.z;

	if(vd < 0.000001 && vd > -0.0000001){
		// cout << "yay1" << endl;
		throw "Parallel line, No intersection";
	} else {
		// cout << l.ro.x << " " << l.ro.y << " " << l.ro.z << endl;
		float v0 = a*l.ro.x + b*l.ro.y + c*l.ro.z + d;
		float t = -1 * v0 / vd;
		// cout << t << " " << v0 << " " << vd << endl;
		if(t<-0.001){
			throw "Behind eye";
		} else {
			if(v0 < 0.000001 && v0 > -0.0000001){
				throw "intersecting at eye";
			} else {
				return Point_3d(l.ro.x + t*l.rd.x, l.ro.y + t*l.rd.y, l.ro.z + t*l.rd.z);
			}
		}
	}
}

Point_3d Triangle::intersection(Line l){
	Point_3d p = this->p.intersection(l);
	Point_3d a = this->pt1;
	Point_3d b = this->pt2;
	Point_3d c = this->pt3;

	Point_3d v0 = c.subtract(a);
	Point_3d v1 = b.subtract(a);
	Point_3d v2 = p.subtract(a);

	// Compute dot products
	float dot00 = v0.dot(v0);
	float dot01 = v0.dot(v1);
	float dot02 = v0.dot(v2);
	float dot11 = v1.dot(v1);
	float dot12 = v1.dot(v2);

	// Compute barycentric coordinates
	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	if((u >= 0) && (v >= 0) && (u + v < 1)){
		return Point_3d(a.x + u*v0.x + v*v1.x, a.y + u*v0.y + v*v1.y, a.z + u*v0.z + v*v1.z);
	} else {
		throw "No intersection";
	}
}

Point_3d Rectangle::intersection(Line l){
	Point_3d a = this->pt1;
	Point_3d b = this->pt2;
	Point_3d c = this->pt3;
	Point_3d d = this->pt4;

	try{
		Triangle t(a,b,c);
		return t.intersection(l);
	} catch(const char* msg){
		try {
			Triangle t2(a,c,d);
			return t2.intersection(l);
		} catch(const char* msg){
			throw "No intersection";
		}	
	}	
}

void Point_3d::normalize()
{
	float mag = pow((this->x),2) + pow((this->y),2) + pow((this->z),2);
	mag = sqrt(mag);
	this->x /= mag;
	this->y /= mag;
	this->z /= mag;
}

Point_3d Sphere::normal(Point_3d p)
{
	Point_3d nor = p.subtract(this->centre);
	nor.normalize();
	return nor;
}

Point_3d Triangle::normal(Point_3d p)
{
	// Point_3d side1 = (this->pt1).subtract(this->pt3);
	// Point_3d side2 = (this->pt2).subtract(this->pt3);
	// Point_3d nor = side1.cross(side2);
	Point_3d nor(this->p.a, this->p.b, this->p.c);
	nor.normalize();
	return nor;
}

Point_3d Rectangle::normal(Point_3d p){
	Point_3d p1(this->p.a, this->p.b, this->p.c);
	p1.normalize();
	return p1;
}

Point_3d Plane::normal(Point_3d p)
{
	Point_3d nor;
	nor.x = this->a;
	nor.y = this->b;
	nor.z = this->c;
	nor.normalize();
	return nor;
}

Point_3d Point_3d::reflected(Point_3d normal)
{
	//Incident ray L calls function (as in slides)
	Point_3d ref = normal;
	float mult = 2 * dot(normal);
	ref.x *= mult;
	ref.y *= mult;
	ref.z *= mult;
	ref = ref.subtract(*this);
	return ref;
}

// int main(int argc, char const *argv[])
// {
// 	Point_3d eye(0,0,0);
// 	Point_3d dir(0,0,1);
// 	Line l(eye,dir);

// 	Triangle t(Point_3d(2,0,4), Point_3d(-2,0,4), Point_3d(0,4,4));
// 	Rectangle r(Point_3d(2,1,4), Point_3d(-2,1,4), Point_3d(2,3,4), Point_3d(-2,3,4));
// 	Sphere s(Point_3d(0,0,4), 4);
// 	Plane p = t.p;

// 	try{
// 		cout << s.intersection(l) << endl;
// 	} catch(const char* msg){
// 		cout << msg << endl;
// 	}
// 	return 0;
// }

// int main(){
// 	Point_3d p1(2.0,3.0,4.0);
// 	Point_3d p2(0,0,1.0);
// 	Point_3d p0(0,0,-1);
// 	Point_3d p10(10,0,-1);
// 	Point_3d p4 = p2;
// 	Point_3d p3 = p1.subtract(p2);
// 	Point_3d p5 = p2.subtract(p4);

// 	// cout << p3.x << endl;
// 	// cout << p3.y << endl;
// 	// cout << p3.z << endl;

// 	// cout << p5.x << endl;
// 	// cout << p5.y << endl;
// 	// cout << p5.z << endl;

// 	// try{
// 	// 	Sphere s(p2,0.4);
// 	// 	Line l(p0,p2);
// 	// 	Point_3d p = s.intersection(l);
// 	// 	cout << p.x << p.y << p.z << endl;
// 	// } catch(const char* msg){
// 	// 	cout << msg << endl;
// 	// }

// 	Point_3d pt1(2,0,0);
// 	Point_3d pt2(-2,0,0);
// 	Point_3d pt3(0,4,0);

// 	Triangle t(pt1,pt2,pt3);
// 	Plane pl = t.p;
// 	// cout << pl.a << endl;
// 	// cout << pl.b << endl;
// 	// cout << pl.c << endl;
// 	// cout << pl.d << endl;
// 	try{
// 		Line l(p10,p2);
// 		// cout << l.ro.x << " " << l.ro.y << " " << l.ro.z << endl;
// 		Point_3d p = pl.intersection(l);
// 		cout << p.x << " " << p.y << " " << p.z << endl;

// 		p = t.intersection(l);
// 		cout << p.x << " " << p.y << " " << p.z << endl;
// 	} catch(const char* msg){
// 		cout << msg << endl;
// 	}		

// 	return 0;
// }