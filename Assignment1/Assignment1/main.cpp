#include <windows.h>
#include <time.h>
#include <iostream>
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <stdlib.h>
#include <vector>
#include <thread>

using namespace std;

#define WIDTH 1024
#define HEIGHT 768
#define X_SIZE 0.001953125
#define Y_SIZE 0.002604166

int numOfSpecies;
int* species;
bool* state;

void glutTimer(int value);
void display();
void draw();
void initializeGrid();
void checkConditions(int type);
int count(int i, int j, int type);

int main(int argc, char** argv)
{
	cout << "Enter number of species: " << endl;
	cin >> numOfSpecies;

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
	vector<thread> threads;

	for (int i = 0; i < numOfSpecies; i++)
	{
		threads.push_back(thread(checkConditions, i));
		threads[i].join();
	}

	draw();
	glutSwapBuffers();
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);
	GLfloat x;
	GLfloat y = 1.002604166;
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
			if (state[i*HEIGHT + j])
			{
				if(species[i*HEIGHT + j] == 0)
					glColor3f(1.0f, 0.0f, 0.0f); //red
				else if(species[i*HEIGHT + j] == 1)
					glColor3f(0.0f, 0.0f, 1.0f); //blue
				else if (species[i*HEIGHT + j] == 2)
					glColor3f(0.0f, 1.0f, 0.0f); //green
				else if (species[i*HEIGHT + j] == 3)
					glColor3f(1.0f, 1.0f, 0.0f); //red-green
				else if (species[i*HEIGHT + j] == 4)
					glColor3f(0.0f, 1.0f, 1.0f); //blue-green
				else if (species[i*HEIGHT + j] == 5)
					glColor3f(1.0f, 0.0f, 1.0f); //red-blue
				else if (species[i*HEIGHT + j] == 6)
					glColor3f(1.0f, 1.0f, 1.0f); //white
				else if (species[i*HEIGHT + j] == 7)
					glColor3f(0.0f, 0.5f, 0.0f); //half-green
				else if (species[i*HEIGHT + j] == 8)
					glColor3f(0.5f, 0.0f, 0.0f); //half-red
				else if (species[i*HEIGHT + j] == 9)
					glColor3f(0.0f, 0.0f, 0.5f); //half-blue
			}
			else
				glColor3f(0.0f, 0.0f, 0.0f);
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
	species = new int[786432];
	state = new bool[786432];

	for (int i = 0; i<786432; i++)
	{
		int type = rand();
		int status = rand();
		species[i] = type % numOfSpecies;

		if (status % 2 == 1)
			state[i] = true; //alive
		else
			state[i] = false; //dead
	}
}

void checkConditions(int type)
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
		
		if (species[i*HEIGHT + j] == type)
		{
			numNeighbors = count(i, j, type);

			if (state[i*HEIGHT + j])
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
					state[i*HEIGHT + j] = false; //kill
					break;
				case 2:
				case 3:
				default:
					break;
				}
			}
			else if (!(state[i*HEIGHT + j]) && (numNeighbors == 3))
			{
				state[i*HEIGHT + j] = true; //revive
			}
		}
	}
}

