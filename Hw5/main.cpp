#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <random>
#include <algorithm>
#include <Eigen>

using namespace Eigen;

// image background color
Vector3f bgcolor(1.0f, 1.0f, 1.0f);

// lights in the scene
std::vector<Vector3f> lightPositions = { Vector3f(  0.0, 60, 60)
                                       , Vector3f(-60.0, 60, 60)
                                       , Vector3f( 60.0, 60, 60) };

class Sphere
{
public:
	Vector3f center;  // position of the sphere
	float radius;  // sphere radius
	Vector3f surfaceColor; // surface color
	
  Sphere(
		const Vector3f &c,
		const float &r,
		const Vector3f &sc) :
		center(c), radius(r), surfaceColor(sc)
	{
	}

  // ray x sphere intersection
	bool intersect(const Vector3f &rayOrigin, const Vector3f &rayDirection, float &t0, float &t1) const
	{
		Vector3f l = center - rayOrigin;
		float tca = l.dot(rayDirection);
		if (tca < 0) return false;
		float d2 = l.dot(l) - tca * tca;
		if (d2 > (radius * radius)) return false;
    float thc = sqrt(radius * radius - d2);
		t0 = tca - thc;
		t1 = tca + thc;

		return true;
	}
};

// diffuse reflection model
Vector3f diffuse(const Vector3f &L, // direction vector from the point on the surface towards a light source
	const Vector3f &N, // normal at this point on the surface
	const Vector3f &diffuseColor,
	const float kd // diffuse reflection constant
	)
{
	Vector3f resColor = Vector3f::Zero();

	// TODO: implement diffuse shading model

	return resColor;
}

// Phong reflection model
Vector3f phong(const Vector3f &L, // direction vector from the point on the surface towards a light source
               const Vector3f &N, // normal at this point on the surface
               const Vector3f &V, // direction pointing towards the viewer
               const Vector3f &diffuseColor, 
               const Vector3f &specularColor, 
               const float kd, // diffuse reflection constant
               const float ks, // specular reflection constant
               const float alpha) // shininess constant
{
	Vector3f resColor = Vector3f::Zero();

	// TODO: implement Phong shading model

	return resColor;
}

Vector3f trace(
	const Vector3f &rayOrigin,
	const Vector3f &rayDirection,
	const std::vector<Sphere> &spheres)
	
{
	//light one 0,60,60
	//light two -60,60,60
	//light three 60,60,60
	//center of spheres 1: {1.00000000, 0.319999993, 0.360000014}  rad:4 Red
	//center of spheres 2: {5.00000000, -1.00000000, -15.0000000}  rad:2 Gold
	//center of spheres 3: {5.00000000, 0.000000000, -25.0000000}  rad:3 Blue
	//center of spheres 4: {-5.50000000, 0.000000000, -13.0000000} rad:3 Grey


	Vector3f pixelColor = Vector3f::Zero();
	bool rayTrue = false;
	bool setColor = false;
	float t0;
	float t1;
	std::vector<float> light1Red;
	float xPos;
	Sphere mySphere = spheres[0];
	pixelColor = bgcolor;
	mySphere = spheres[0];
	rayTrue = mySphere.intersect(rayOrigin, rayDirection, t0, t1);
	for (int j = 0; j < lightPositions.size(); j++)
	{
		xPos = lightPositions[0][j];
		//light1Red = sqrt(pow(lightPositions[j][0] - 1, 2) + pow(lightPositions[j][ 1] - .31, 2) + pow(lightPositions[j][2] - .36, 2));
	}
	if (rayTrue)
	{
				pixelColor[0] = 0.5f;
				pixelColor[1] = 0.5f;
				pixelColor[2] = 0.5f;
	}

	mySphere = spheres[3];
	rayTrue = mySphere.intersect(rayOrigin, rayDirection, t0, t1);
	if (rayTrue)
		{
			pixelColor[0] = .65f;
			pixelColor[1] = .77f;
			pixelColor[2] = .97f;			
		}
	for (int i = 0; i < spheres.size(); i++)
	{
		mySphere = spheres[i];
		rayTrue = mySphere.intersect(rayOrigin, rayDirection, t0, t1);
		if (rayTrue == true)
		{
			switch (i)
			{
			case 1:
			{
				pixelColor[0] = 1.0f;
				pixelColor[1] = .32f;
				pixelColor[2] = .36f;
				break;
			}
		
			case 2:
			{
				pixelColor[0] = .9f;
				pixelColor[1] = .76f;
				pixelColor[2] = .46f;
				break;
			}
			case 4:
			{
				pixelColor[0] = .9f;
				pixelColor[1] = .9f;
				pixelColor[2] = .9f;
				break;
			}
		
			}
		}

	}

	

	//std::cout << "help" << pixelColor;
	// TODO: implement ray tracing as described in the homework description
	
	return pixelColor;
}

void render(const std::vector<Sphere> &spheres)
{
  unsigned width = 640;
  unsigned height = 480;
  Vector3f *image = new Vector3f[width * height];
  Vector3f *pixel = image;
  float invWidth  = 1 / float(width);
  float invHeight = 1 / float(height);
  float fov = 30;
  float aspectratio = width / float(height);
	float angle = tan(M_PI * 0.5f * fov / 180.f);
	
	// Trace rays
	for (unsigned y = 0; y < height; ++y) 
	{
		for (unsigned x = 0; x < width; ++x) 
		{
			float rayX = (2 * ((x + 0.5f) * invWidth) - 1) * angle * aspectratio;
			float rayY = (1 - 2 * ((y + 0.5f) * invHeight)) * angle;
			Vector3f rayDirection(rayX, rayY, -1);
			rayDirection.normalize();
			*(pixel++) = trace(Vector3f::Zero(), rayDirection, spheres);
		}
	}
	
	// Save result to a PPM image
	std::ofstream ofs("./render.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned i = 0; i < width * height; ++i) 
	{
		const float x = image[i](0);
		const float y = image[i](1);
		const float z = image[i](2);

		ofs << (unsigned char)(std::min(float(1), x) * 255) 
			  << (unsigned char)(std::min(float(1), y) * 255) 
			  << (unsigned char)(std::min(float(1), z) * 255);
	}
	
	ofs.close();
	delete[] image;
}

int main(int argc, char **argv)
{
	std::vector<Sphere> spheres;
	// position, radius, surface color
	spheres.push_back(Sphere(Vector3f(0.0, -10004, -20), 10000, Vector3f(0.50, 0.50, 0.50)));
	spheres.push_back(Sphere(Vector3f(0.0, 0, -20), 4, Vector3f(1.00, 0.32, 0.36)));
	spheres.push_back(Sphere(Vector3f(5.0, -1, -15), 2, Vector3f(0.90, 0.76, 0.46)));
	spheres.push_back(Sphere(Vector3f(5.0, 0, -25), 3, Vector3f(0.65, 0.77, 0.97)));
	spheres.push_back(Sphere(Vector3f(-5.5, 0, -13), 3, Vector3f(0.90, 0.90, 0.90)));

	render(spheres);

	return 0;
}
