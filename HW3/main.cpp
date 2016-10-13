#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>


#define M_PI 3.141592654f

unsigned int g_windowWidth = 600;
unsigned int g_windowHeight = 600;
char* g_windowName = "HW3-Rasterization";

GLFWwindow* g_window;

const int g_image_width = g_windowWidth;
const int g_image_height = g_windowHeight;

std::vector<float> g_image;

struct color
{
	unsigned char r, g, b;
};

int ReadLine(FILE *fp, int size, char *buffer)
{
	int i;
	for (i = 0; i < size; i++) {
		buffer[i] = fgetc(fp);
		if (feof(fp) || buffer[i] == '\n' || buffer[i] == '\r') {
			buffer[i] = '\0';
			return i + 1;
		}
	}
	return i;
}

//-------------------------------------------------------------------------------

void glfwErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error " << error << ": " << description << std::endl;
	exit(1);
}

void glfwKeyCallback(GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods)
{
	if (p_key == GLFW_KEY_ESCAPE && p_action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(g_window, GL_TRUE);
	}
}

void initWindow()
{
	// initialize GLFW
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
	{
		std::cerr << "GLFW Error: Could not initialize GLFW library" << std::endl;
		exit(1);
	}

	g_window = glfwCreateWindow(g_windowWidth, g_windowHeight, g_windowName, NULL, NULL);
	if (!g_window)
	{
		glfwTerminate();
		std::cerr << "GLFW Error: Could not initialize window" << std::endl;
		exit(1);
	}

	// callbacks
	glfwSetKeyCallback(g_window, glfwKeyCallback);

	// Make the window's context current
	glfwMakeContextCurrent(g_window);

	// turn on VSYNC
	glfwSwapInterval(1);
}

void initGL()
{
	glClearColor(1.f, 1.f, 1.f, 1.0f);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawPixels(g_image_width, g_image_height, GL_LUMINANCE, GL_FLOAT, &g_image[0]);
}

void renderLoop()
{
	while (!glfwWindowShouldClose(g_window))
	{
		// clear buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render();

		// Swap front and back buffers
		glfwSwapBuffers(g_window);

		// Poll for and process events
		glfwPollEvents();
	}
}

void initImage()
{
	g_image.resize(g_image_width * g_image_height);
}

bool writeImage()
{
	std::vector<color> tmpData;
	tmpData.resize(g_image_width * g_image_height);

	for (int i = 0; i < g_image_height; i++)
	{
		for (int j = 0; j < g_image_width; j++)
		{
			// make sure the value will not be larger than 1 or smaller than 0, which might cause problem when converting to unsigned char
			float tmp = g_image[i* g_image_width + j];
			if (tmp < 0.0f)	tmp = 0.0f;
			if (tmp > 1.0f)	tmp = 1.0f;

			tmpData[(g_image_height - i - 1)* g_image_width + j].r = unsigned char(tmp * 255.0);
			tmpData[(g_image_height - i - 1)* g_image_width + j].g = unsigned char(tmp * 255.0);
			tmpData[(g_image_height - i - 1)* g_image_width + j].b = unsigned char(tmp * 255.0);
		}
	}

	FILE *fp = fopen("data/out.ppm", "wb");
	if (!fp) return false;

	fprintf(fp, "P6\r");
	fprintf(fp, "%d %d\r", g_image_width, g_image_height);
	fprintf(fp, "255\r");
	fwrite(tmpData.data(), sizeof(color), g_image_width * g_image_height, fp);

	return true;
}

//-------------------------------------------------------------------------------

void putPixel(int x, int y)
{
	// clamp
	if (x >= g_image_width || x < 0 || y >= g_image_height || y < 0) return;

	// write
	g_image[y* g_image_width + x] = 1.0f;
	
}

