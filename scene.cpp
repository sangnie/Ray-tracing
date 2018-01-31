#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "objects.h"
#include <math.h>
#include <vector>
#include <limits>
#include <typeinfo>
// #include <GL/glut.h>
using namespace std;

Color Ambient_Intensity(80,80,80);
// Color k_reflection(0.7,0.7,0.7);
// Color k_transmission(0.5,0.5,0.5);
int MAX_DEPTH = 2;
const int width = 250;
const int height = 250;
int image_glob[2*height + 1][2*width + 1][3];
int anti_alias[2*height + 2][2*width + 2][3];

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

Color illumination(Line l, vector<Object*> objects, vector<Light*> sources, int depth, bool inside){

	Color final(0,0,0);
	// cout << "CLICK" << endl;
	float min = std::numeric_limits<float>::max();
	Point_3d closest = l.ro;
	Object *nearest;
	bool intersecting = false;

	for (std::vector<Object*>::iterator it = objects.begin() ; it != objects.end(); ++it){
		// cout << typeid(*it).name() << endl;
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

	// cout << (*nearest).type << endl;
	// if((*nearest).type == LIGHT_POINT){
	// 	return (*nearest).intensity;
	// }
	if(!intersecting)
	{
		for (std::vector<Light*>::iterator it = sources.begin() ; it != sources.end(); ++it){
			Point_3d r1 = l.rd.multiply(-1);
			r1.normalize();
			float dot = r1.dot((*it)->direction);
			if(dot > 0.999 && dot < 1.001){
				return (*it)->intensity;
			}

		}	
		return Color(0,0,0);	
	}



	// cout<<closest<<"sgjhvj"<<endl;

	Color diff(0,0,0);
	Color spec(0,0,0);

	for (std::vector<Light*>::iterator it = sources.begin() ; it != sources.end(); ++it)
	{
		// cout << typeid(*it).name() << endl;
		if((*it)->type == LIGHT_POINT)
		{	
			// cout<<"c";
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
		else if((*it)->type == LIGHT_DIREC)
		{
			Point_3d direc = (*it)->direction.multiply(-1);
			direc.normalize();
			Line light_ray(closest,direc);
			
			float min_int = std::numeric_limits<float>::max();
			Point_3d closest_int = light_ray.ro;
			Object *nearest_int;
			bool intersectingg = false;

			for (std::vector<Object*>::iterator it2 = objects.begin() ; it2 != objects.end(); ++it2)
			{
				// cout << typeid(*it2).name() << endl;
				try{
					Point_3d p = (*it2)->intersection(light_ray);
					intersectingg = true;
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
			// float source_dist = distance(closest,(*it)->location);
			// if(min_int + 0.001 < source_dist ){
			// 	// cout<<light_ray<<endl;
			// 	// cout<<closest<<endl;
			// 	// cout<<min_int<<" "<<source_dist<<endl;
			// 	// cout<<closest_int << " " << (*nearest_int).type<<endl;
			// 	// cout<<"BLOCKED"<<endl;
			// 	continue;	// Light blocked
			// }

			if(intersectingg)
				continue;

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
		else if((*it)->type == LIGHT_SPOT)
		{
			// cout<<"a";
			Point_3d direc = (*it)->location.subtract(closest);
			direc.normalize();
			Line light_ray(closest,direc);
			// Check if within angle

			Point_3d view_at_source = direc.multiply(-1);

			if(view_at_source.dot((*it)->direction) < (*it)->dot_min)
			{
				// cout<<"b";
				continue;
			}
			
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


			// cout<<"n";
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
			// cerr << "yay1" <<endl;
			// cout<< closest << endl;
			// cout <<l << endl;
			// cout << incident << endl;
			// cout << reflected_direction << endl;
			refl = illumination(reflected, objects, sources, depth+1,inside);	
			// cerr << "yay2" <<endl;
		}
	}
	refl = refl.multiply((*nearest).kr);

	final = final.add(refl);

	Color trans(0,0,0);
	// ADD TRANSMISSION COMP
	if(depth <= MAX_DEPTH && (*nearest).type == SPHERE)
	{
		if((*nearest).kt.r > 0.01 || (*nearest).kt.g > 0.01 || (*nearest).kt.b > 0.01)
		{
			if(inside){
				// cout << "WHOOPIEE" << endl;
			}
			Point_3d incident = l.rd;
			Point_3d normal_at_poi = (*nearest).normal(closest);
			if(incident.dot(normal_at_poi) > 0 )
			{
				normal_at_poi = normal_at_poi.multiply(-1);
			}
			// Point_3d reflected_direction = l.reflected(normal_at_poi);
			float eta = (*nearest).index;
			float ratio = (inside) ? eta : 1/eta;
			float cosi = -incident.dot(normal_at_poi);

			if(!inside || (cosi*cosi > (1 - (1/(eta*eta))))){
				// cout << "YIPEE" << endl;
				float cost = sqrt(1 - ratio*ratio*(1-(cosi*cosi)));
				Point_3d trans_dirn = (incident.multiply(ratio)).add(normal_at_poi.multiply(ratio*cosi - cost));

				trans_dirn.normalize();
				Line transmitted(closest,trans_dirn);
				trans = illumination(transmitted, objects, sources, depth+1,!inside);
			} else {
				cout << "daisy" << endl;
			}
		}
	}
	trans = trans.multiply((*nearest).kt);

	final = final.add(trans);

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


    // int i = 21;
    // int j = -125;
    // Point_3d ray = (eye_.subtract((Point_3d(i,j,0)))).multiply(-1);
    // cout<<ray<<endl;
    // ray = v_to_w(ray, camera, u,v,n);
    // ray.normalize();
    // Line l(eye,ray);
    // cout<<ray<<endl;
    // // image[i][j] = illumination(l, objects, sources, 0);
    // Color temp;
    // temp = illumination(l, objects, sources, 0, false);
    // cout<<temp<<endl;

    ofstream img;
    img.open("image.ppm");
    img << "P3\n" << 2*width + 1 << " " << 2*height+1 << "\n255\n";
    
    /////////// Normal fn
    // for(int i = height; i>=-height; i--){
	   //  for(int j = -width; j<=width; j++){
		  //   // cout<<"YAYYY"<<i<<" " << j<<endl;
    //         Point_3d ray = (eye_.subtract((Point_3d(i,j,0)))).multiply(-1);
    //         // cout<<ray<<endl;
    //         ray = v_to_w(ray, camera, u,v,n);
    //         ray.normalize();
    //         Line l(eye,ray);
    //         // cout<<ray<<endl;
    //         // image[i][j] = illumination(l, objects, sources, 0);
    //         Color temp;
    //         temp = illumination(l, objects, sources, 0, i, j);
    //         // cout<< i << " " << j << " " << temp << endl;
    //         image_glob[i + height][j + width][0] = (unsigned char)temp.r;
    //         image_glob[i + height][j + width][1] = (unsigned char)temp.g;
    //         image_glob[i + height][j + width][2] = (unsigned char)temp.b;
    //         img << (temp.r<255?(int) temp.r : 255) << " ";
    //         img << (temp.g<255?(int) temp.g : 255) << " ";
    //         img << (temp.b<255?(int) temp.b : 255) << " ";
    //         // if(temp.r < 50 && temp.g < 50 && temp.b < 50){
    //         // 	cout << i << " " << j << endl;
    //         // }
    //         // image_glob[((i+height)*width+(j+width))*3+0] = (int)temp.r;
    //         // image_glob[((i+height)*width+(j+width))*3+1] = (int)temp.g;
    //         // image_glob[((i+height)*width+(j+width))*3+2] = (int)temp.b;
    //     }
    //     img << endl;
    // }


    ///////// ANTIALIASING
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
            temp = illumination(l, objects, sources, 0, false);

            // if(i==150 && j==200)
            // {
            // 	cout<<"a "<<temp<<endl;
            // 	cout<<(unsigned char)temp.r;
            // 	cout<<(unsigned char)temp.g;
            // 	cout<<(unsigned char)temp.b;
            // }

            // cout<< i << " " << j << " " << temp << endl;
            image_glob[i + height][j + width][0] = (int)temp.r;
            image_glob[i + height][j + width][1] = (int)temp.g;
            image_glob[i + height][j + width][2] = (int)temp.b;
            // img << (temp.r<255?(int) temp.r : 255) << " ";
            // img << (temp.g<255?(int) temp.g : 255) << " ";
            // img << (temp.b<255?(int) temp.b : 255) << " ";

            // if(temp.r < 50 && temp.g < 50 && temp.b < 50){
            // 	cout << i << " " << j << endl;
            // }
            // image_glob[((i+height)*width+(j+width))*3+0] = (int)temp.r;
            // image_glob[((i+height)*width+(j+width))*3+1] = (int)temp.g;
            // image_glob[((i+height)*width+(j+width))*3+2] = (int)temp.b;
        }
        img << endl;
    }

    for(int i = -height; i<=height+1; i++){
	    for(int j = -width; j<=width+1; j++){
		    // cout<<"YAYYY"<<i<<" " << j<<endl;
            Point_3d ray = (eye_.subtract((Point_3d(i-0.5,j-0.5,0)))).multiply(-1);
            // cout<<ray<<endl;
            ray = v_to_w(ray, camera, u,v,n);
            ray.normalize();
            Line l(eye,ray);
            // cout<<ray<<endl;
            // image[i][j] = illumination(l, objects, sources, 0);
            Color temp;
            temp = illumination(l, objects, sources, 0, false);
            // cout<< i << " " << j << " " << temp << endl;
            anti_alias[i + height][j + width][0] = (int)temp.r;
            anti_alias[i + height][j + width][1] = (int)temp.g;
            anti_alias[i + height][j + width][2] = (int)temp.b;
            // img << (temp.r<255?(int) temp.r : 255) << " ";
            // img << (temp.g<255?(int) temp.g : 255) << " ";
            // img << (temp.b<255?(int) temp.b : 255) << " ";
            // if(temp.r < 50 && temp.g < 50 && temp.b < 50){
            // 	cout << i << " " << j << endl;
            // }
            // image_glob[((i+height)*width+(j+width))*3+0] = (int)temp.r;
            // image_glob[((i+height)*width+(j+width))*3+1] = (int)temp.g;
            // image_glob[((i+height)*width+(j+width))*3+2] = (int)temp.b;
        }
        img << endl;
    }

	for(int j = 2*width; j>=0 ; j--){
	    for(int i = 0; i<=2*height ; i++){
    		int tempr = (int)image_glob[i][j][0] + (int)anti_alias[i][j][0] + (int)anti_alias[i][j+1][0] + (int)anti_alias[i+1][j][0] + (int)anti_alias[i+1][j+1][0];
    		tempr = tempr/5;
    		int tempg = (int)image_glob[i][j][1] + (int)anti_alias[i][j][1] + (int)anti_alias[i][j+1][1] + (int)anti_alias[i+1][j][1] + (int)anti_alias[i+1][j+1][1];
    		tempg = tempg/5;
    		int tempb = (int)image_glob[i][j][2] + (int)anti_alias[i][j][2] + (int)anti_alias[i][j+1][2] + (int)anti_alias[i+1][j][2] + (int)anti_alias[i+1][j+1][2];
    		tempb = tempb/5;
    		// if(tempr<40 || tempg<40 || tempb<40)
    		// {
    		// 	cout<<"W"<<" "<<tempr<<" "<<tempg<<" "<<tempb<<" "<<i<<" "<<j<<endl;
    		// }
            img << (tempr<255?(int) tempr : 255) << " ";
            img << (tempg<255?(int) tempg : 255) << " ";
            img << (tempb<255?(int) tempb : 255) << " ";

    	}

    }



    img.close();
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
	Color k1(0.7,0.5,0.7);
	Color k2(0.7,0.7,0.7);
	Color k4(0.4,0.4,0.4);
	Color red(0.8,0.5,0.5);
	Color yellow(0.4,0.7,0.7);
	Color pink(0.7,0.5,0.6);
	Color green(0.3,0.5,0.7);
	Color kall(1,1,1);
	Color ksmall(0.2,0.2,0.2);
	Plane* wall1 = new Plane(1,0,0,0,yellow,yellow,yellow,k0,k0,2);
	Plane* wall2 = new Plane(0,1,0,0,pink,pink,pink,k0,k0,2);
	Plane* wall3 = new Plane(0,0,1,0,green,green,green,k0,k0,2);
	Plane* wall4 = new Plane(1,0,0,-10000,yellow,yellow,yellow,k0,k0,2);
	Plane* wall5 = new Plane(0,1,0,-10000,pink,pink,pink,k0,k0,2);
	Plane* wall6 = new Plane(0,0,1,-10000,green,green,green,k0,k0,2);

	Rectangle* box1 = new Rectangle(Point_3d(6000,8000,0),Point_3d(7000,8000,0),Point_3d(7000,8000,1000),Point_3d(6000,8000,1000),k1,k1,k1,k0,k0,2);
	Rectangle* box2 = new Rectangle(Point_3d(6000,7000,0),Point_3d(7000,7000,0),Point_3d(7000,7000,1000),Point_3d(6000,7000,1000),k1,k1,k1,k0,k0,2);
	Rectangle* box3 = new Rectangle(Point_3d(6000,8000,0),Point_3d(6000,7000,0),Point_3d(6000,7000,1000),Point_3d(6000,8000,1000),k1,k1,k1,k0,k0,2);
	Rectangle* box4 = new Rectangle(Point_3d(7000,8000,0),Point_3d(7000,7000,0),Point_3d(7000,7000,1000),Point_3d(7000,8000,1000),k1,k1,k1,k0,k0,2);


	Sphere* ball = new Sphere(Point_3d(5000,5000,5000), 500, k1,k1,k1,k0,k0,2,1);
	Sphere* glass = new Sphere(Point_3d(7000,7000,3000), 500, yellow,yellow,yellow,ksmall,k0,2,2.6);

	Rectangle* bottom = new Rectangle(Point_3d(0,0,0),Point_3d(0,10000,0),Point_3d(10000,10000,0),Point_3d(10000,0,0),red,red,red,k0,k0,2);
	Rectangle* top = new Rectangle(Point_3d(10000,10000,10000),Point_3d(0,10000,10000),Point_3d(0,0,10000),Point_3d(10000,0,10000),k,k,k,k0,k0,2);
	Rectangle* left = new Rectangle(Point_3d(0,0,0),Point_3d(0,0,10000),Point_3d(0,10000,10000),Point_3d(0,10000,0),k,k,k,k0,k0,2);
	Rectangle* right = new Rectangle(Point_3d(10000,0,0),Point_3d(10000,10000,0),Point_3d(10000,10000,10000),Point_3d(10000,0,10000),k,k,k,k0,k0,2);
	Rectangle* front = new Rectangle(Point_3d(0,10000,0),Point_3d(0,10000,10000),Point_3d(10000,10000,10000),Point_3d(10000,10000,0),k,k,k,k0,k0,2);
	Rectangle* back = new Rectangle(Point_3d(0,0,10000),Point_3d(0,0,0),Point_3d(10000,0,0),Point_3d(10000,0,10000),k,k,k,k0,k0,2);

	Rectangle* mirror1 = new Rectangle(Point_3d(2000,9990,8000),Point_3d(8000,9990,8000),Point_3d(8000,9990,2000),Point_3d(2000,9990,2000),k0,k0,k0,k2,k2,2);
	Rectangle* mirror2 = new Rectangle(Point_3d(3000,6000,0),Point_3d(3000,6000,3000),Point_3d(4000,4000,3000),Point_3d(4000,4000,0),k0,k0,k0,k2,k2,2);

	Point_source* light = new Point_source(Point_3d(5000,5000,10000), Color(255,255,255));
	Point_source* light2 = new Point_source(Point_3d(5000,9800,5000), Color(255,255,255));
	Point_source* light3 = new Point_source(Point_3d(9800,5000,8000), Color(255,255,255));
	Point_source* light4 = new Point_source(Point_3d(200,5000,8000), Color(255,255,0));

	Point_3d sphere_centre(5000,5000,2000);
	Spotlight* light5 = new Spotlight(Point_3d(9990,5000,8000), sphere_centre.subtract(Point_3d(9990,5000,8000)), 0.97 ,Color(255,255,0));

	Quadric* ellipsoid = new Quadric(4,1,1,0,0,0,-40000,-10000,-10000,149750000,k1,k1,k1,k0,k0,2);

	Direction_source* sun = new Direction_source(Point_3d(10,-5,-20),Color(255,255,255)); 

	std::vector<Object*> objects;
	// objects.push_back(wall1);
	// objects.push_back(wall2);
	// objects.push_back(wall3);
	// objects.push_back(wall4);
	// objects.push_back(wall5);
	// objects.push_back(wall6);
	// objects.push_back(ball);
	objects.push_back(glass);
	// objects.push_back(mirror1);
	// objects.push_back(mirror2);
	// objects.push_back(box1);
	// objects.push_back(box2);
	// objects.push_back(box3);
	// objects.push_back(box4);
	// objects.push_back(ellipsoid);

	objects.push_back(bottom);
	// // objects.push_back(top);
	// objects.push_back(front);
	// objects.push_back(back);
	// objects.push_back(left);
	// objects.push_back(right);

	std::vector<Light*> lights;
	// lights.push_back(light);
	// lights.push_back(light2);
	// lights.push_back(light3);
	// lights.push_back(light4);
	// lights.push_back(light5);
	lights.push_back(sun);

	// cout<<"spot"<<light5->type<<" "<<light5->direction<<" "<<light5->intensity<<" "<<light5->location<<" "<<light5->dot_min<<endl;

	Point_3d eye(1000,1000,9500);
	Point_3d dirn = eye.subtract(Point_3d(10000,10000,0)).multiply(-1);
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

// void display()
// {
//     glClearColor( 0, 0, 0, 1 );
//     glClear( GL_COLOR_BUFFER_BIT );

//     // char data[height][width][3];
//     // for( size_t y = 0; y < 2*height + 1; ++y )
//     // {
//     //     for( size_t x = 0; x < 2* width + 1; ++x )
//     //     {
//     //         // image_glob[y][x][0] = ( rand() % 256 ) ;//* 256 * 256 * 256;
//     //         // image_glob[y][x][1] = ( rand() % 256 ) ;//* 256 * 256 * 256;
//     //         // data[y][x][2] = ( rand() % 256 ) ;//* 256 * 256 * 256;
//     //         image_glob[(y*width+x)*3+0] = (rand() % 256);
//     //         image_glob[(y*width+x)*3+1] = (rand() % 256);
//     //         image_glob[(y*width+x)*3+2] = (rand() % 256);

//     //     }
//     // }


//     // for(int i=0; i<(2*height+1)*(2*width+1)*3;i++){
//     // 	if(i%3==0){
//     // 		cout<<endl;
//     // 	}
//     // 	cout<<(image_glob[i])<<" ";
//     // }

//     glDrawPixels( 2*width+1, 2*height+1, GL_RGB, GL_UNSIGNED_BYTE, image_glob );

//     glutSwapBuffers();
// }

// int main(int argc, char **argv){
	
// 	ifstream ifs;
// 	ifs.open(argv[1]);

// 	if (!ifs) {
//         cout << "Unable to open file";
//         exit(1); // terminate with error
//     }

//     Point_3d eye;
//     float E;
//     Point_3d n;
//     ifs >> eye.x >> eye.y >> eye.z >> E >> n.x >> n.y >> n.z;
//     n.normalize();

// 	std::vector<Object*> objects;
// 	std::vector<Light*> lights;
	
//     while(!ifs.eof()){
//     	string type;
//     	ifs >> type;

//     	if(type == "POINT SOURCE"){
//     		Color intn;
//     		Point_3d loc;
//     		ifs >> intn.r >> intn.g >> intn.b >> loc.x >> loc.y >> loc.z;
//     		lights.push_back(new Point_source(loc, intn));
//     	}
//     	if(type == "DIRECTION SOURCE"){
//     		Color intn;
//     		Point_3d dirn;
//     		ifs >> intn.r >> intn.g >> intn.b >> dirn.x >> dirn.y >> dirn.z;
//     		dirn.normalize();
//     		lights.push_back(new Direction_source(dirn, intn));	
//     	}
//     	if(type == "SPHERE"){
//     		Point_3d centre;
//     		float r;
//     		Color ks;
//     		Color kd;
//     		Color ka;
//     		Color kr;
//     		Color kt;
//     		int n_spec;
//     		ifs >> centre.x >> centre.y >> centre.z >> r;
//     		ifs >> ks.r >> ks.g >> ks.b >> kd.r >> kd.g >> kd.b >> ka.r >> ka.g >> ka.b >> kr.r >> kr.g >> kr.b >> kt.r >> kt.g >> kt.b >> n_spec; 
//     		objects.push_back(new Sphere(centre,r,ks, kd, ka, kr, kt , n_spec));
//     	}
//     	if(type == "TRIANGLE"){
//     		Point_3d p1;
//     		Point_3d p2;
//     		Point_3d p3;
//     		Color ks;
//     		Color kd;
//     		Color ka;
//     		Color kr;
//     		Color kt;
//     		int n_spec;
//     		ifs >> p1.x>>p1.y>>p1.z;
//     		ifs >> p2.x>>p2.y>>p2.z;
//     		ifs >> p3.x>>p3.y>>p3.z;
//     		ifs >> ks.r >> ks.g >> ks.b >> kd.r >> kd.g >> kd.b >> ka.r >> ka.g >> ka.b >> kr.r >> kr.g >> kr.b >> kt.r >> kt.g >> kt.b >> n_spec; 
//     		objects.push_back(new Triangle(p1,p2,p3,ks, kd, ka, kr, kt , n_spec));
//     	}
//     	if(type == "PLANE"){
//     		float a,b,c,d;
//     		Color ks;
//     		Color kd;
//     		Color ka;
//     		Color kr;
//     		Color kt;
//     		int n_spec;
//     		ifs >> a >> b >> c >> d;
//     		ifs >> ks.r >> ks.g >> ks.b >> kd.r >> kd.g >> kd.b >> ka.r >> ka.g >> ka.b >> kr.r >> kr.g >> kr.b >> kt.r >> kt.g >> kt.b >> n_spec; 
//     		objects.push_back(new Plane(a,b,c,d,ks, kd, ka, kr, kt , n_spec));
//     	}
//     	if(type == "RECTANGLE"){
//     		Point_3d p1;
//     		Point_3d p2;
//     		Point_3d p3;
//     		Point_3d p4;    	
//     		Color ks;
//     		Color kd;
//     		Color ka;
//     		Color kr;
//     		Color kt;
//     		int n_spec;
//     		ifs >> p1.x>>p1.y>>p1.z;
//     		ifs >> p2.x>>p2.y>>p2.z;
//     		ifs >> p3.x>>p3.y>>p3.z;
//     		ifs >> p4.x>>p4.y>>p4.z;
//     		ifs >> ks.r >> ks.g >> ks.b >> kd.r >> kd.g >> kd.b >> ka.r >> ka.g >> ka.b >> kr.r >> kr.g >> kr.b >> kt.r >> kt.g >> kt.b >> n_spec; 
//     		objects.push_back(new Rectangle(p1,p2,p3,p4,ks, kd, ka, kr, kt , n_spec));
//     	}
//     	if(type == "QUADRIC"){
//     		float a,b,c,de,f,g,h,i,j;
//     		Color ks;
//     		Color kd;
//     		Color ka;
//     		Color kr;
//     		Color kt;
//     		int n_spec;
//     		ifs >> a >> b >> c >> d >>e>>f>>g>>h>>i>>j;
//     		ifs >> ks.r >> ks.g >> ks.b >> kd.r >> kd.g >> kd.b >> ka.r >> ka.g >> ka.b >> kr.r >> kr.g >> kr.b >> kt.r >> kt.g >> kt.b >> n_spec; 
//     		Quadric(a, b, c, d, e, f, g, h, i, j, ks, kd, ka, kr,kt,n_spec){
//     	}
//     	if(type == "CIRCLE"){
//     		Point_3d centre;
//     		float r;
//     		Plane p;
//     		Color ks;
//     		Color kd;
//     		Color ka;
//     		Color kr;
//     		Color kt;
//     		int n_spec;
//     		ifs >> centre.x >> centre.y >> centre.z >> r;
//     		ifs >> p.a >> p.b >> p.c >> p.d;
//     		ifs >> ks.r >> ks.g >> ks.b >> kd.r >> kd.g >> kd.b >> ka.r >> ka.g >> ka.b >> kr.r >> kr.g >> kr.b >> kt.r >> kt.g >> kt.b>>n_spec;
//     		objects.push_back(new Circle(centre,r,p,ks, kd, ka, kr, kt , n_spec)); 
//     	}
//     }

//     click(objects, lights, eye, E, n);

// 	return 0;	
// }