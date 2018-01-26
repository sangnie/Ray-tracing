#ifndef OBJECTS_H_INCLUDED
#define OBJECTS_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <ostream>

#define SPHERE 1
#define TRIANGLE 2
#define BOX 3
#define PLANE 4
#define RECTANGLE 5
#define LIGHT_POINT 6



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
    Point_3d subtract(Point_3d p);
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
}; 

class Object{
	public:
	int type;
	// Object(){}
	// virtual ~Object(){}
	virtual Point_3d intersection(Line l) =0;
	virtual Point_3d normal(Point_3d p) =0;
};

class Sphere : public Object{
    public:
    Point_3d centre;
    float radius;

    Sphere(Point_3d p, float r){
        this->centre = p;
        this->radius = r;
        this->type = SPHERE;
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

    Plane(float x, float y, float z, float w){
        this->a = x;
        this->b = y;
        this->c = z;
        this->d = w;
        this->type = PLANE;
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

        Point_3d u = p3.subtract(p1);
        Point_3d v = p2.subtract(p1);
        
       	float a = u.y*v.z - u.z*v.y;
       	float b = u.z*v.x - u.x*v.z;
       	float c = u.x*v.y - u.y*v.x;
       	float d = -1*(a*p1.x + b*p1.y + c*p1.z);

       	Plane p(a,b,c,d);
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

    Rectangle(Point_3d p1, Point_3d p2, Point_3d p3, Point_3d p4){
        this->pt1 = p1;
        this->pt2 = p2;
        this->pt3 = p3;
        this->pt4 = p4;

        Point_3d u = p3.subtract(p1);
        Point_3d v = p2.subtract(p1);
        
       	float a = u.y*v.z - u.z*v.y;
       	float b = u.z*v.x - u.x*v.z;
       	float c = u.x*v.y - u.y*v.x;
       	float d = -1*(a*p1.x + b*p1.y + c*p1.z);

       	Plane p(a,b,c,d);
       	this->p = p;
       	this->type = RECTANGLE;
    }

    Point_3d intersection(Line l);
    Point_3d normal(Point_3d p);
}; 

class Color
{
	public:
	int r;
	int g;
	int b;
	Color(){}
	Color(int red,int green,int blue)
	{
		r = red;
		g = green;
		b = blue;
	}

	Color multiply(Color c)
	{
		Color s;
		s.r = this->r * c.r;
		s.g = this->g * c.g;
		s.b = this->b * c.b;
	}
};

class Point_source : public Object{
	public:
	Point_3d location;
	Color intensity;

	Point_source(Point_3d p, float i){
		this->location = p;
		this->intensity = i;
	}	
	Point_3d intersection(Line l);
    Point_3d normal(Point_3d p);
};

class Direction_source : public Object{
	public:
	Point_3d direction;
	Color intensity;

	Direction_source(Point_3d p, float i){
		this->direction = p;
		this->intensity = i;
	}	
	Point_3d intersection(Line l);
    Point_3d normal(Point_3d p);
};


// class Circle{
//     public:
//     Point_3d centre;
//     float radius;
//     plane p;

//     Circle(Point_3d p, float r, plane pl1){

//     }

//     Point_3d intersection(Line l1);
//     Point_3d normal(Point_3d p);
// }

#endif