void drawLine(int x1, int y1, int x2, int y2)
{
	// Task 1
	int dx = x2 - x1;
	int dy = y2 - y1;
	float mx = 0;
	float my = 0;
	int roundY = 0;
	int x = x1;
	int y = y1;
	if (dx == 0)
	{
		mx = 0;
	}
	else
	{
		mx = float(dy) / float(dx);
	}


	int slope = 0;
	if (x2 < x1)
	{
		x1 = x2;
		x2 = x;
		x = x1;
		y1 = y2;
		y2 = y;
		y = y1;
		slope = 2;
	}

	float dee = (2 * dy) - dx;
	float inCrease0 = 2 * dy;
	float inCrease1 = 2 * (dy - dx);


	if (dx == 0)
	{
		slope = 1;
	}
	else if (slope == 2)
	{
		slope = 2;
	}
	else if (mx < 0)
	{
		slope = 4;
	}
	else
	{
		slope = 3;
	}
	putPixel(x, y);

	switch (slope)
	{
	case 1:
	{
		while (y < y2)
		{
			y = y + 1;
			putPixel(x, y);

		}
		break;
	}
	case 2:
	{
		while (x <= x2)
		{
			if (dee <= 0)
			{
				dee = dee - inCrease0;
			}
			else
			{
				dee = dee + inCrease1;
				y = y - 1;
			}
			x = x + 1;
			putPixel(x, y);
		}
		break;
	}
	case 3:
	{
		while (x < x2)
		{
			if (dee <= 0)
			{
				dee = dee + inCrease0;

			}
			else
			{
				dee = dee + inCrease1;
				y = y + 1;
			}
			x = x + 1;
			putPixel(x, y);
		}
		break;
	}
	case 4:
	{
		while (y2 <= y)
		{
			if (dee <= 0)
			{
				dee = dee - inCrease1;

			}
			else
			{

				dee = dee + inCrease0;
				y = y - 1;

			}


			x = x + 1;
			putPixel(x, y);

		}
		break;
	}

	}




}
void circlePoints(int x0, int y0, int xC, int yC, int color)
{
	int x = x0;
	int y = y0;
	int xc = xC;
	int yc = yC;
	int colorPixel = color;
	putPixel(x + xc, y + yc);
	putPixel(y + yc, x + xc);
	putPixel(x + xc, -y + yc);
	putPixel(y + yc, -x + xc);
	putPixel(-x + xc, y + yc);
	putPixel(-y + yc, x + xc);
	putPixel(-x + xc, -y + yc);
	putPixel(-y + yc, -x + xc);
}
void eclipsePoints(int x0, int y0, int xC, int yC, int color)
{
	int x = x0;
	int y = y0;
	int xc = xC;
	int yc = yC;
	putPixel(x + xc, y + yc);
	putPixel(-x + xc, y + yc);
	putPixel(x + xc, -y + yc);
	putPixel(-x + xc, -y + yc);
}


void drawCircle(int x0, int y0, int R)
{
	// Task 2
	int xC = x0;
	int yC = y0;
	int r = R;
	int dee = 0;
	int x = 0;
	int y = r;
	dee = 1 - r;
	circlePoints(x, y, xC, yC, 0x333333);

	while (y > x)
	{
		if (dee < 0)
		{
			dee = dee + 2 * x + 3;

		}
		else
		{
			dee = dee + 2 * (x - y) + 5;
			y = y - 1;

		}
		x = x + 1;
		circlePoints(x, y, xC, yC, 0x333333);
	}

}

void drawEclipse(int xC , int yC, int width, int height)
{
	
	int width_2 = width * width;
	//int a2 = width * width;
	int height_2 = height * height;
	//int b2 = height * height;
	int increase0 = 4 * width_2;
	//int fa2 = 4 * a2,
	int increase1 = 4 * height_2;
	//fb2 = 4 * b2;
	int x = 0;
	int y = height;
	int dee = 2 * height_2+ width_2*(1-2*height);
	
	while (height_2*x <= width_2*y)
	{
		eclipsePoints(x, y, xC, yC, 0xffffff);
		if (dee >= 0)
		{
			dee += increase0 * (1 - y);
			y--;
		}
		dee += increase1 * ((4 * x) + 6);
		x++;
	}
	
	x = width;
	y = 0;
	dee = 2 * width_2 + height_2 * (1 - 2 * width);
	while (width_2*y <= height_2*x)
	{
		y++;
		eclipsePoints(x, y, xC, yC, 0x333333);
		if (dee >= 0)
		{
			dee = dee + (increase1 * (1 - x));
			x = x - 1;
		}

		dee = dee + increase0 * ((4 * y) + 6);
		
	}
		/*
	second half 
	for (x = width, y = 0, sigma = 2 * a2 + b2*(1 - 2 * width); a2*y <= b2*x; y++)
	{
		DrawPixel(xc + x, yc + y);
		DrawPixel(xc - x, yc + y);
		DrawPixel(xc + x, yc - y);
		DrawPixel(xc - x, yc - y);
		if (sigma >= 0)
		{
			sigma += fb2 * (1 - x);
			x--;
		}
		sigma += a2 * ((4 * y) + 6);
	}
}
*/

}
	
void drawImage()
{
	/*for (int x = 0; x < 590; x++)
		putPixel(x, x);*/
	drawLine(150, 10, 450, 10);
	drawLine(150, 310, 450, 310);
	drawLine(150, 10, 150, 310);
	drawLine(450, 10, 450, 310);
	drawLine(150, 310, 300, 410);
	drawLine(300, 410, 450, 310);
	drawLine(300, 110, 150, 310);

	drawCircle(500, 500, 50);

	drawEclipse(100, 500, 50, 75);
}



int main()
{
	initImage();
	drawImage();
	writeImage();

	// render loop
	initWindow();
	initGL();
	renderLoop();

	return 0;
}
