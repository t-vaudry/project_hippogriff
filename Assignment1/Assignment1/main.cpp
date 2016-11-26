#include <time.h>
#include <iostream>
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <stdlib.h>
#include <thread>
#include <random>

using namespace std;

#define WIDTH 1024
#define HEIGHT 768
#define SIZE 786432
#define X_SIZE 0.001953125 // 2/1024
#define Y_SIZE 0.002604166 // 2/768

int numOfSpecies;
float** color = new float*[10];
thread* compThreads;
bool* species;

void glutTimer(int value);
void initializeColor();
void initializeGrid();
void display();
void draw();
void checkConditions(int type);
int countNeighbors(int i, int type);

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

	// Initialize the color schemes and initial grid
	initializeColor();
	initializeGrid();

	// Initialize threads for computation
	compThreads = new thread[numOfSpecies];

	// Initialize OpenGL
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Game of Life - Multiple Species");

	// Set timer to recall every 33ms for 30FPS
	glutTimerFunc(33, glutTimer, 1);

	// Set display function that will be called
	glutDisplayFunc(display);

	// Call OpenGL main loop
	glutMainLoop();
	return 0;
}

void glutTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(33, glutTimer, 1);
}

void initializeColor()
{
	for (int i = 0; i < 10; i++)
	{
		color[i] = new float[3];
	}

	color[0][0] = 1.0, color[0][1] = 0.0, color[0][2] = 0.0;
	color[1][0] = 0.0, color[1][1] = 1.0, color[1][2] = 0.0;
	color[2][0] = 0.0, color[2][1] = 0.0, color[2][2] = 1.0;
	color[3][0] = 1.0, color[3][1] = 1.0, color[3][2] = 0.0;
	color[4][0] = 0.0, color[4][1] = 1.0, color[4][2] = 1.0;
	color[5][0] = 1.0, color[5][1] = 0.0, color[5][2] = 1.0;
	color[6][0] = 1.0, color[6][1] = 1.0, color[6][2] = 1.0;
	color[7][0] = 0.5, color[7][1] = 0.75, color[7][2] = 0.33;
	color[8][0] = 0.33, color[8][1] = 0.5, color[8][2] = 0.75;
	color[9][0] = 0.75, color[9][1] = 0.33, color[9][2] = 0.5;
}

void initializeGrid()
{
	species = new bool[numOfSpecies*SIZE];

	for (int i = 0; i < numOfSpecies*SIZE; i++)
	{
		species[i] = false;
	}

	// Random number generation
	default_random_engine generator;
	uniform_int_distribution<int> distribution(1, numOfSpecies);
	uniform_int_distribution<int> state_distribution(0, 1);

	for (int i = 0; i < SIZE; i++)
	{
		int type = distribution(generator); // Species

		if (state_distribution(generator) == 1)
			species[i + SIZE*(type-1)] = true; // Alive
										 //else dead
	}
}

void display()
{
	// Call draw function to display grid
	draw();
	glutSwapBuffers();

	// Call threads for each species to check conditions
	for (int i = 0; i < numOfSpecies; i++)
	{
		compThreads[i] = thread(checkConditions, i);
	}

	for (int i = 0; i < numOfSpecies; i++)
	{
		compThreads[i].join();
	}
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);

	// Variables used to draw each pixel and define color
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
					if (species[k*SIZE + (i*WIDTH) + j])
					{
						// Increase the factor based on number of live species on current pixel
						factor++;
						red += color[k][0];
						green += color[k][1];
						blue += color[k][2];
					}
				}
				if (factor != 0)
					glColor3f(red / factor, green / factor, blue / factor);
				else
					glColor3f(red, blue, green); //black
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

void checkConditions(int type)
{
	int numOfNeighbors = 0;
	bool* tmp = new bool[SIZE]; // Used for next state

	for (int i = 0; i < SIZE; i++)
	{
		numOfNeighbors = countNeighbors(i, type); // Return number of live neighbors of species type
			
		// Rules of game of life for next state
		if (numOfNeighbors < 2) // Less than 2, underpopulated
			tmp[i] = false;
		else if ((numOfNeighbors == 2)&&(!species[i + SIZE*type])) // 2 neighbors, and currently dead, remain dead
			tmp[i] = false;
		else if (numOfNeighbors == 3) // 3 neighbors, revive/remain alive
			tmp[i] = true;
		else if (numOfNeighbors > 3) // More than 3, overpopulated
			tmp[i] = false;
	}

	for (int i = 0; i < SIZE; i++)
	{
		species[i + type*SIZE] = tmp[i]; // Set next state
	}

	delete tmp;
}

int countNeighbors(int i, int type)
{
	int count = 0;

	if (i >= WIDTH)
	{
		if (i%WIDTH != 0)
		{
			if (species[type*SIZE + i - WIDTH - 1]) // Top-Left Corner
				count++;
		}
		if (species[type*SIZE + i - WIDTH]) // Top-Center Edge
			count++;
		if (i%WIDTH != WIDTH-1)
		{
			if (species[type*SIZE + i - WIDTH + 1]) // Top-Right Corner
				count++;
		}
	}

	if (i < WIDTH*(HEIGHT - 1))
	{
		if (i%WIDTH != 0)
		{
			if (species[type*SIZE + i + WIDTH - 1]) // Bottom-Left Corner
				count++;
		}
		if (species[type*SIZE + i + WIDTH]) // Bottom-Center Edge
			count++;
		if (i%WIDTH != WIDTH-1)
		{
			if (species[type*SIZE + i + WIDTH + 1]) // Bottom-Right Corner
				count++;
		}
	}

	if (i%WIDTH != 0)
	{
		if (species[type*SIZE + i - 1]) // Middle-Left Edge
			count++;
	}

	if (i%WIDTH != WIDTH-1)
	{
		if (species[type*SIZE + i + 1]) // Middle-Right Edge
			count++;
	}

	return count;
}