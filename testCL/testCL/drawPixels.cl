__kernel void drawPixels(__global float* pixels, __global bool* species, __global int* numOfSpecies, __global float* color)
{
	int gid = get_global_id(0);
	int SIZE = 786432;
	float factor = 0.0;
	float red = 0.0;
	float blue = 0.0;
	float green = 0.0;

	for(int k = 0; k < *numOfSpecies; k++)
	{
		if (species[k*SIZE + gid])
		{
			// Increase the factor based on number of live species on current pixel
			factor++;
			red += color[3*k];
			green += color[3*k + 1];
			blue += color[3*k + 2];
		}
	}

	if(factor != 0)
	{
		pixels[11*gid + 8] = red/factor;
		pixels[11*gid + 9] = green/factor;
		pixels[11*gid + 10] = blue/factor;
	}
	else //black
	{
		pixels[11*gid + 8] = red;
		pixels[11*gid + 9] = green;
		pixels[11*gid + 10] = blue;
	}	
}