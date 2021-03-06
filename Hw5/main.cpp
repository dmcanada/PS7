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
	Vector3f coloring = Vector3f::Zero();
	float maxValue;
	float difColor;
	maxValue = L.dot(N);
	if (maxValue < 0)
	{
		maxValue = 0;
	}

	resColor = 0.333 * kd * maxValue* diffuseColor ;
	//difColor = 0.333 * kd * maxValue;
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
	Vector3f R = Vector3f::Zero();
	
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
	Vector3f resetColor;
	Vector3f normalVector = Vector3f::Zero();
	float t0_0;
	float t1_0;
	float t0_1;
	float t1_1;
	bool rayTrue = false;
	bool LrayTrue = false;
	Vector3f colorFactor1 = Vector3f::Zero();
	float t0Posintersect;
	int closestOrb = -1;
	std::vector<int> closestshad;
	float checkt0;
	float mint0;
	float cne;
	bool skip = false;
	float mint1 = -1;
	float LMag;
	float NMag;
	Sphere mySphere = spheres[0];
	pixelColor = bgcolor;
	Vector3f LvectorOrigin = Vector3f::Zero();
	Vector3f Lvector = Vector3f::Zero();
	Vector3f veiwVector = Vector3f::Zero();
	std::vector<float> color;
	float colorFactor = 0;
	bool miss = false;
	bool hit = false;
	

	for (int i = 0; i < spheres.size(); i++)
	{
		if (i != 3)
		{
			mySphere = spheres[i];

			rayTrue = mySphere.intersect(rayOrigin, rayDirection, t0_0, t1_0);

			if (rayTrue == true)
			{
				checkt0 = t0_0;
				if (closestOrb == -1)
				{
					mint0 = checkt0;
					closestOrb = i;
				}
				else if (checkt0 < mint0)
				{

					mint0 = checkt0;
					closestOrb = i;
				}
			}
		}
	}
	if (closestOrb > -1)
	{
		mySphere = spheres[closestOrb];
		LvectorOrigin = { rayOrigin[0] + (rayDirection[0] * t0_0), rayOrigin[1] + (rayDirection[1] * t0_0), rayOrigin[2] + (rayDirection[2] * t0_0) };
		for (int k = 0; k < lightPositions.size(); k++)
		{
			Lvector = { lightPositions[k][0] - LvectorOrigin[0], lightPositions[k][1] - LvectorOrigin[1], lightPositions[k][2] - LvectorOrigin[2] };
			LMag = sqrt(pow(Lvector[0], 2) + pow(Lvector[1], 2) + pow(Lvector[2], 2));
			Lvector[0] = Lvector[0] / LMag;
			Lvector[1] = Lvector[1] / LMag;
			Lvector[2] = Lvector[2] / LMag;
			miss = false;
			hit = false;
			
			normalVector = { LvectorOrigin[0] - mySphere.center[0], LvectorOrigin[1] - mySphere.center[1], LvectorOrigin[2] - mySphere.center[2] };
			NMag = sqrt(pow(normalVector[0], 2) + pow(normalVector[1], 2) + pow(normalVector[2], 2));
			normalVector[0] = normalVector[0] / NMag;
			normalVector[1] = normalVector[1] / NMag;
			normalVector[2] = normalVector[2] / NMag;

			
			for (int m = 0; m < spheres.size(); m++)
			{
				mySphere = spheres[m];
				LrayTrue = mySphere.intersect(LvectorOrigin, Lvector, t0_1, t1_1);
				
					if (LrayTrue != false)
					{
						hit = true;
						miss = false;
					}
					else
					{
						miss = true;
					}
				}
				
			
			
				if (miss == true && hit == false)
				{
					mySphere = spheres[closestOrb];
					resetColor = diffuse(Lvector, normalVector, mySphere.surfaceColor, 1.0f);
					//resetColor = phong(Lvector, normalVector, rayDirection, mySphere.surfaceColor, Vector3f::Ones(), 1.0f,
					//	3.0f, 100.0f);
						//const Vector3f &V, // direction pointing towards the viewer
					//colorFactor += .33f;
					/*
					colorFactor1[0] += .33f;
					colorFactor1[1] += .33f;
					colorFactor1[2] += .33f;
					*/
					colorFactor1[0] += resetColor[0];
					colorFactor1[1] += resetColor[1];
					colorFactor1[2] += resetColor[2];
					//*/
				}
			
		}
		
		
		switch (closestOrb)
		{
		case 0:
		{
			
			pixelColor[0] = 0.5f *colorFactor1[0];
			pixelColor[1] = 0.5f *colorFactor1[1];
			pixelColor[2] = 0.5f *colorFactor1[2];
			break;  
		}

		case 1:
		{
			pixelColor[0] = 1.0f * colorFactor1[0];
			pixelColor[1] = .32f * colorFactor1[1];
			pixelColor[2] = .36f * colorFactor1[2];
			break;
			  
		}
		
		case 2:
		{
			pixelColor[0] = .9f * colorFactor1[0];
			pixelColor[1] = .76f * colorFactor1[1];
			pixelColor[2] = .46f * colorFactor1[2];
			break; 
		}
		case 3:
		{
			pixelColor[0] = .65f * colorFactor1[0];
			pixelColor[1] = .77f * colorFactor1[1];
			pixelColor[2] = .97f * colorFactor1[2];
			break; 
		}
		case 4:
		{
			pixelColor[0] = .9f *colorFactor1[0];
			pixelColor[1] = .9f *colorFactor1[1];
			pixelColor[2] = .9f *colorFactor1[2];
			break; 
		}
	deflaut:
		{
			break;
		}


		}
		/*
		switch (closestOrb)
		{
		case 0:
		{
			pixelColor[0] = 0.5f * colorFactor;
			pixelColor[1] = 0.5f * colorFactor;
			pixelColor[2] = 0.5f * colorFactor;
			break;
		}

		case 1:
		{
			pixelColor[0] = 1.0f * colorFactor;
			pixelColor[1] = .32f * colorFactor;
			pixelColor[2] = .36f *colorFactor;
			break;
		}

		case 2:
		{
			pixelColor[0] = .9f * colorFactor;
			pixelColor[1] = .76f * colorFactor;
			pixelColor[2] = .46f * colorFactor;
			break;
		}
		case 3:
		{
			pixelColor[0] = .65f * colorFactor;
			pixelColor[1] = .77f * colorFactor;
			pixelColor[2] = .97f * colorFactor;
			break;
		}
		case 4:
		{
			pixelColor[0] = .9f * colorFactor;
			pixelColor[1] = .9f * colorFactor;
			pixelColor[2] = .9f * colorFactor;
			break;
		}
	deflaut:
		{
			break;
		}


		}*/
	}

	//std::cout << "help" << pixelColor;
	// TODO: implement ray tracing as described in the homework description
	
	return pixelColor;
}
// auxiliary math functions
float dotProduct(const float* a, const float* b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
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
