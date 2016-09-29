void initializeGrid()
{
	species = new int*[HEIGHT];
	state = new bool*[HEIGHT];

	for(int i = 0; i < WIDTH; i++)
	{
		species[i] = new int[WIDTH];
		state[i] = new bool[WIDTH];
	}

	for(int i = 0; i < HEIGHT; i++)
	{
		for(int j = 0; j < WIDTH; j++)
		{
			species[i][j] = rand() % numOfSpecies;

			if(rand()%2 == 1)
				state[i][j] = true; //alive
			else
				state[i][j] = false; //dead
		}
	}
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);
	GLfloat x;
	GLfloat y = 1.002604166;

	for(int i=0; i<768; i++){
		x = -1.0;
		for(int j=0; j<1024; j++){
		glBegin(GL_POLYGON);
			if (state[i][j])
			{
				if(species[i][j] == 0)
					glColor3f(1.0f, 0.0f, 0.0f); //red
				else if(species[i][j] == 1)
					glColor3f(0.0f, 0.0f, 1.0f); //blue
				else if (species[i][j] == 2)
					glColor3f(0.0f, 1.0f, 0.0f); //green
				else if (species[i][j] == 3)
					glColor3f(1.0f, 1.0f, 0.0f); //red-green
				else if (species[i][j] == 4)
					glColor3f(0.0f, 1.0f, 1.0f); //blue-green
				else if (species[i][j] == 5)
					glColor3f(1.0f, 0.0f, 1.0f); //red-blue
				else if (species[i][j] == 6)
					glColor3f(1.0f, 1.0f, 1.0f); //white
				else if (species[i][j] == 7)
					glColor3f(0.0f, 0.5f, 0.0f); //half-green
				else if (species[i][j] == 8)
					glColor3f(0.5f, 0.0f, 0.0f); //half-red
				else if (species[i][j] == 9)
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

void checkConditions(int type)
{
	for(int i = 0; i < 768; i++)
	{
		for(int j=0; j < 1024; j++)
		{
			if (species[i][j] == type)
			{
				numNeighbors[i][j] = count(i, j, type);
			}
	}

	for (int i = 0; i < 768; i++)
	{
		for (int j = 0; j < 1024; j++)
		{
			if (species[i][j] == type)
			{
				if (numNeighbors[i][j] < 2)
					state[i][j] = false;
				else if (numNeighbors[i][j] == 3)
					state[i][j] = true;
				else if (numNeighbors[i][j] > 3)
					state[i][j] = false;
			}
		}
	}
}

int count(int i, int j, int type)
{
	int count = 0;

	if (i == 0)
	{
		if (j == 0) // top-left corner
		{
			if((state[i][j+1])&&(species[i][j+1]==type)) //middle-right
				count++;
			if((state[i+1][j])&&(species[i+1][j]==type)) //bottom-center
				count++;
			if((state[i+1][j+1])&&(species[i+1][j+1]==type)) //bottom-right
				count++;
		}
		else if (j == WIDTH - 1) // top-right corner
		{
			if((state[i][j-1])&&(species[i][j-1]==type)) //middle-left
				count++
			if((state[i+1][j-1])&&(species[i+1][j-1]==type)) //bottom-left
				count++;
			if((state[i+1][j])&&(species[i+1][j]==type)) //bottom-center
				count++;
		}
		else //top edge
		{
			if((state[i][j-1])&&(species[i][j-1]==type)) //middle-left
				count++
			if((state[i][j+1])&&(species[i][j+1]==type)) //middle-right
				count++;
			if((state[i+1][j-1])&&(species[i+1][j-1]==type)) //bottom-left
				count++;
			if((state[i+1][j])&&(species[i+1][j]==type)) //bottom-center
				count++;
			if((state[i+1][j+1])&&(species[i+1][j+1]==type)) //bottom-right
				count++;
		}
	}
	else if (i == WIDTH - 1)
	{
		if (j == 0) // bottom-left corner
		{
			if((state[i-1][j])&&(species[i-1][j]==type)) //top-center
				count++
			if((state[i-1][j+1])&&(species[i-1][j+1]==type)) //top-right
				count++;
			if((state[i][j+1])&&(species[i][j+1]==type)) //middle-right
				count++;
		}
		else if (j == WIDTH - 1) // bottom-right corner
		{
			if((state[i-1][j+1])&&(species[i-1][j+1]==type)) //top-left
				count++;
			if((state[i-1][j])&&(species[i-1][j]==type)) //top-center
				count++
			if((state[i][j-1])&&(species[i][j-1]==type)) //middle-left
				count++
		}
		else // bottom edge
		{
			if((state[i-1][j+1])&&(species[i-1][j+1]==type)) //top-left
				count++;
			if((state[i-1][j])&&(species[i-1][j]==type)) //top-center
				count++
			if((state[i-1][j+1])&&(species[i-1][j+1]==type)) //top-right
				count++;
			if((state[i][j-1])&&(species[i][j-1]==type)) //middle-left
				count++
			if((state[i][j+1])&&(species[i][j+1]==type)) //middle-right
				count++;
		}
	}
	else if (j == 0) // left edge
	{
		if((state[i-1][j])&&(species[i-1][j]==type)) //top-center
			count++
		if((state[i-1][j+1])&&(species[i-1][j+1]==type)) //top-right
			count++;
		if((state[i][j+1])&&(species[i][j+1]==type)) //middle-right
			count++;
		if((state[i+1][j])&&(species[i+1][j]==type)) //bottom-center
			count++;
		if((state[i+1][j+1])&&(species[i+1][j+1]==type)) //bottom-right
			count++;
	}
	else if (j == WIDTH - 1) // right edge
	{
		if((state[i-1][j+1])&&(species[i-1][j+1]==type)) //top-left
			count++;
		if((state[i-1][j])&&(species[i-1][j]==type)) //top-center
			count++
		if((state[i][j-1])&&(species[i][j-1]==type)) //middle-left
			count++
		if((state[i+1][j-1])&&(species[i+1][j-1]==type)) //bottom-left
			count++;
		if((state[i+1][j])&&(species[i+1][j]==type)) //bottom-center
			count++;
	}
	else // middle
	{
		if((state[i-1][j+1])&&(species[i-1][j+1]==type)) //top-left
			count++;
		if((state[i-1][j])&&(species[i-1][j]==type)) //top-center
			count++
		if((state[i-1][j+1])&&(species[i-1][j+1]==type)) //top-right
			count++;
		if((state[i][j-1])&&(species[i][j-1]==type)) //middle-left
			count++
		if((state[i][j+1])&&(species[i][j+1]==type)) //middle-right
			count++;
		if((state[i+1][j-1])&&(species[i+1][j-1]==type)) //bottom-left
			count++;
		if((state[i+1][j])&&(species[i+1][j]==type)) //bottom-center
			count++;
		if((state[i+1][j+1])&&(species[i+1][j+1]==type)) //bottom-right
			count++;
	}
	return count;
}