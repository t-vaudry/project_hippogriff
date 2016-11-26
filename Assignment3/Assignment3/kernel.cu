#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <time.h>
#include <iostream>
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <stdlib.h>
#include <random>
#include <stdio.h>

using namespace std;

#define WIDTH 1024
#define HEIGHT 768
#define SIZE 786432
#define X_SIZE 0.001953125 // 2/1024
#define Y_SIZE 0.002604166 // 2/768

int numOfSpecies;
float** color = new float*[10];
bool* species;
bool* tmp;
bool* dev_species = 0;
bool* dev_tmp = 0;

void glutTimer(int value); 
void initializeColor();
void initializeGrid();
void keyboard(unsigned char key, int x, int y);
void display();
void draw();
void gameOfLife();

__global__ void countNeighbors(bool* dev_s, bool* temp, int type)
{
    int i = threadIdx.x + blockDim.x*blockIdx.x;
	int numOfNeighbors = 0;

	if (i >= WIDTH)
	{
		if (i%WIDTH != 0)
		{
			if (dev_s[type*SIZE + i - WIDTH - 1]) // Top-Left Corner
				numOfNeighbors++;
		}
		if (dev_s[type*SIZE + i - WIDTH]) // Top-Center Edge
			numOfNeighbors++;
		if (i%WIDTH != WIDTH - 1)
		{
			if (dev_s[type*SIZE + i - WIDTH + 1]) // Top-Right Corner
				numOfNeighbors++;
		}
	}

	if (i < WIDTH*(HEIGHT - 1))
	{
		if (i%WIDTH != 0)
		{
			if (dev_s[type*SIZE + i + WIDTH - 1]) // Bottom-Left Corner
				numOfNeighbors++;
		}
		if (dev_s[type*SIZE + i + WIDTH]) // Bottom-Center Edge
			numOfNeighbors++;
		if (i%WIDTH != WIDTH - 1)
		{
			if (dev_s[type*SIZE + i + WIDTH + 1]) // Bottom-Right Corner
				numOfNeighbors++;
		}
	}

	if (i%WIDTH != 0)
	{
		if (dev_s[type*SIZE + i - 1]) // Middle-Left Edge
			numOfNeighbors++;
	}

	if (i%WIDTH != WIDTH - 1)
	{
		if (dev_s[type*SIZE + i + 1]) // Middle-Right Edge
			numOfNeighbors++;
	}

	// Rules of game of life for next state
	if (numOfNeighbors < 2) // Less than 2, underpopulated
		temp[i + type*SIZE] = false;
	else if ((numOfNeighbors == 2) && (!dev_s[type*SIZE + i])) // 2 neighbors, and currently dead, remain dead
		temp[i + type*SIZE] = false;
	else if (numOfNeighbors == 3) // 3 neighbors, revive/remain alive
		temp[i + type*SIZE] = true;
	else if (numOfNeighbors > 3) // More than 3, overpopulated
		temp[i + type*SIZE] = false;
}

__global__ void setState(bool* dev_s, bool* temp)
{
	int i = threadIdx.x + blockDim.x*blockIdx.x;
	dev_s[i] = temp[i];
}

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

	glutInit(&argc, argv);
	// Initialize the color schemes and initial grid
	initializeColor();
	initializeGrid();
	gameOfLife();

    return 0;
}

// Function to simulate the Game of Life
void gameOfLife()
{
	cudaError_t cudaStatus;
	
	// Choose which GPU to run on, change this on a multi-GPU system.
	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		goto Error;
	}

	// Allocate GPU buffers for two vectors (one input, one output)
	cudaStatus = cudaMalloc((void**)&dev_species, SIZE * numOfSpecies * sizeof(bool));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	cudaStatus = cudaMalloc((void**)&dev_tmp, SIZE * numOfSpecies * sizeof(bool));
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		goto Error;
	}

	// Copy input vectors from host memory to GPU buffers.
	cudaStatus = cudaMemcpy(dev_species, species, SIZE * numOfSpecies * sizeof(bool), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	cudaStatus = cudaMemcpy(dev_tmp, tmp, SIZE * numOfSpecies * sizeof(bool), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
		goto Error;
	}

	// Initialize OpenGL
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Game of Life - Multiple Species");

	// Set timer to recall every 33ms for 30FPS
	glutTimerFunc(1, glutTimer, 1);

	glutKeyboardFunc(keyboard);

	// Set display function that will be called
	glutDisplayFunc(display);

	// Call OpenGL main loop
	glutMainLoop();

	// cudaDeviceReset must be called before exiting in order for profiling and
	// tracing tools such as Nsight and Visual Profiler to show complete traces.
	cudaStatus = cudaDeviceReset();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceReset failed!");
	}

Error:
	cudaFree(dev_species);
	cudaFree(dev_tmp);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case(27):
		cudaFree(dev_species);
		cudaFree(dev_tmp);
		exit(0);
	}
}

void glutTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(1, glutTimer, 1);
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
	tmp = new bool[SIZE*numOfSpecies];

	for (int i = 0; i < numOfSpecies*SIZE; i++)
	{
		species[i] = false;
		tmp[i] = false;
	}

	// Random number generation
	default_random_engine generator;
	uniform_int_distribution<int> distribution(1, numOfSpecies);
	uniform_int_distribution<int> state_distribution(0, 1);

	for (int i = 0; i < SIZE; i++)
	{
		int type = distribution(generator); // Species

		if (state_distribution(generator) == 1)
			species[(type - 1)*SIZE + i] = true; // Alive
										 //else dead
	}
}

void display()
{
	// Call draw function to display grid
	draw();
	glutSwapBuffers();

	//system("pause");

	cudaError_t cudaStatus;

	// Call threads for each species to check conditions
	for (int i = 0; i < numOfSpecies; i++)
	{
		countNeighbors<<<1024, 768>>>(dev_species, dev_tmp, i);

		// cudaDeviceSynchronize waits for the kernel to finish, and returns
		// any errors encountered during the launch.
		cudaStatus = cudaDeviceSynchronize();
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
		}
	}

	setState<<<2048*numOfSpecies, 384>>>(dev_species, dev_tmp);

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
	}

	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
	}
	
	cudaStatus = cudaMemcpy(species, dev_species, SIZE * numOfSpecies * sizeof(bool), cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMemcpy failed!");
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

	for (int i = 0; i<HEIGHT; i++)
	{
		x = -1.0;
		for (int j = 0; j<WIDTH; j++)
		{
			factor = 0.0;
			red = 0.0;
			blue = 0.0;
			green = 0.0;

			glBegin(GL_POLYGON);

			//Choose color
			for (int k = 0; k < numOfSpecies; k++)
			{
				if (species[k*SIZE + i*WIDTH + j])
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
