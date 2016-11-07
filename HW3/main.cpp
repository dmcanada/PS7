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
	bool angle = abs(dx) < abs(dy);
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
		if (angle){
			while (y2 <= y)
			{
				if (dee <= 0)
				{
					dee = dee - inCrease1;
				}
				else
				{
					dee = dee + inCrease0;
					x = x + 1;
				}
				y = y - 1;				
				putPixel(x, y);
			}
		}
		else
		{
			while (x <= x2)
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
	putPixel(y + xc, x + yc);
	putPixel(x + xc, -y + yc);
	putPixel(y + xc, -x + yc);
	putPixel(-x + xc, y + yc);
	putPixel(-y + xc, x + yc);
	putPixel(-x + xc, -y + yc);
	putPixel(-y + xc, -x + yc);

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

void drawEllipse(int xC, int yC, int width, int height)
{
	int width_2 = width * width;
	int height_2 = height * height;;
	int increase0 = 4 * width_2;
	int increase1 = 4 * height_2;
	int x = 0;
	int y = height;

	int dee = 2 * height_2 + width_2*(1 - 2 * height);
	int dee2 = 2 * width_2 + height_2 * (1 - 2 * width);

	while (height_2*x <= width_2*y)
	{
		eclipsePoints(x, y, xC, yC, 0xffffff);
		if (dee >= 0)
		{
			dee += increase0 * (1 - y);
			y--;
		}
		dee += height_2  * ((4 * x) + 6);
		x++;
	}

	x = width;
	y = 0;

	while (width_2*y <= height_2*x)
	{
		y++;
		eclipsePoints(x, y, xC, yC, 0x333333);
		if (dee2 >= 0)
		{
			dee2 = dee2 + (increase1 * (1 - x));
			x = x - 1;
		}
		dee2 = dee2 + width_2 * ((4 * y) + 6);
	}
}

	
void drawImage()
{
	int m = 0;
	/*for (int x = 0; x < 590; x++)
		putPixel(x, x);*/
	/*
	drawLine(150, 10, 450, 10);
	drawLine(150, 310, 450, 310);
	drawLine(150, 10, 150, 310);
	drawLine(450, 10, 450, 310);
	drawLine(150, 310, 300, 410);
	drawLine(300, 410, 450, 310);
	drawLine(300, 110, 150, 310);
	
	drawCircle(500, 500, 50);
	*/
	
	drawCircle(600, 333, 50);
	drawEllipse(600, 333, 27, 49);
	drawEllipse(100, 475, 27, 55);
	drawEllipse(100, 493, 48, 10);
	for (int i = 50; i >0; i--)
	{
	drawEllipse(100+(2*i), 500-(5*i), i+1, i);
	}
	drawEllipse(300, 475, 27, 55);
	drawEllipse(300, 493, 48, 10);
	for (int i = 50; i >0; i--)
	{
	drawEllipse(300 - (2 * i), 500 - (5 * i), i + 1, i);
	}
	//mountains
	drawLine(0, 113, 56, 147);
	drawLine(56, 146 ,71, 142);
	drawLine(50, 110, 150, 310);
	drawLine(150, 210, 197, 170);
	drawLine(198, 177, 250, 210);
	drawLine(250, 210, 450, 110);
	drawLine(309, 169, 370, 220);
	drawLine(370, 220, 413, 180);
	//space ship
	drawCircle(200, 550, 47);
	drawEllipse(200, 497, 187, 65);
	drawLine(314, 523, 371, 523);
	drawLine(239, 523, 286, 523);
	drawLine(113, 523, 161, 523);
	drawLine(28, 523, 87, 523);
	
	drawEllipse(200, 533, 3, 30);
	drawLine(200, 477, 200, 533);
	drawCircle(392, 489, 6);
	drawLine(390, 495, 398, 495);
	drawLine(390, 483, 398, 483);
	for (int j = 0; j < 13; j++)
	{
		drawLine(157+j, 482-(j*2), 243-j, 482-(j*2));
		if (j == 12)
		{
			for (int k = 0; k < 9; k++)
			drawLine(169 - k, 453 - (k * 2), 231 + k, 453 - (k * 2));
		}
	}
	drawLine(355, 132, 450, 230);
	drawLine(450, 228, 497, 190);
	drawLine(498, 197, 550, 230);
	drawLine(550, 230, 600, 190);
	
	for (int n = 0; n < 8; n++)
	{
		m = m + (n + 17)*n;
		drawCircle(412 + m, 487+ ((n+1)*2), 7+n);
	}
	
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
