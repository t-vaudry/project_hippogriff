#include <windows.h>
#include <time.h>
#include <iostream>
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <stdlib.h>
#include <thread>

using namespace std;

#define WIDTH 1024
#define HEIGHT 768
#define X_SIZE 0.001953125
#define Y_SIZE 0.002604166

char* speciesA;
char* speciesB;

void glutTimer(int value);
void display();
void draw();
void initializeGrid();
void checkConditions(char* ary);
int count(int i, int j, char* ary);

int main(int argc, char** argv)
{
	initializeGrid();
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Game of Life - Multiple Species");
	glutTimerFunc(33, glutTimer, 1);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}

void glutTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(33, glutTimer, 1);
}

void display()
{
	thread t1(checkConditions, speciesA);
	thread t2(checkConditions, speciesB);

	t1.join();
	t2.join();

	draw();
	glutSwapBuffers();
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);
	float x;
	float y = 1.002604166;
	int i = -1;
	int j = 0;

	for (int k = 0; k < 786432; k++)
	{
		j = k % 1024;
		if (k % 1024 == 0) {
			i++;
			x = -1;
			y -= Y_SIZE;
		}
		glBegin(GL_POLYGON);
		
		if (speciesA[i*HEIGHT + j] != 'n')
		{
			
			if (speciesA[i*HEIGHT + j] == 'a')
				glColor3f(1.0f, 0.0f, 0.0f);
			else
				glColor3f(0.0f, 0.0f, 0.0f);
		}
		else
		{
			if (speciesB[i*HEIGHT + j] == 'a')
				glColor3f(0.0f, 0.0f, 1.0f);
			else
				glColor3f(0.0f, 0.0f, 0.0f);
		}
			glVertex2f(x, y - Y_SIZE);
			glVertex2f(x, y);
			glVertex2f(x + X_SIZE, y);
			glVertex2f(x + X_SIZE, y - Y_SIZE);
		glEnd();
		x += X_SIZE;
	}
}

void initializeGrid()
{
	speciesA = new char[786432];
	speciesB = new char[786432];

	for (int i = 0; i<786432; i++)
	{
		int type = rand();
		int status = rand();
		if (type % 2 == 1)
		{
			if (status % 2 == 1)
				speciesA[i] = 'a';
			else
				speciesA[i] = 'd';

			speciesB[i] = 'n';
		}
		else
		{
			if (status % 2 == 1)
				speciesB[i] = 'a';
			else
				speciesB[i] = 'd';

			speciesA[i] = 'n';
		}
	}
}

void checkConditions(char* ary)
{
	int i = -1;
	int j = 0;
	int numNeighbors = 0;

	for (int k = 0; k < 786432; k++)
	{
		j = k % 1024;
		if (k % 1024 == 0) {
			i++;
		}
		numNeighbors = count(i, j, ary);

		if (ary[i*HEIGHT + j] == 'a')
		{
			switch (numNeighbors)
			{
			case 0:
			case 1:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
				ary[i*HEIGHT + j] = 'd';
				break;
			case 2:
			case 3:
			default:
				break;
			}
		}
		else if ((ary[i*HEIGHT + j] == 'd') && (numNeighbors == 3))
		{
			ary[i*HEIGHT + j] = 'a';
		}
	}
}

int count(int i, int j, char* ary)
{
	int count = 0;

	if (i == 0)
	{
		if (j == 0)
		{
			if (ary[i*HEIGHT + j + 1] == 'a')
				count++;
			if (ary[(i + 1)*HEIGHT + j] == 'a')
				count++;
			if (ary[(i + 1)*HEIGHT + j + 1] == 'a')
				count++;
		}
		else if (j == WIDTH - 1)
		{
			if (ary[(i + 1)*HEIGHT + j - 1] == 'a')
				count++;
			if (ary[(i + 1)*HEIGHT + j - 1] == 'a')
				count++;
			if (ary[(i + 1)*HEIGHT + j] == 'a')
				count++;
		}
		else
		{
			if (ary[i*HEIGHT + j - 1] == 'a')
				count++;
			if (ary[i*HEIGHT + j + 1] == 'a')
				count++;
			if (ary[(i + 1)*HEIGHT + j - 1] == 'a')
				count++;
			if (ary[(i + 1)*HEIGHT + j] == 'a')
				count++;
			if (ary[(i + 1)*HEIGHT + j + 1] == 'a')
				count++;
		}
	}
	else if (i == HEIGHT - 1)
	{
		if (j == 0)
		{
			if (ary[(i - 1)*HEIGHT + j] == 'a')
				count++;
			if (ary[(i - 1)*HEIGHT + j + 1] == 'a')
				count++;
			if (ary[i*HEIGHT + j + 1] == 'a')
				count++;
		}
		else if (j == WIDTH - 1)
		{
			if (ary[(i - 1)*HEIGHT + j + 1] == 'a')
				count++;
			if (ary[(i - 1)*HEIGHT + j] == 'a')
				count++;
			if (ary[i*HEIGHT + j - 1] == 'a')
				count++;
		}
		else
		{
			if (ary[(i - 1)*HEIGHT + j + 1] == 'a')
				count++;
			if (ary[(i - 1)*HEIGHT + j] == 'a')
				count++;
			if (ary[(i - 1)*HEIGHT + j + 1] == 'a')
				count++;
			if (ary[i*HEIGHT + j - 1] == 'a')
				count++;
			if (ary[i*HEIGHT + j + 1] == 'a')
				count++;
		}
	}
	else if (j == 0)
	{
		if (ary[(i - 1)*HEIGHT + j] == 'a')
			count++;
		if (ary[(i - 1)*HEIGHT + j + 1] == 'a')
			count++;
		if (ary[i*HEIGHT + j + 1] == 'a')
			count++;
		if (ary[(i + 1)*HEIGHT + j] == 'a')
			count++;
		if (ary[(i + 1)*HEIGHT + j + 1] == 'a')
			count++;
	}
	else if (j == WIDTH - 1)
	{
		if (ary[(i - 1)*HEIGHT + j + 1] == 'a')
			count++;
		if (ary[(i - 1)*HEIGHT + j] == 'a')
			count++;
		if (ary[i*HEIGHT + j - 1] == 'a')
			count++;
		if (ary[(i + 1)*HEIGHT + j - 1] == 'a')
			count++;
		if (ary[(i + 1)*HEIGHT + j] == 'a')
			count++;
	}
	else
	{
		if (ary[(i - 1)*HEIGHT + j + 1] == 'a')
			count++;
		if (ary[(i - 1)*HEIGHT + j] == 'a')
			count++;
		if (ary[(i - 1)*HEIGHT + j + 1] == 'a')
			count++;
		if (ary[i*HEIGHT + j - 1] == 'a')
			count++;
		if (ary[i*HEIGHT + j + 1] == 'a')
			count++;
		if (ary[(i + 1)*HEIGHT + j - 1] == 'a')
			count++;
		if (ary[(i + 1)*HEIGHT + j] == 'a')
			count++;
		if (ary[(i + 1)*HEIGHT + j + 1] == 'a')
			count++;
	}

	return count;
}