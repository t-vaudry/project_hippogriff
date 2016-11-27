//
// Book:      OpenCL(R) Programming Guide
// Authors:   Aaftab Munshi, Benedict Gaster, Timothy Mattson, James Fung, Dan Ginsburg
// ISBN-10:   0-321-74964-2
// ISBN-13:   978-0-321-74964-2
// Publisher: Addison-Wesley Professional
// URLs:      http://safari.informit.com/9780132488006/
//            http://www.openclprogrammingguide.com
//

// HelloWorld.cpp
//
//    This is a simple example that demonstrates basic OpenCL setup and
//    use.

#include <iostream>
#include <fstream>
#include <sstream>
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <stdlib.h>
#include <random>
#include <stdio.h>

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

///
//  Constants
//
#define WIDTH 1024
#define HEIGHT 768
#define SIZE 786432
#define X_SIZE 0.001953125 // 2/1024
#define Y_SIZE 0.002604166 // 2/768

cl_context context = 0;
cl_command_queue commandQueue = 0;
cl_program program[3] = { 0, 0, 0 };
cl_device_id gpu_device = 0;
cl_kernel kernel[3] = { 0, 0, 0 };
cl_mem memObjects[5] = { 0, 0, 0, 0, 0 };
cl_int errNum;
cl_event clEvent = 0;

int numOfSpecies;
float* color = new float[30];
bool* species;
bool* tmp;
float* pixels;

void glutTimer(int value);
void initializeColor();
void initializeGrid();
void initializePixels();
void keyboard(unsigned char key, int x, int y);
void display();
void draw();
void drawPixels();

using namespace std;

///
//  Create an OpenCL context on the first available platform using
//  either a GPU or CPU depending on what is available.
//
cl_context CreateContext()
{
	cl_int errNum;
	cl_uint numPlatforms;
	cl_platform_id firstPlatformId;
	cl_context context = NULL;

	// First, select an OpenCL platform to run on.  For this example, we
	// simply choose the first available platform.  Normally, you would
	// query for all available platforms and select the most appropriate one.
	errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
	if (errNum != CL_SUCCESS || numPlatforms <= 0)
	{
		cerr << "Failed to find any OpenCL platforms." << endl;
		return NULL;
	}

	// Next, create an OpenCL context on the platform.  Attempt to
	// create a GPU-based context, and if that fails, try to create
	// a CPU-based context.
	// get device count
	cl_uint deviceCount;
	errNum = clGetDeviceIDs(firstPlatformId, CL_DEVICE_TYPE_GPU, 0, NULL, &deviceCount);
	if (errNum != CL_SUCCESS)
	{
		cerr << "Failed to count devices." << endl;
		return NULL;
	}

	// get all devices
	cl_device_id* devices;
	devices = new cl_device_id[deviceCount];
	errNum = clGetDeviceIDs(firstPlatformId, CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);
	if (errNum != CL_SUCCESS)
	{
		cerr << "Failed to get devices." << endl;
		return NULL;
	}

	// create a single context for all devices
	context = clCreateContext(NULL, deviceCount, devices, NULL, NULL, &errNum);
	if (errNum != CL_SUCCESS)
	{
		cerr << "Failed to create an OpenCL GPU or CPU context." << endl;
		return NULL;
	}

	return context;
}

///
//  Create a command queue on the first device available on the
//  context
//
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device)
{
	cl_int errNum;
	cl_device_id *devices;
	cl_command_queue commandQueue = NULL;
	size_t deviceBufferSize = -1;

	// First get the size of the devices buffer
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
	if (errNum != CL_SUCCESS)
	{
		cerr << "Failed call to clGetContextInfo(...,GL_CONTEXT_DEVICES,...)";
		return NULL;
	}

	if (deviceBufferSize <= 0)
	{
		cerr << "No devices available.";
		return NULL;
	}

	// Allocate memory for the devices buffer
	devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
	if (errNum != CL_SUCCESS)
	{
		delete[] devices;
		cerr << "Failed to get device IDs";
		return NULL;
	}

	// In this example, we just choose the first available device.  In a
	// real program, you would likely use all available devices or choose
	// the highest performance device based on OpenCL device queries
	commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);
	if (commandQueue == NULL)
	{
		delete[] devices;
		cerr << "Failed to create commandQueue for device";
		return NULL;
	}

	*device = devices[0];
	delete[] devices;
	return commandQueue;
}

