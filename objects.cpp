#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "objects.h"
#include <math.h>
#include <limits>
#include <vector>
using namespace std;

std::ostream& operator<<(std::ostream &strm, const Point_3d &a) {
	return strm << "Point(" << a.x << "," << a.y << "," << a.z << ")";
}

std::ostream& operator<<(std::ostream &strm, const Line &a) {
	return strm << "Line(" << a.ro.x << "," << a.ro.y << "," << a.ro.z << "," << a.rd.x << "," << a.rd.y << "," << a.rd.z << ")";
}

std::ostream& operator<<(std::ostream &strm, const Color &a) {
	return strm << "Color(" << a.r << "," << a.g << "," << a.b << ")";
}

// std::ostream& operator<<(std::ostream &strm, const Sphere &a) {
// 	return strm << "Sphere(" << a.radius << ")";
// }

// std::ostream& operator<<(std::ostream &strm, const Plane &a) {
// 	return strm << "Plane("<< a.a << "," << a.b << "," << a.c << "," <<a.d << ")";
// }

float distance(Point_3d p1, Point_3d p2){
	return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y) + (p1.z - p2.z)*(p1.z - p2.z));
}

Point_3d Point_3d::add(Point_3d p){
	Point_3d p1;

	p1.x = this->x + p.x;
	p1.y = this->y + p.y;
	p1.z = this->z + p.z;

	return p1;
}

Point_3d Point_3d::subtract(Point_3d p){
	Point_3d p1;

	p1.x = this->x - p.x;
	p1.y = this->y - p.y;
	p1.z = this->z - p.z;

	return p1;
}

