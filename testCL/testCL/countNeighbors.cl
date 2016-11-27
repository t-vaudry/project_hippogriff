__kernel void countNeighbors(__global bool *dev_s, __global bool *temp)
{
    int WIDTH = 1024;
    int HEIGHT = 768;
    int SIZE = 786432;
    int i = get_global_id(0);
    int type = i/(SIZE);

    int numOfNeighbors = 0;

    if (i - type*SIZE >= WIDTH)
    {
        if (i%WIDTH != 0)
        {
            if (dev_s[i - WIDTH - 1]) // Top-Left Corner
                numOfNeighbors++;
        }
        if (dev_s[i - WIDTH]) // Top-Center Edge
            numOfNeighbors++;
        if (i%WIDTH != WIDTH - 1)
        {
            if (dev_s[i - WIDTH + 1]) // Top-Right Corner
                numOfNeighbors++;
        }
    }

    if (i - type*SIZE < WIDTH*(HEIGHT - 1))
    {
        if (i%WIDTH != 0)
        {
            if (dev_s[i + WIDTH - 1]) // Bottom-Left Corner
                numOfNeighbors++;
        }
        if (dev_s[i + WIDTH]) // Bottom-Center Edge
            numOfNeighbors++;
        if (i%WIDTH != WIDTH - 1)
        {
            if (dev_s[i + WIDTH + 1]) // Bottom-Right Corner
                numOfNeighbors++;
        }
    }

    if (i%WIDTH != 0)
    {
        if (dev_s[i - 1]) // Middle-Left Edge
            numOfNeighbors++;
    }

    if (i%WIDTH != WIDTH - 1)
    {
        if (dev_s[i + 1]) // Middle-Right Edge
            numOfNeighbors++;
    }

    // Rules of game of life for next state
    if (numOfNeighbors < 2) // Less than 2, underpopulated
        temp[i] = false;
    else if ((numOfNeighbors == 2) && (!dev_s[i])) // 2 neighbors, and currently dead, remain dead
        temp[i] = false;
    else if (numOfNeighbors == 3) // 3 neighbors, revive/remain alive
        temp[i] = true;
    else if (numOfNeighbors > 3) // More than 3, overpopulated
        temp[i]= false;
}