///
//  Create an OpenCL program from the kernel source file
//
cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName)
{
	cl_int errNum;
	cl_program program;

	ifstream kernelFile(fileName, ios::in);
	if (!kernelFile.is_open())
	{
		cerr << "Failed to open file for reading: " << fileName << endl;
		return NULL;
	}

	ostringstream oss;
	oss << kernelFile.rdbuf();

	string srcStdStr = oss.str();
	const char *srcStr = srcStdStr.c_str();
	program = clCreateProgramWithSource(context, 1,
		(const char**)&srcStr,
		NULL, NULL);
	if (program == NULL)
	{
		cerr << "Failed to create CL program from source." << endl;
		return NULL;
	}

	errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (errNum != CL_SUCCESS)
	{
		// Determine the reason for the error
		char buildLog[16384];
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
			sizeof(buildLog), buildLog, NULL);

		cerr << "Error in kernel: " << endl;
		cerr << buildLog;
		clReleaseProgram(program);
		return NULL;
	}

	return program;
}

///
//  Create memory objects used as the arguments to the kernel
//  The kernel takes three arguments: result (output), a (input),
//  and b (input)
//
bool CreateMemObjects(cl_context context, cl_mem memObjects[5])
{
	memObjects[0] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		sizeof(bool) * SIZE * numOfSpecies, species, NULL);
	memObjects[1] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		sizeof(bool) * SIZE * numOfSpecies, tmp, NULL);
	memObjects[2] = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * SIZE * 11, pixels, NULL);
	memObjects[3] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(int), &numOfSpecies, NULL);
	memObjects[4] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * 30, color, NULL);

	if (memObjects[0] == NULL || memObjects[1] == NULL || memObjects[2] == NULL || memObjects[3] == NULL || memObjects[4] == NULL)
	{
		cerr << "Error creating memory objects." << endl;
		return false;
	}

	return true;
}

///
//  Cleanup any created OpenCL resources
//
void Cleanup(cl_context context, cl_command_queue commandQueue,
	cl_program program[2], cl_kernel kernel[2], cl_mem memObjects[5])
{
	for (int i = 0; i < 5; i++)
	{
		if (memObjects[i] != 0)
			clReleaseMemObject(memObjects[i]);
	}
	for (int i = 0; i < 3; i++)
	{
		if (kernel != 0)
			clReleaseKernel(kernel[i]);

		if (program != 0)
			clReleaseProgram(program[i]);
	}

	if (commandQueue != 0)
		clReleaseCommandQueue(commandQueue);

	if (context != 0)
		clReleaseContext(context);

	//system("pause");
}

