#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <ostream>

#define CIRCLE 0
#define SPHERE 1
#define TRIANGLE 2
#define BOX 3
#define PLANE 4
#define RECTANGLE 5
#define LIGHT_POINT 6
#define LIGHT_DIREC 7
#define LIGHT_SPOT 8
#define QUADRIC 9


class Color
{
	public:
	float r;
	float g;
	float b;
	Color(){}
	Color(float red,float green,float blue)
	{
		r = red;
		g = green;
		b = blue;
	}

	Color multiply(float c)
	{
		Color s;
		s.r = this->r * c;
		s.g = this->g * c;
		s.b = this->b * c;
		return s;
	}

	Color multiply(Color c)
	{
		Color s;
		s.r = this->r * c.r;
		s.g = this->g * c.g;
		s.b = this->b * c.b;
		return s;
	}

	Color add(Color c)
	{
		Color s;
		s.r = this->r + c.r;
		s.g = this->g + c.g;
		s.b = this->b + c.b;
		return s;
	}

	friend std::ostream& operator<<(std::ostream &strm, const Color &a);
};

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

    float dot(Point_3d p);
    Point_3d cross(Point_3d p);
    Point_3d add(Point_3d p);
    Point_3d subtract(Point_3d p);
    Point_3d multiply(float n);
    void normalize();
    Point_3d reflected(Point_3d normal);

    friend std::ostream& operator<<(std::ostream &strm, const Point_3d &a);
}; 

class Line{
    public:
    Point_3d ro;
    Point_3d rd;

    Line(Point_3d ro, Point_3d rd){
    	this->ro = ro;
    	this->rd = rd;
    }

    friend std::ostream& operator<<(std::ostream &strm, const Line &a);
}; 

class Object{
	public:
	int type;
	// Color intensity;
	Color ks;
	Color kd;
    Color ka;
    Color kr;
    Color kt;
    float index;
	int n_spec;
	// Object(){}
	// virtual ~Object(){}
	virtual Point_3d intersection(Line l) =0;
	virtual Point_3d normal(Point_3d p) =0;
};

class Sphere : public Object{
    public:
    Point_3d centre;
    float radius;

    Sphere(Point_3d p, float r, Color ks, Color kd, Color ka, Color kr, Color kt, int n_spec, int index){
        this->centre = p;
        this->radius = r;
        this->type = SPHERE;
        this->ks = ks;
        this->kd = kd;
        this->ka = ka;
        this->kr = kr;
        this->kt = kt;
        this->n_spec = n_spec;
        this->index = index;
    }
    Point_3d intersection(Line l);
    Point_3d normal(Point_3d p);
};

// class Box{
// public:
    
// }

class Plane : public Object{
    public:
    float a;
    float b;
    float c;
    float d;

    Plane(){
    	this->type = PLANE;
    }

    Plane(float x, float y, float z, float w, Color ks, Color kd, Color ka, Color kr, Color kt, int n_spec){
        this->a = x;
        this->b = y;
        this->c = z;
        this->d = w;
        this->type = PLANE;
        this->ks = ks;
        this->kd = kd;
        this->ka = ka;
        this->kr = kr;
        this->kt = kt;
        this->n_spec = n_spec;
    }

    Point_3d intersection(Line l);
    Point_3d normal(Point_3d p);
};

class Triangle : public Object{
    public:
    Point_3d pt1;
    Point_3d pt2;
    Point_3d pt3;
    Plane p;

    Triangle(Point_3d p1, Point_3d p2, Point_3d p3){
        this->pt1 = p1;
        this->pt2 = p2;
        this->pt3 = p3;
        this->n_spec = n_spec;

        Point_3d u = p3.subtract(p1);
        Point_3d v = p2.subtract(p1);
        
       	float a = u.y*v.z - u.z*v.y;
       	float b = u.z*v.x - u.x*v.z;
       	float c = u.x*v.y - u.y*v.x;
       	float d = -1*(a*p1.x + b*p1.y + c*p1.z);

       	Plane p(a,b,c,d,ks,kd,ka,kr,kt,n_spec);
       	this->p = p;
       	this->type = TRIANGLE;
    }

