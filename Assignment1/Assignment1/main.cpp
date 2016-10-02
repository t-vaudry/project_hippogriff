#include <time.h>
#include <iostream>
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <stdlib.h>
#include <vector>
#include <thread>
#include "Color.h"

using namespace std;

#define WIDTH 1024
#define HEIGHT 768
#define SIZE 786432
#define X_SIZE 0.001953125 // 2/1024
#define Y_SIZE 0.002604166 // 2/768

int numOfSpecies;
bool** species;
Color color[10] = {};

void glutTimer(int value);
void display();
void draw();
void initializeColor();
void initializeGrid();
void checkConditions(int type);
int count(int i, int j, int type);

int main(int argc, char** argv)
{
	cout << "Enter number of species: " << endl;
	cin >> numOfSpecies;

	if (numOfSpecies < 5)
	{
		cout << "Number of species is less than 5. Default to 5." << endl;
		numOfSpecies = 5;
	}
	else if (numOfSpecies > 10)
	{
		cout << "Number of species is greater than 10. Default to 10." << endl;
		numOfSpecies = 10;
	}

	srand(time(NULL));
	initializeColor();
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
	}

	for (int i = 0; i < numOfSpecies; i++)
	{
		threads[i].join();
	}
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);
	GLfloat x;
	GLfloat y = 1.0;
	GLfloat red = 0.0;
	GLfloat blue = 0.0;
	GLfloat green = 0.0;
	float factor;

	for(int i=0; i<HEIGHT; i++)
	{
		x = -1.0;
		for(int j=0; j<WIDTH; j++)
		{
			factor = 0.0;
			red = 0.0;
			blue = 0.0;
			green = 0.0;
			glBegin(GL_POLYGON);

				//Choose color
				for (int k = 0; k < numOfSpecies; k++)
				{
					if (species[k][(i*WIDTH) + j])
					{
						factor++;
						red += color[k].red;
						blue += color[k].blue;
						green += color[k].green;
					}
				}
				if (factor != 0)
					glColor3f(red / factor, green / factor, blue / factor);
				else
					glColor3f(red, blue, green);
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

void initializeColor()
{
	color[0] = Color(1.0,0.0,0.0); //red
	color[1] = Color(0.0,1.0,0.0); //green
	color[2] = Color(0.0,0.0,1.0); //blue
	color[3] = Color(1.0,1.0,0.0); //red-green
	color[4] = Color(0.0,1.0,1.0); //green-blue
	color[5] = Color(1.0,0.0,1.0); //red-blue
	color[6] = Color(1.0,1.0,1.0); //white
	color[7] = Color(0.5,0.75,0.33); //other
	color[8] = Color(0.33,0.5,0.75); //other
	color[9] = Color(0.75,0.33,0.5); //other
}

void initializeGrid()
{
	species = new bool*[numOfSpecies];

	for (int i = 0; i < numOfSpecies; i++)
	{
		species[i] = new bool[SIZE];
	}

	for (int i = 0; i < numOfSpecies; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			species[i][j] = false;
		}
	}

	for (int i = 0; i < SIZE; i++)
	{
		int type = rand() % numOfSpecies;

		if (rand() % 2 == 1)
			species[type][i] = true; //alive
		//else
			//species[type][i] = false; //dead
	}
}

void checkConditions(int type)
{
	int numOfNeighbors = 0;
	bool* tmp = new bool[SIZE];

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			numOfNeighbors = count(i, j, type);
			
			if (numOfNeighbors < 2)
				tmp[(i*WIDTH) + j] = false;
			else if ((numOfNeighbors == 2)&&(!species[type][(i*WIDTH)+j]))
				tmp[(i*WIDTH) + j] = false;
			else if (numOfNeighbors == 3)
				tmp[(i*WIDTH) + j] = true;
			else if (numOfNeighbors > 3)
				tmp[(i*WIDTH) + j] = false;
		}
	}

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			species[type][(i*WIDTH) + j] = tmp[(i*WIDTH) + j];
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
			if (species[type][i*WIDTH + j + 1])
				count++;
			if (species[type][(i + 1)*WIDTH + j])
				count++;
			if (species[type][(i + 1)*WIDTH + j + 1])
				count++;
		}
		else if (j == WIDTH - 1)
		{
			if (species[type][(i + 1)*WIDTH + j - 1])
				count++;
			if (species[type][(i + 1)*WIDTH + j - 1])
				count++;
			if (species[type][(i + 1)*WIDTH + j])
				count++;
		}
		else
		{
			if (species[type][i*WIDTH + j - 1])
				count++;
			if (species[type][i*WIDTH + j + 1])
				count++;
			if (species[type][(i + 1)*WIDTH + j - 1])
				count++;
			if (species[type][(i + 1)*WIDTH + j])
				count++;
			if (species[type][(i + 1)*WIDTH + j + 1])
				count++;
		}
	}
	else if (i == WIDTH - 1)
	{
		if (j == 0)
		{
			if (species[type][(i - 1)*WIDTH + j])
				count++;
			if (species[type][(i - 1)*WIDTH + j + 1])
				count++;
			if (species[type][i*WIDTH + j + 1])
				count++;
		}
		else if (j == WIDTH - 1)
		{
			if (species[type][(i - 1)*WIDTH + j - 1])
				count++;
			if (species[type][(i - 1)*WIDTH + j])
				count++;
			if (species[type][i*WIDTH + j - 1])
				count++;
		}
		else
		{
			if (species[type][(i - 1)*WIDTH + j - 1])
				count++;
			if (species[type][(i - 1)*WIDTH + j])
				count++;
			if (species[type][(i - 1)*WIDTH + j + 1])
				count++;
			if (species[type][i*WIDTH + j - 1])
				count++;
			if (species[type][i*WIDTH + j + 1])
				count++;
		}
	}
	else if (j == 0)
	{
		if (species[type][(i - 1)*WIDTH + j])
			count++;
		if (species[type][(i - 1)*WIDTH + j + 1])
			count++;
		if (species[type][i*WIDTH + j + 1])
			count++;
		if (species[type][(i + 1)*WIDTH + j])
			count++;
		if (species[type][(i + 1)*WIDTH + j + 1])
			count++;
	}
	else if (j == WIDTH - 1)
	{
		if (species[type][(i - 1)*WIDTH + j - 1])
			count++;
		if (species[type][(i - 1)*WIDTH + j])
			count++;
		if (species[type][i*WIDTH + j - 1])
			count++;
		if (species[type][(i + 1)*WIDTH + j - 1])
			count++;
		if (species[type][(i + 1)*WIDTH + j])
			count++;
	}
	else
	{
		if (species[type][(i - 1)*WIDTH + j - 1]) //top-left
			count++;
		if (species[type][(i - 1)*WIDTH + j])
			count++;
		if (species[type][(i - 1)*WIDTH + j + 1])
			count++;
		if (species[type][i*WIDTH + j - 1])
			count++;
		if (species[type][i*WIDTH + j + 1])
			count++;
		if (species[type][(i + 1)*WIDTH + j - 1])
			count++;
		if (species[type][(i + 1)*WIDTH + j])
			count++;
		if (species[type][(i + 1)*WIDTH + j + 1])
			count++;
	}

	return count;
}