///
//	main() for HelloWorld example
//
int main(int argc, char** argv)
{
	cout << "Enter number of species: " << endl;
	cin >> numOfSpecies;

	if (numOfSpecies < 1)
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
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Game of Life - Multiple Species");

	// Create an OpenCL GPU context on first available platform
	context = CreateContext();
	if (context == NULL)
	{
		cerr << "Failed to create OpenCL context." << endl;
		return 1;
	}

	// Create a command-queue on the first device available
	// on the created context
	commandQueue = CreateCommandQueue(context, &gpu_device);
	if (commandQueue == NULL)
	{
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return 1;
	}

	// Create OpenCL program from HelloWorld.cl kernel source
	program[0] = CreateProgram(context, gpu_device, "countNeighbors.cl");
	if (program[0] == NULL)
	{
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return 1;
	}

	// Create OpenCL program from HelloWorld.cl kernel source
	program[1] = CreateProgram(context, gpu_device, "setState.cl");
	if (program[1] == NULL)
	{
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return 1;
	}

	// Create OpenCL program from HelloWorld.cl kernel source
	program[2] = CreateProgram(context, gpu_device, "drawPixels.cl");
	if (program[2] == NULL)
	{
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return 1;
	}

	// Create OpenCL kernel
	kernel[0] = clCreateKernel(program[0], "countNeighbors", NULL);
	if (kernel[0] == NULL)
	{
		cerr << "Failed to create kernel 1" << endl;
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return 1;
	}

	// Create OpenCL kernel
	kernel[1] = clCreateKernel(program[1], "setState", NULL);
	if (kernel[1] == NULL)
	{
		cerr << "Failed to create kernel 2" << endl;
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return 1;
	}

	// Create OpenCL kernel
	kernel[2] = clCreateKernel(program[2], "drawPixels", NULL);
	if (kernel[2] == NULL)
	{
		cerr << "Failed to create kernel 3" << endl;
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return 1;
	}

	// Create memory objects that will be used as arguments to
	// kernel.  First create host memory arrays that will be
	// used to store the arguments to the kernel
	initializeColor();
	initializeGrid();
	initializePixels();

	if (!CreateMemObjects(context, memObjects))
	{
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return 1;
	}

	// Set the kernel arguments (result, a, b)
	errNum = clSetKernelArg(kernel[0], 0, sizeof(cl_mem), &memObjects[0]);
	errNum |= clSetKernelArg(kernel[0], 1, sizeof(cl_mem), &memObjects[1]);
	if (errNum != CL_SUCCESS)
	{
		cerr << "Error setting kernel arguments 1." << endl;
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return 1;
	}

	errNum = clSetKernelArg(kernel[1], 0, sizeof(cl_mem), &memObjects[0]);
	errNum |= clSetKernelArg(kernel[1], 1, sizeof(cl_mem), &memObjects[1]);
	if (errNum != CL_SUCCESS)
	{
		cerr << "Error setting kernel arguments 2." << endl;
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return 1;
	}

	errNum = clSetKernelArg(kernel[2], 0, sizeof(cl_mem), &memObjects[2]);
	errNum |= clSetKernelArg(kernel[2], 1, sizeof(cl_mem), &memObjects[0]);
	errNum |= clSetKernelArg(kernel[2], 2, sizeof(cl_mem), &memObjects[3]);
	errNum |= clSetKernelArg(kernel[2], 3, sizeof(cl_mem), &memObjects[4]);
	if (errNum != CL_SUCCESS)
	{
		cerr << "Error setting kernel arguments 3." << endl;
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return 1;
	}

	/*size_t wg_size;
	errNum = clGetKernelWorkGroupInfo(kernel[0], gpu_device, CL_KERNEL_WORK_GROUP_SIZE,
		sizeof(wg_size), &wg_size, NULL);

	size_t multiple;
	errNum = clGetKernelWorkGroupInfo(kernel[0], gpu_device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(multiple), &multiple, NULL);

	size_t device_size;
	errNum = clGetDeviceInfo(gpu_device, CL_DEVICE_ADDRESS_BITS, sizeof(size_t), &device_size, NULL);

	size_t max_work_group_size;
	errNum = clGetDeviceInfo(gpu_device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t), &max_work_group_size, NULL);*/

	// Set timer to recall every 33ms for 30FPS
	glutTimerFunc(1, glutTimer, 1);

	glutKeyboardFunc(keyboard);

	// Set display function that will be called
	glutDisplayFunc(display);

	// Call OpenGL main loop
	glutMainLoop();

	return 0;
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case(27):
		Cleanup(context, commandQueue, program, kernel, memObjects);
		exit(0);
	}
}

void glutTimer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(1, glutTimer, 1);
}

void display()
{
	size_t globalWorkSize[1] = { SIZE*numOfSpecies };
	size_t globalSize[1] = { SIZE };
	size_t localWorkSize[1] = { 8192 };
	int size = SIZE*numOfSpecies;

	errNum = clEnqueueNDRangeKernel(commandQueue, kernel[2], 1, NULL,
		globalSize, NULL,
		0, NULL, NULL);
	if (errNum != CL_SUCCESS)
	{
		cerr << "Error queuing kernel for execution 1." << endl;
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return;
	}

	// Read the output buffer back to the Host
	errNum = clEnqueueReadBuffer(commandQueue, memObjects[2], CL_TRUE,
		0, sizeof(float) * SIZE * 11, (void*)pixels,
		0, NULL, NULL);
	if (errNum != CL_SUCCESS)
	{
		cerr << "Error reading result buffer." << endl;
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return;
	}

	//drawPixels();
	draw();
	glutSwapBuffers();

	errNum = clEnqueueNDRangeKernel(commandQueue, kernel[0], 1, NULL,
		globalWorkSize, NULL,
		0, NULL, &clEvent);
	if (errNum != CL_SUCCESS)
	{
		cerr << "Error queuing kernel for execution 1." << endl;
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return;
	}

	errNum = clEnqueueNDRangeKernel(commandQueue, kernel[1], 1, NULL,
		globalWorkSize, NULL,
		1, &clEvent, NULL);
	if (errNum != CL_SUCCESS)
	{
		cerr << "Error queuing kernel for execution." << endl;
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return;
	}

	// Read the output buffer back to the Host
	errNum = clEnqueueReadBuffer(commandQueue, memObjects[0], CL_TRUE,
		0, sizeof(bool) * size, (void*)species,
		0, NULL, NULL);
	if (errNum != CL_SUCCESS)
	{
		cerr << "Error reading result buffer." << endl;
		Cleanup(context, commandQueue, program, kernel, memObjects);
		return;
	}
}

