__kernel void drawPixels(__global float* colors, __global bool* species, __global int* numOfSpecies, __global float* color)
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
		colors[3*gid] = red/factor;
		colors[3*gid + 1] = green/factor;
		colors[3*gid + 2] = blue/factor;
	}
	else //black
	{
		colors[3*gid] = red;
		colors[3*gid + 1] = green;
		colors[3*gid + 2] = blue;
	}
}