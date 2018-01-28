#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "objects.h"
#include <math.h>
#include <vector>
#include <limits>
#include <typeinfo>
#include <GL/glut.h>
using namespace std;

Color Ambient_Intensity(80,80,80);
// Color k_reflection(0.7,0.7,0.7);
// Color k_transmission(0.5,0.5,0.5);
int MAX_DEPTH = 1;
const int width = 250;
const int height = 250;
unsigned char image_glob[2*height + 1][2*width + 1][3];

float distance(Point_3d p1, Point_3d p2){
	return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y) + (p1.z - p2.z)*(p1.z - p2.z));
}

Color diffusion(Point_3d light, Color intensity, Point_3d normal, Color kd)
{
	return intensity.multiply(kd.multiply(abs(light.dot(normal))));
}

Color specular(Point_3d light, Color intensity, Point_3d normal, Color ks, int n_spec, Point_3d view)
{
	Point_3d ref = light.reflected(normal);
	return intensity.multiply(ks.multiply(pow(abs(ref.dot(view)),n_spec)));
}

Color illumination(Line l, vector<Object*> objects, vector<Light*> sources, int depth, int i, int j){

	Color final(0,0,0);

	float min = std::numeric_limits<float>::max();
	Point_3d closest = l.ro;
	Object *nearest;

	for (std::vector<Object*>::iterator it = objects.begin() ; it != objects.end(); ++it){
		// cout << typeid(*it).name() << endl;
		try{
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
			direc.normalize();
			Line light_ray(closest,direc);
			
			float min_int = std::numeric_limits<float>::max();
			Point_3d closest_int = light_ray.ro;
			Object *nearest_int;

			for (std::vector<Object*>::iterator it2 = objects.begin() ; it2 != objects.end(); ++it2)
			{
				// cout << typeid(*it2).name() << endl;
				try{
					// intersect = true;
					Point_3d p = (*it2)->intersection(light_ray);
					// cout<< p <<endl;
					float d = distance(light_ray.ro,p);
					if(d<min_int){
						min_int = d;
						closest_int = p;	//Point of intersection
						nearest_int = (*it2);	//Intersecting object

						// cout<<"yay"<<endl;
					}
					// cout << distance(l.ro,p)<<endl;
				} catch(const char* msg){
					// cout << "No intersection" << endl;
				}
			}
			float source_dist = distance(closest,(*it)->location);
			if(min_int + 0.001 < source_dist ){
				// cout<<light_ray<<endl;
				// cout<<closest<<endl;
				// cout<<min_int<<" "<<source_dist<<endl;
				// cout<<closest_int << " " << (*nearest_int).type<<endl;
				// cout<<"BLOCKED"<<endl;
				continue;	// Light blocked
			}

			// Add Diffused and Spectral components
			
			// Diffusion
			Point_3d dir = direc;
			// dir.normalize();
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
		if((*nearest).kr.r > 0.01 || (*nearest).kr.g > 0.01 || (*nearest).kr.b > 0.01)
		{
			Point_3d incident = l.rd.multiply(-1);
			incident.normalize();
			Point_3d normal_at_poi = (*nearest).normal(closest);
			if(incident.dot(normal_at_poi) < 0 )
			{
				normal_at_poi = normal_at_poi.multiply(-1);
			}
			Point_3d reflected_direction = incident.reflected(normal_at_poi);
			reflected_direction.normalize();
			Line reflected(closest,reflected_direction);
			refl = illumination(reflected, objects, sources, depth+1,i,j);	
		}
	}
	refl = refl.multiply((*nearest).kr);

	final = final.add(refl);

	// cout<<"Prefinal"<<final<<endl;
	// cout<<refl<<endl;
	// cout<<final<<endl;
	return final;
}

Point_3d v_to_w(Point_3d p, Point_3d r, Point_3d u, Point_3d v, Point_3d n){
    Point_3d p_(p.x*u.x + p.y*v.x + p.z*n.x, p.x*u.y + p.y*v.y + p.z*n.y, p.x*u.z + p.y*v.z + p.z*n.z);
    return p_;
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

    // cout<<u<<endl;
    // cout<<v<<endl;
    // cout<<n<<endl;

    // Color image[2*height + 1][2*width + 1];
    // image_glob = new char[(2*height+1)*(2*width+1)*3];
    
    Point_3d camera = eye.add(n.multiply(E));
    // cout<<camera<<"camera"<<endl;
    Point_3d eye_(0,0,-1*E);


    // int i=160;
    // int j = 236;
    // Point_3d ray = (eye_.subtract((Point_3d(i,j,0)))).multiply(-1);
    // // cout<<ray<<endl;
    // ray = v_to_w(ray, camera, u,v,n);
    // ray.normalize();
    // Line l(eye,ray);
    // // cout<<ray<<endl;
    // // image[i][j] = illumination(l, objects, sources, 0);
    // Color temp;
    // temp = illumination(l, objects, sources, 0, i, j);
    // // cout<<temp<<endl;

    ofstream img;
    img.open("image.ppm");
    img << "P3\n" << 2*width + 1 << " " << 2*height+1 << "\n255\n";
    for(int i = height; i>=-height; i--){
	    for(int j = -width; j<=width; j++){
		    // cout<<"YAYYY"<<i<<" " << j<<endl;
            Point_3d ray = (eye_.subtract((Point_3d(i,j,0)))).multiply(-1);
            // cout<<ray<<endl;
            ray = v_to_w(ray, camera, u,v,n);
            ray.normalize();
            Line l(eye,ray);
            // cout<<ray<<endl;
            // image[i][j] = illumination(l, objects, sources, 0);
            Color temp;
            temp = illumination(l, objects, sources, 0, i, j);
            // cout<< i << " " << j << " " << temp << endl;
            image_glob[i + height][j + width][0] = (unsigned char)temp.r;
            image_glob[i + height][j + width][1] = (unsigned char)temp.g;
            image_glob[i + height][j + width][2] = (unsigned char)temp.b;
            img << (temp.r<255?(int) temp.r : 255) << " ";
            img << (temp.g<255?(int) temp.g : 255) << " ";
            img << (temp.b<255?(int) temp.b : 255) << " ";
            if(temp.r < 50 && temp.g < 50 && temp.b < 50){
            	cout << i << " " << j << endl;
            }
            // image_glob[((i+height)*width+(j+width))*3+0] = (int)temp.r;
            // image_glob[((i+height)*width+(j+width))*3+1] = (int)temp.g;
            // image_glob[((i+height)*width+(j+width))*3+2] = (int)temp.b;
        }
        img << endl;
    }
    img.close();
}

void display()
{
    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

    // char data[height][width][3];
    // for( size_t y = 0; y < 2*height + 1; ++y )
    // {
    //     for( size_t x = 0; x < 2* width + 1; ++x )
    //     {
    //         // image_glob[y][x][0] = ( rand() % 256 ) ;//* 256 * 256 * 256;
    //         // image_glob[y][x][1] = ( rand() % 256 ) ;//* 256 * 256 * 256;
    //         // data[y][x][2] = ( rand() % 256 ) ;//* 256 * 256 * 256;
    //         image_glob[(y*width+x)*3+0] = (rand() % 256);
    //         image_glob[(y*width+x)*3+1] = (rand() % 256);
    //         image_glob[(y*width+x)*3+2] = (rand() % 256);

    //     }
    // }


    // for(int i=0; i<(2*height+1)*(2*width+1)*3;i++){
    // 	if(i%3==0){
    // 		cout<<endl;
    // 	}
    // 	cout<<(image_glob[i])<<" ";
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
	Color k0(0,0,0);
	Plane* wall1 = new Plane(1,0,0,0,k,k,k,k0,k0,2);
	Plane* wall2 = new Plane(0,1,0,0,k,k,k,k0,k0,2);
	Plane* wall3 = new Plane(0,0,1,0,k,k,k,k0,k0,2);
	Plane* wall4 = new Plane(1,0,0,-10000,k,k,k,k0,k0,2);
	Plane* wall5 = new Plane(0,1,0,-10000,k,k,k,k0,k0,2);
	Plane* wall6 = new Plane(0,0,1,-10000,k,k,k,k0,k0,2);

	Color k1(0.2,0.8,0.2);
	Color k2(0.7,0.7,0.7);
	Sphere* ball = new Sphere(Point_3d(5000,5000,500), 500, k,k,k,k0,k0,2);
	Rectangle* mirror = new Rectangle(Point_3d(2000,9990,8000),Point_3d(8000,9990,8000),Point_3d(8000,9990,2000),Point_3d(2000,9990,2000),k0,k0,k0,k2,k2,2);
	// Point_source* light = new Point_source(Point_3d(5000,5000,10000), Color(255,255,255));
	// Point_source* light2 = new Point_source(Point_3d(5000,9800,5000), Color(255,255,255));
	Point_source* light3 = new Point_source(Point_3d(9800,5000,8000), Color(0,255,255));
	Point_source* light4 = new Point_source(Point_3d(200,5000,8000), Color(255,255,0));

	std::vector<Object*> objects;
	objects.push_back(wall1);
	objects.push_back(wall2);
	objects.push_back(wall3);
	objects.push_back(wall4);
	objects.push_back(wall5);
	objects.push_back(wall6);
	objects.push_back(ball);
	objects.push_back(mirror);

	std::vector<Light*> lights;
	// lights.push_back(light);
	// lights.push_back(light2);
	lights.push_back(light3);
	lights.push_back(light4);

	Point_3d eye(5000,500,9500);
	Point_3d dirn = eye.subtract(Point_3d(5000,10000,0)).multiply(-1);
	dirn.normalize();
	
    // image_glob = new unsigned int [(2*height+1)*(2*width+1)*3];

	click(objects, lights, eye, 500, dirn);
	
	// cout<<"YAYYY"<<endl;
	
	// glutInit( &argc, argv );
 //    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
 //    glutInitWindowSize( 2*width+1, 2*height+1 );
 //    glutCreateWindow( "GLUT" );
 //    glutDisplayFunc( display );
 //    glutMainLoop();

	// cout<<dirn<<endl;

	return 0;
}