void initializeColor()
{
	color[0] = 1.0, color[1] = 0.0, color[2] = 0.0;
	color[3] = 0.0, color[4] = 1.0, color[5] = 0.0;
	color[6] = 0.0, color[7] = 0.0, color[8] = 1.0;
	color[9] = 1.0, color[10] = 1.0, color[11] = 0.0;
	color[12] = 0.0, color[13] = 1.0, color[14] = 1.0;
	color[15] = 1.0, color[16] = 0.0, color[17] = 1.0;
	color[18] = 1.0, color[19] = 1.0, color[20] = 1.0;
	color[21] = 0.5, color[22] = 0.75, color[23] = 0.33;
	color[24] = 0.33, color[25] = 0.5, color[26] = 0.75;
	color[27] = 0.75, color[25] = 0.33, color[29] = 0.5;
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

void initializePixels()
{
	pixels = new float[SIZE * 11];
	float x;
	float y = 1.0;
	for (int i = 0; i<HEIGHT; i++)
	{
		x = -1.0;
		for (int j = 0; j<WIDTH; j++)
		{
			pixels[11*(i*WIDTH + j)] = x;				//V1 x
			pixels[11*(i*WIDTH + j) + 1] = y - Y_SIZE;	//V1 y
			pixels[11*(i*WIDTH + j) + 2] = x;			//V2 x
			pixels[11*(i*WIDTH + j) + 3] = y;			//V2 y
			pixels[11*(i*WIDTH + j) + 4] = x + X_SIZE;	//V3 x
			pixels[11*(i*WIDTH + j) + 5] = y;			//V3 y
			pixels[11*(i*WIDTH + j) + 6] = x + X_SIZE;	//V4 x
			pixels[11*(i*WIDTH + j) + 7] = y - Y_SIZE;	//V4 y
			pixels[11 * (i*WIDTH + j) + 8] = 0.0;		//red
			pixels[11 * (i*WIDTH + j) + 9] = 0.0;		//green
			pixels[11 * (i*WIDTH + j) + 10] = 0.0;		//blue

			x += X_SIZE;
		}
		y -= Y_SIZE;
	}
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);

	for (int i = 0; i<HEIGHT; i++)
	{
		for (int j = 0; j<WIDTH; j++)
		{
			glBegin(GL_POLYGON);
			glColor3f(pixels[11 * ((i*WIDTH) + j) + 8], pixels[11 * ((i*WIDTH) + j) + 9], pixels[11 * ((i*WIDTH) + j) + 10]);
			glVertex2f(pixels[11*((i*WIDTH) + j)], pixels[11*((i*WIDTH) + j) + 1]);
			glVertex2f(pixels[11*((i*WIDTH) + j) + 2], pixels[11*((i*WIDTH) + j) + 3]);
			glVertex2f(pixels[11*((i*WIDTH) + j) + 4], pixels[11*((i*WIDTH) + j) + 5]);
			glVertex2f(pixels[11*((i*WIDTH) + j) + 6], pixels[11*((i*WIDTH) + j) + 7]);
			glEnd();
		}
	}
}

void drawPixels()
{
	float factor = 0.0;
	float red = 0.0;
	float blue = 0.0;
	float green = 0.0;

	for (int i = 0; i < SIZE; i++)
	{
		factor = 0.0;
		red = 0.0;
		blue = 0.0;
		green = 0.0;

		for (int k = 0; k < numOfSpecies; k++)
		{
			if (species[k*SIZE + i])
			{
				// Increase the factor based on number of live species on current pixel
				factor++;
				red += color[3*k + 0];
				green += color[3*k + 1];
				blue += color[3*k + 2];
			}
		}

		if (factor != 0)
		{
			pixels[11 * i + 8] = red / factor;
			pixels[11 * i + 9] = green / factor;
			pixels[11 * i + 10] = blue / factor;
		}
		else //black
		{
			pixels[11 * i + 8] = red;
			pixels[11 * i + 9] = green;
			pixels[11 * i + 10] = blue;
		}
	}
}