int count(int i, int j, int type)
{
	int count = 0;

	if (i == 0)
	{
		if (j == 0)
		{
			if ((state[i*HEIGHT + j + 1])&&(species[i*HEIGHT + j + 1] == type))
				count++;
			if ((state[(i + 1)*HEIGHT + j])&&(species[(i + 1)*HEIGHT + j] == type))
				count++;
			if ((state[(i + 1)*HEIGHT + j + 1])&&(species[(i + 1)*HEIGHT + j + 1] == type))
				count++;
		}
		else if (j == WIDTH - 1)
		{
			if ((state[(i + 1)*HEIGHT + j - 1])&&(species[(i + 1)*HEIGHT + j - 1] == type))
				count++;
			if ((state[(i + 1)*HEIGHT + j - 1])&&(species[(i + 1)*HEIGHT + j - 1] == type))
				count++;
			if ((state[(i + 1)*HEIGHT + j])&&(species[(i + 1)*HEIGHT + j] == type))
				count++;
		}
		else
		{
			if ((state[i*HEIGHT + j - 1])&&(species[i*HEIGHT + j - 1] == type))
				count++;
			if ((state[i*HEIGHT + j + 1])&&(species[i*HEIGHT + j + 1] == type))
				count++;
			if ((state[(i + 1)*HEIGHT + j - 1])&&(species[(i + 1)*HEIGHT + j - 1] == type))
				count++;
			if ((state[(i + 1)*HEIGHT + j])&&(species[(i + 1)*HEIGHT + j] == type))
				count++;
			if ((state[(i + 1)*HEIGHT + j + 1])&&(species[(i + 1)*HEIGHT + j + 1]== type))
				count++;
		}
	}
	else if (i == HEIGHT - 1)
	{
		if (j == 0)
		{
			if ((state[(i - 1)*HEIGHT + j])&&(species[(i - 1)*HEIGHT + j]== type))
				count++;
			if ((state[(i - 1)*HEIGHT + j + 1])&&(species[(i - 1)*HEIGHT + j + 1]== type))
				count++;
			if ((state[i*HEIGHT + j + 1])&&(species[i*HEIGHT + j + 1] == type))
				count++;
		}
		else if (j == WIDTH - 1)
		{
			if ((state[(i - 1)*HEIGHT + j + 1])&&(species[(i - 1)*HEIGHT + j + 1] == type))
				count++;
			if ((state[(i - 1)*HEIGHT + j])&&(species[(i - 1)*HEIGHT + j] == type))
				count++;
			if ((state[i*HEIGHT + j - 1])&&(species[i*HEIGHT + j - 1] == type))
				count++;
		}
		else
		{
			if ((state[(i - 1)*HEIGHT + j + 1])&&(species[(i - 1)*HEIGHT + j + 1] == type))
				count++;
			if ((state[(i - 1)*HEIGHT + j])&&(species[(i - 1)*HEIGHT + j] == type))
				count++;
			if ((state[(i - 1)*HEIGHT + j + 1])&&(species[(i - 1)*HEIGHT + j + 1] == type))
				count++;
			if ((state[i*HEIGHT + j - 1])&&(species[i*HEIGHT + j - 1] == type))
				count++;
			if ((state[i*HEIGHT + j + 1])&&(species[i*HEIGHT + j + 1] == type))
				count++;
		}
	}
	else if (j == 0)
	{
		if ((state[(i - 1)*HEIGHT + j])&&(species[(i - 1)*HEIGHT + j] == type))
			count++;
		if ((state[(i - 1)*HEIGHT + j + 1])&&(species[(i - 1)*HEIGHT + j + 1] == type))
			count++;
		if ((state[i*HEIGHT + j + 1])&&(species[i*HEIGHT + j + 1] == type))
			count++;
		if ((state[(i + 1)*HEIGHT + j])&&(species[(i + 1)*HEIGHT + j] == type))
			count++;
		if ((state[(i + 1)*HEIGHT + j + 1])&&(species[(i + 1)*HEIGHT + j + 1] == type))
			count++;
	}
	else if (j == WIDTH - 1)
	{
		if ((state[(i - 1)*HEIGHT + j + 1])&&(species[(i - 1)*HEIGHT + j + 1] == type))
			count++;
		if ((state[(i - 1)*HEIGHT + j])&&(species[(i - 1)*HEIGHT + j] == type))
			count++;
		if ((state[i*HEIGHT + j - 1])&&(species[i*HEIGHT + j - 1] == type))
			count++;
		if ((state[(i + 1)*HEIGHT + j - 1])&&(species[(i + 1)*HEIGHT + j - 1] == type))
			count++;
		if ((state[(i + 1)*HEIGHT + j])&&(species[(i + 1)*HEIGHT + j] == type))
			count++;
	}
	else
	{
		if ((state[(i - 1)*HEIGHT + j + 1])&&(species[(i - 1)*HEIGHT + j + 1] == type))
			count++;
		if ((state[(i - 1)*HEIGHT + j])&&(species[(i - 1)*HEIGHT + j] == type))
			count++;
		if ((state[(i - 1)*HEIGHT + j + 1])&&(species[(i - 1)*HEIGHT + j + 1] == type))
			count++;
		if ((state[i*HEIGHT + j - 1])&&(species[i*HEIGHT + j - 1] == type))
			count++;
		if ((state[i*HEIGHT + j + 1])&&(species[i*HEIGHT + j + 1] == type))
			count++;
		if ((state[(i + 1)*HEIGHT + j - 1])&&(species[(i + 1)*HEIGHT + j - 1] == type))
			count++;
		if ((state[(i + 1)*HEIGHT + j])&&(species[(i + 1)*HEIGHT + j] == type))
			count++;
		if ((state[(i + 1)*HEIGHT + j + 1])&&(species[(i + 1)*HEIGHT + j + 1] == type))
			count++;
	}

	return count;
}