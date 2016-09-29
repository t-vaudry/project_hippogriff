#include <iostream>
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <stdlib.h>
#include <vector>
#include <thread>

using namespace std;

#define WIDTH 1024
#define HEIGHT 768
#define X_SIZE 0.001953125 // 2/1024
#define Y_SIZE 0.002604166 // 2/768

int numOfSpecies;
int* numNeighbors;
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

	srand(time(NULL));
	initializeGrid();
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

	draw();
	glutSwapBuffers();

	for (int i = 0; i < numOfSpecies; i++)
	{
		threads.push_back(thread(checkConditions, i));
		threads[i].join();
	}

	for (int i = 0; i < 768; i++)
	{
		for (int j = 0; j < 1024; j++)
		{
			if (numNeighbors[i*WIDTH + j] < 2)
				state[i*WIDTH + j] = false;
			else if (numNeighbors[i*WIDTH + j] == 3)
				state[i*WIDTH + j] = true;
			else if (numNeighbors[i*WIDTH + j] > 3)
				state[i*WIDTH + j] = false;
		}
	}
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);
	GLfloat x;
	GLfloat y = 1.0;

	for(int i=0; i<768; i++){
		x = -1.0;
		for(int j=0; j<1024; j++){
		glBegin(GL_POLYGON);
			if (state[i*WIDTH + j])
			{
				if(species[i*WIDTH + j] == 0)
					glColor3f(1.0f, 0.0f, 0.0f); //red
				else if(species[i*WIDTH + j] == 1)
					glColor3f(0.0f, 0.0f, 1.0f); //blue
				else if (species[i*WIDTH + j] == 2)
					glColor3f(0.0f, 1.0f, 0.0f); //green
				else if (species[i*WIDTH + j] == 3)
					glColor3f(1.0f, 1.0f, 0.0f); //red-green
				else if (species[i*WIDTH + j] == 4)
					glColor3f(0.0f, 1.0f, 1.0f); //blue-green
				else if (species[i*WIDTH + j] == 5)
					glColor3f(1.0f, 0.0f, 1.0f); //red-blue
				else if (species[i*WIDTH + j] == 6)
					glColor3f(1.0f, 1.0f, 1.0f); //white
				else if (species[i*WIDTH + j] == 7)
					glColor3f(0.0f, 0.5f, 0.0f); //half-green
				else if (species[i*WIDTH + j] == 8)
					glColor3f(0.5f, 0.0f, 0.0f); //half-red
				else if (species[i*WIDTH + j] == 9)
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
	y -= Y_SIZE;
	}
}

void initializeGrid()
{
	species = new int[786432];
	state = new bool[786432];
	numNeighbors = new int[786432];

	for (int i = 0; i<786432; i++)
	{
		species[i] = rand() % numOfSpecies;

		if (rand() % 2 == 1)
			state[i] = true; //alive
		else
			state[i] = false; //dead
	}
}

void checkConditions(int type)
{
	int i = -1;
	int j = 0;

	for (int i = 0; i < 768; i++)
	{
		for (int j = 0; j < 1024; j++)
		{
			if (species[i*WIDTH + j] == type)
			{
				numNeighbors[i*WIDTH + j] = count(i, j, type);
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
			if ((state[i*WIDTH + j + 1])&&(species[i*WIDTH + j + 1] == type))
				count++;
			if ((state[(i + 1)*WIDTH + j])&&(species[(i + 1)*WIDTH + j] == type))
				count++;
			if ((state[(i + 1)*WIDTH + j + 1])&&(species[(i + 1)*WIDTH + j + 1] == type))
				count++;
		}
		else if (j == WIDTH - 1)
		{
			if ((state[(i + 1)*WIDTH + j - 1])&&(species[(i + 1)*WIDTH + j - 1] == type))
				count++;
			if ((state[(i + 1)*WIDTH + j - 1])&&(species[(i + 1)*WIDTH + j - 1] == type))
				count++;
			if ((state[(i + 1)*WIDTH + j])&&(species[(i + 1)*WIDTH + j] == type))
				count++;
		}
		else
		{
			if ((state[i*WIDTH + j - 1])&&(species[i*WIDTH + j - 1] == type))
				count++;
			if ((state[i*WIDTH + j + 1])&&(species[i*WIDTH + j + 1] == type))
				count++;
			if ((state[(i + 1)*WIDTH + j - 1])&&(species[(i + 1)*WIDTH + j - 1] == type))
				count++;
			if ((state[(i + 1)*WIDTH + j])&&(species[(i + 1)*WIDTH + j] == type))
				count++;
			if ((state[(i + 1)*WIDTH + j + 1])&&(species[(i + 1)*WIDTH + j + 1]== type))
				count++;
		}
	}
	else if (i == WIDTH - 1)
	{
		if (j == 0)
		{
			if ((state[(i - 1)*WIDTH + j])&&(species[(i - 1)*WIDTH + j]== type))
				count++;
			if ((state[(i - 1)*WIDTH + j + 1])&&(species[(i - 1)*WIDTH + j + 1]== type))
				count++;
			if ((state[i*WIDTH + j + 1])&&(species[i*WIDTH + j + 1] == type))
				count++;
		}
		else if (j == WIDTH - 1)
		{
			if ((state[(i - 1)*WIDTH + j - 1])&&(species[(i - 1)*WIDTH + j - 1] == type))
				count++;
			if ((state[(i - 1)*WIDTH + j])&&(species[(i - 1)*WIDTH + j] == type))
				count++;
			if ((state[i*WIDTH + j - 1])&&(species[i*WIDTH + j - 1] == type))
				count++;
		}
		else
		{
			if ((state[(i - 1)*WIDTH + j - 1])&&(species[(i - 1)*WIDTH + j - 1] == type))
				count++;
			if ((state[(i - 1)*WIDTH + j])&&(species[(i - 1)*WIDTH + j] == type))
				count++;
			if ((state[(i - 1)*WIDTH + j + 1])&&(species[(i - 1)*WIDTH + j + 1] == type))
				count++;
			if ((state[i*WIDTH + j - 1])&&(species[i*WIDTH + j - 1] == type))
				count++;
			if ((state[i*WIDTH + j + 1])&&(species[i*WIDTH + j + 1] == type))
				count++;
		}
	}
	else if (j == 0)
	{
		if ((state[(i - 1)*WIDTH + j])&&(species[(i - 1)*WIDTH + j] == type))
			count++;
		if ((state[(i - 1)*WIDTH + j + 1])&&(species[(i - 1)*WIDTH + j + 1] == type))
			count++;
		if ((state[i*WIDTH + j + 1])&&(species[i*WIDTH + j + 1] == type))
			count++;
		if ((state[(i + 1)*WIDTH + j])&&(species[(i + 1)*WIDTH + j] == type))
			count++;
		if ((state[(i + 1)*WIDTH + j + 1])&&(species[(i + 1)*WIDTH + j + 1] == type))
			count++;
	}
	else if (j == WIDTH - 1)
	{
		if ((state[(i - 1)*WIDTH + j - 1])&&(species[(i - 1)*WIDTH + j - 1] == type))
			count++;
		if ((state[(i - 1)*WIDTH + j])&&(species[(i - 1)*WIDTH + j] == type))
			count++;
		if ((state[i*WIDTH + j - 1])&&(species[i*WIDTH + j - 1] == type))
			count++;
		if ((state[(i + 1)*WIDTH + j - 1])&&(species[(i + 1)*WIDTH + j - 1] == type))
			count++;
		if ((state[(i + 1)*WIDTH + j])&&(species[(i + 1)*WIDTH + j] == type))
			count++;
	}
	else
	{
		if ((state[(i - 1)*WIDTH + j - 1])&&(species[(i - 1)*WIDTH + j - 1] == type)) //top-left
			count++;
		if ((state[(i - 1)*WIDTH + j])&&(species[(i - 1)*WIDTH + j] == type))
			count++;
		if ((state[(i - 1)*WIDTH + j + 1])&&(species[(i - 1)*WIDTH + j + 1] == type))
			count++;
		if ((state[i*WIDTH + j - 1])&&(species[i*WIDTH + j - 1] == type))
			count++;
		if ((state[i*WIDTH + j + 1])&&(species[i*WIDTH + j + 1] == type))
			count++;
		if ((state[(i + 1)*WIDTH + j - 1])&&(species[(i + 1)*WIDTH + j - 1] == type))
			count++;
		if ((state[(i + 1)*WIDTH + j])&&(species[(i + 1)*WIDTH + j] == type))
			count++;
		if ((state[(i + 1)*WIDTH + j + 1])&&(species[(i + 1)*WIDTH + j + 1] == type))
			count++;
	}

	return count;
}