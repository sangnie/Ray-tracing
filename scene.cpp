#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <ostream>
#include "objects.h"
#include <math.h>
#include <vector>
#include <limits>
#include <typeinfo>
#include <GL/glut.h>
using namespace std;

Color Ambient_Intensity(1,1,1);
Color k_reflection(0.5,0.5,0.5);
Color k_transmission(0.5,0.5,0.5);
int MAX_DEPTH = 0;
char *image_glob;
const int width = 50;
const int height = 50;

float distance(Point_3d p1, Point_3d p2){
	return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y) + (p1.z - p2.z)*(p1.z - p2.z);
}

Color diffusion(Point_3d light, Color intensity, Point_3d normal, Color kd)
{
	return intensity.multiply(kd.multiply(light.dot(normal)));
}

Color specular(Point_3d light, Color intensity, Point_3d normal, Color ks, int n_spec, Point_3d view)
{
	Point_3d ref = light.reflected(normal);
	return intensity.multiply(ks.multiply(pow(ref.dot(view),n_spec)));
}

Color illumination(Line l, vector<Object*> objects, vector<Light*> sources, int depth, int i, int j){

	Color final(0,0,0);

	float min = std::numeric_limits<float>::max();
	Point_3d closest = l.ro;
	Object *nearest;

	for (std::vector<Object*>::iterator it = objects.begin() ; it != objects.end(); ++it){
		// cout << typeid(*it).name() << endl;
		try{
			// intersect = true;
			// if(i==-15 && j==13){
			// 	cout<<"hchcjcjv"<<endl;
			// 	cout<<(*it)->type<<endl;
			// 	cout<<(*it)->n_spec<<endl;
			// 	cout<<l<<endl;
			// }
			Point_3d p = (*it)->intersection(l);
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
	// if((*nearest).type == LIGHT_POINT){
	// 	return (*nearest).intensity;
	// }

	// cout<<closest<<"sgjhvj"<<endl;

	Color diff(0,0,0);
	Color spec(0,0,0);

	for (std::vector<Light*>::iterator it = sources.begin() ; it != sources.end(); ++it)
	{
		// cout << typeid(*it).name() << endl;
		if((*it)->type = LIGHT_POINT)
		{	
			Point_3d direc = (*it)->location.subtract(closest);
			Line light_ray(closest,direc);
			
			float min_int = std::numeric_limits<float>::max();
			Point_3d closest_int = light_ray.ro;
			Object *nearest_int;

			for (std::vector<Object*>::iterator it = objects.begin() ; it != objects.end(); ++it)
			{
				// cout << typeid(*it).name() << endl;
				try{
					// intersect = true;
					Point_3d p = (*it)->intersection(light_ray);
					// cout<< p <<endl;
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

			// Add Diffused and Spectral components
			
			// Diffusion
			Point_3d dir = direc;
			dir.normalize();
			diff = diff.add(diffusion(dir,(*it)->intensity,(*nearest).normal(closest),(*nearest).kd));

			//Spectral
			Point_3d view_vector = l.rd.multiply(-1);
			view_vector.normalize();
			spec = spec.add(specular(dir,(*it)->intensity,(*nearest).normal(closest),(*nearest).ks,(*nearest).n_spec,view_vector));
		}
		else if((*it)->type = LIGHT_DIREC)
		{

		}
	
	}

	// Add Ambient 
	Color ambi = Ambient_Intensity.multiply((*nearest).ka);

	final = final.add(diff);
	final = final.add(spec);
	final = final.add(ambi);

	Color refl(0,0,0);
	// ADD REFLECTION COMP
	if(depth <= MAX_DEPTH)
	{
		Point_3d incident = l.rd.multiply(-1);
		incident.normalize();
		Point_3d normal_at_poi = (*nearest).normal(closest);
		Point_3d reflected_direction = incident.reflected(normal_at_poi);
		reflected_direction.normalize();
		Line reflected(closest,reflected_direction);
		refl = illumination(reflected, objects, sources, depth+1,i,j);
	}
	refl = refl.multiply(k_reflection);

	final.add(refl);

	return final;
}

Point_3d v_to_w(Point_3d p, Point_3d r, Point_3d u, Point_3d v, Point_3d n){
    Point_3d p_(p.x*u.x + p.y*v.x + p.z*n.x, p.x*u.y + p.y*v.y + p.z*n.y, p.x*u.z + p.y*v.z + p.z*n.z);
    return p_.add(r);
}

Point_3d w_to_v(Point_3d p, Point_3d r, Point_3d u, Point_3d v, Point_3d n){
    Point_3d p_ = p.subtract(r);
    p_ = Point_3d(p_.dot(u), p_.dot(v), p_.dot(n));
    return p_;
}

void click(vector<Object*> objects, vector<Light*> sources, Point_3d eye, float E, Point_3d n){
    Point_3d up(0,0,1);
    Point_3d v = up.subtract(n.multiply(up.dot(n)));
    v.normalize();

    Point_3d u = n.cross(v);
    u.normalize();

    cout<<u<<endl;
    cout<<v<<endl;
    cout<<n<<endl;

    // Color image[2*height + 1][2*width + 1];
    // image_glob = new char[(2*height+1)*(2*width+1)*3];
    
    Point_3d camera = eye.add(n.multiply(E));
    Point_3d eye_(0,0,-1*E);
    for(int i = -height; i<=height; i++){
        for(int j = -width; j<=width; j++){
		    // cout<<"YAYYY"<<i<<" " << j<<endl;
            Point_3d ray = (eye_.subtract((Point_3d(i,j,0)))).multiply(-1);
            ray = v_to_w(ray, camera, u,v,n);
            ray.normalize();
            Line l(eye,ray);
            // cout<<l<<endl;
            // image[i][j] = illumination(l, objects, sources, 0);
            Color temp;
            temp = illumination(l, objects, sources, 0, i, j);
            // image_glob[i][j][0] = (int)temp.r;
            // image_glob[i][j][1] = (int)temp.g;
            // image_glob[i][j][2] = (int)temp.b;
            image_glob[((i+height)*width+(j+width))*3+0] = (int)temp.r;
            image_glob[((i+height)*width+(j+width))*3+1] = (int)temp.g;
            image_glob[((i+height)*width+(j+width))*3+2] = (int)temp.b;
        }
    }
}

void display()
{
    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

    // char data[height][width][3];
    // for( size_t y = 0; y < height; ++y )
    // {
    //     for( size_t x = 0; x < width; ++x )
    //     {
    //         // data[y][x][0] = ( rand() % 256 ) ;//* 256 * 256 * 256;
    //         // data[y][x][1] = ( rand() % 256 ) ;//* 256 * 256 * 256;
    //         // data[y][x][2] = ( rand() % 256 ) ;//* 256 * 256 * 256;
    //         image_glob[(y*width+x)*3+0] = 128 ;
    //         image_glob[(y*width+x)*3+1] = 128;
    //         image_glob[(y*width+x)*3+2] = 0;

    //     }
    // }

    glDrawPixels( 2*width+1, 2*height+1, GL_RGB, GL_UNSIGNED_BYTE, image_glob );

    glutSwapBuffers();
}

int main(int argc, char **argv){
		// Point_3d eye(0,0,0);
		// Point_3d dirn(0,0,1.0);
		// Point_3d pt1(2,0,1);
		// Point_3d pt2(-2,0,1);
		// Point_3d pt3(0,4,1);

		// Triangle* t = new Triangle(pt1,pt2,pt3);
		// Plane pl = t->p;
		// Sphere* s = new Sphere(pt1,0.1);

		// Line l(eye,dirn);
		// Point_3d p = closest_intersection(l,objects);
		// cout << p.x << " " << p.y << " " << p.z << endl;

	Color k(0.5,0.5,0.5);
	Plane* wall1 = new Plane(1,0,0,0,k,k,k,2);
	Plane* wall2 = new Plane(0,1,0,0,k,k,k,2);
	Plane* wall3 = new Plane(0,0,1,0,k,k,k,2);
	Plane* wall4 = new Plane(1,0,0,-1000,k,k,k,2);
	Plane* wall5 = new Plane(0,1,0,-1000,k,k,k,2);
	Plane* wall6 = new Plane(0,0,1,-1000,k,k,k,2);

	Color k1(0.2,0.2,0.8);
	Sphere* ball = new Sphere(Point_3d(500,500,500), 50, k1,k1,k1,2);
	Point_source* light = new Point_source(Point_3d(500,500,1000), Color(256,0,0));

	std::vector<Object*> objects;
	objects.push_back(wall1);
	objects.push_back(wall2);
	objects.push_back(wall3);
	objects.push_back(wall4);
	objects.push_back(wall5);
	objects.push_back(wall6);
	objects.push_back(ball);

	std::vector<Light*> lights;
	lights.push_back(light);

	Point_3d dirn(1000,1000,-1000);
	dirn.normalize();
	
    image_glob = new char[(2*height+1)*(2*width+1)*3];

	click(objects, lights, Point_3d(0,0,1000), 20, dirn);
	
	cout<<"YAYYY"<<endl;
	
	glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    glutInitWindowSize( 2*width+1, 2*height+1 );
    glutCreateWindow( "GLUT" );
    glutDisplayFunc( display );
    glutMainLoop();

	return 0;
}