Point_3d Point_3d::multiply(float n){
    return(Point_3d(n*this->x, n*this->y, n*this->z));
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
	// cout<<"l:   " << l<<endl;
	// cout<<"rd:   " << l1.rd<<endl;
	float t_ca = l.dot(l1.rd);
	// cout <<t_ca<<endl;

	if(t_ca < 0){
		// cout<<"yay"<<endl;
		throw "Sphere: Behind eye";
	} else {
		float dsqr = l.dot(l) - t_ca*t_ca;
		float r = this->radius;
		// cout<<"yay2"<< dsqr<< " " << sqrt(dsqr)<<" "<< r<<endl;
		if(sqrt(dsqr) > r){
			// cout<<"yay3"<<endl;
			throw "Sphere: No intersection";
		} else {
			float t_hc = sqrt(r*r - dsqr);
			float t = t_ca - t_hc;

			if(t < 0.001 && t > -0.001){
				// throw "Sphere: intersecting at eye";
				t = t_ca + t_hc;
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

	if(vd < 0.001 && vd > -0.001){
		// cout << "yay1" << endl;
		throw "Plane: Parallel line, No intersection";
	} else {
		// cout << l.ro.x << " " << l.ro.y << " " << l.ro.z << endl;
		// cout << l.ro.x * a<< " " << l.ro.y *b << " " << l.ro.z *c << d<< endl;
		// cout<< a << " "<<b<<" "<< c<<" "<<d<<endl;
		float v0 = a*l.ro.x + b*l.ro.y + c*l.ro.z + d;
		float t = -1 * v0 / vd;
		// cout << t << " " << v0 << " " << vd << endl;
		if(t<-0.001){
			throw "Plane: Behind eye";
		} else {
			if((v0 < 0.001 && v0 > -0.001) || (t < 0.01 && t > -0.01)){
				throw "Plane: intersecting at eye";
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

Point_3d Circle::intersection(Line l){
	
	try{
		Point_3d p = this->p.intersection(l);
		Point_3d d = p.subtract(this->centre);
		if(d.dot(d) < this->radius){
			return p;
		} else {
			throw "Outside Circle";
		}
	} catch(const char* msg){
		throw "No intersection";
	}	
}

Point_3d Quadric::intersection(Line l){
	float A = this->a;
	float B = this->b;
	float C = this->c;
	float D = this->d;
	float E = this->e;
	float F = this->f;
	float G = this->g;
	float H = this->h;
	float I = this->i;
	float J = this->j;
	float xo = l.ro.x;
	float yo = l.ro.y;
	float zo = l.ro.z;
	float xd = l.rd.x;
	float yd = l.rd.y;
	float zd = l.rd.z;
	float Aq = A*xd*xd + B*yd*yd + C*zd*zd + D*xd*yd + E*xd*zd + F*yd*zd;
	float Bq = 2*A*xo*xd + 2*B*yo*yd + 2*C*zo*zd + D*(xo*yd + yo*xd) + E*(xo*zd + zo*xd) + F*(yo*zd + yd*zo) + G*xd + H*yd + I*zd;
	float Cq = A*xo*xo + B*yo*yo + C*zo*zo + D*xo*yo + E*xo*zo + F*yo*zo + G*xo + H*yo + I*zo + J ;
	// float Aq = a*l.rd.x*l.rd.x + b*l.rd.y*l.rd.y + c*l.rd.z*l.rd.z + d*l.rd.x*l.rd.y + e*l.rd.x*l.rd.z + f*l.rd.y*l.rd.z;

	float t;

	if(Aq<0.01 && A>-0.01){
		t = -1 * Cq / Bq;
	} else {
		float discriminant = Bq*Bq - 4*Aq*Cq;
		if(discriminant<0){
			throw "No intersection";
		} else {
			discriminant = sqrt(discriminant);
			t = (-Bq - discriminant) / (2*Aq);
			if(t < 0){
				t = (-Bq + discriminant) / (2*Aq);
			}
		}
	}

    return Point_3d(l.ro.x + t*l.rd.x, l.ro.y + t*l.rd.y, l.ro.z + t*l.rd.z);
}

Point_3d Mesh::intersection(Line l){

	float min = std::numeric_limits<float>::max();
	Point_3d closest = l.ro;
	Triangle *nearest;
	bool intersecting = false;

	for (std::vector<Triangle*>::iterator it = this->triangles.begin() ; it != this->triangles.end(); ++it){
		try{
			Point_3d p = (*it)->intersection(l);
			intersecting = true;
			float d = distance(l.ro,p);
			if(d<min){
				min = d;
				closest = p;	//Point of intersection
				nearest = (*it);	//Intersecting object
				// cout<<"yay"<<endl;
			}
			// cout << distance(l.ro,p)<<endl;
		} catch(const char* msg){
			// cout << msg << endl;
		}
	}

	if(intersecting){
		this->normal_ = (*nearest).normal(closest);
		this->tri = nearest;
		return closest;
	} else {
		throw "No intersection";
	}

	// throw "No intersection";
}

Point_3d Mesh::normal(Point_3d p){
	normal_.normalize();
	return this->normal_;
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
	Point_3d nor(this->p.a, this->p.b, this->p.c);
	nor.normalize();
	return nor;
}

Point_3d Rectangle::normal(Point_3d p){
	Point_3d p1(this->p.a, this->p.b, this->p.c);
	p1.normalize();
	return p1;
}

Point_3d Circle::normal(Point_3d p){
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

Point_3d Quadric::normal(Point_3d p){
	Point_3d nor;
	float A = this->a;
	float B = this->b;
	float C = this->c;
	float D = this->d;
	float E = this->e;
	float F = this->f;
	float G = this->g;
	float H = this->h;
	float I = this->i;
	float J = this->j;
	nor.x = 2*A*p.x + D*p.y + E*p.z + G;
	nor.y = 2*B*p.y + D*p.x + F*p.z + H;
	nor.z = 2*C*p.z + E*p.x + F*p.y + I;

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

// 	// Triangle t(Point_3d(2,0,4), Point_3d(-2,0,4), Point_3d(0,4,4));
// 	// Rectangle r(Point_3d(2,1,4), Point_3d(-2,1,4), Point_3d(2,3,4), Point_3d(-2,3,4));
// 	Color k(0,0,0);
// 	Sphere s(Point_3d(0,4,3), 5,k,k,k,k,k,2);
// 	// Plane p = t.p;

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