    Triangle(Point_3d p1, Point_3d p2, Point_3d p3, Color ks, Color kd, Color ka, Color kr, Color kt, int n_spec){
        this->pt1 = p1;
        this->pt2 = p2;
        this->pt3 = p3;
        this->ks = ks;
        this->kd = kd;
        this->ka = ka;
        this->kr = kr;
        this->kt = kt;
        this->n_spec = n_spec;

        Point_3d u = p3.subtract(p1);
        Point_3d v = p2.subtract(p1);
        
       	float a = u.y*v.z - u.z*v.y;
       	float b = u.z*v.x - u.x*v.z;
       	float c = u.x*v.y - u.y*v.x;
       	float d = -1*(a*p1.x + b*p1.y + c*p1.z);

       	Plane p(a,b,c,d,ks,kd,ka,kr,kt,n_spec);
       	this->p = p;
       	this->type = TRIANGLE;
    }

    Point_3d intersection(Line l);
    Point_3d normal(Point_3d p);
}; 

class Rectangle : public Object{
    public:
    Point_3d pt1;
    Point_3d pt2;
    Point_3d pt3;
    Point_3d pt4;
    Plane p;

    Rectangle(Point_3d p1, Point_3d p2, Point_3d p3, Point_3d p4, Color ks, Color kd, Color ka, Color kr, Color kt, int n_spec){
        this->pt1 = p1;
        this->pt2 = p2;
        this->pt3 = p3;
        this->pt4 = p4;
        this->ks = ks;
        this->kd = kd;
        this->ka = ka;
        this->kr = kr;
        this->kt = kt;
        this->n_spec = n_spec;

        Point_3d u = p3.subtract(p1);
        Point_3d v = p2.subtract(p1);
        
       	float a = u.y*v.z - u.z*v.y;
       	float b = u.z*v.x - u.x*v.z;
       	float c = u.x*v.y - u.y*v.x;
       	float d = -1*(a*p1.x + b*p1.y + c*p1.z);

       	Plane p(a,b,c,d,ks,kd,ka,kr,kt,n_spec);
       	this->p = p;
       	this->type = RECTANGLE;
    }

    Point_3d intersection(Line l);
    Point_3d normal(Point_3d p);
}; 

class Light{
	public:
	Color intensity;
	int type;
	Point_3d location;
	Point_3d direction;
	float dot_min;
};

class Point_source : public Light{
	public:
	// Point_3d location;
	// Color intensity;

	Point_source(Point_3d p, Color i){
		this->location = p;
		this->intensity = i;
		this->type = LIGHT_POINT;
	}	
	// Point_3d intersection(Line l);
 	// Point_3d normal(Point_3d p);
};

class Direction_source : public Light{
	public:
	// Point_3d direction;
	// Color intensity;

	Direction_source(Point_3d p, Color i){
		this->direction = p;
		this->intensity = i;
		this->type = LIGHT_DIREC;
	}	
	// Point_3d intersection(Line l);
 	// Point_3d normal(Point_3d p);
};

class Spotlight : public Light{
    public:
    // Point_3d direction;
    // Color intensity;

    Spotlight(Point_3d loc, Point_3d dir, float angle, Color i){
        dir.normalize();
        this->direction = dir;
        this->intensity = i;
        this->location = loc;
        this->dot_min = angle;
        this->type = LIGHT_SPOT;
    }   
    // Point_3d intersection(Line l);
    // Point_3d normal(Point_3d p);
};

class Circle : public Object{
    public:
    Point_3d centre;
    float radius;
    Plane p;

    Circle(Point_3d p, float r, Plane pl, Color ks, Color kd, Color ka, Color kr, Color kt, int n_spec){
        this->centre = p;
        this->radius = r;
        this->p = pl;
        this->ks = ks;
        this->kd = kd;
        this->ka = ka;
        this->kr = kr;
        this->kt = kt;
        this->n_spec = n_spec;
        this->type = CIRCLE;
    }

    Point_3d intersection(Line l1);
    Point_3d normal(Point_3d p);
};

class Quadric : public Object{
    public:
    float a;
    float b;
    float c;
    float d;
    float e;
    float f;
    float g;
    float h;
    float i;
    float j;

    Quadric(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, Color ks, Color kd, Color ka, Color kr, Color kt, int n_spec){
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        this->e = e;
        this->f = f;
        this->g = g;
        this->h = h;
        this->i = i;
        this->j = j;
        this->ks = ks;
        this->kd = kd;
        this->ka = ka;
        this->kr = kr;
        this->kt = kt;
        this->n_spec = n_spec;
        this->type = QUADRIC;
    }

    Point_3d intersection(Line l1);
    Point_3d normal(Point_3d p);

};

#endif