__kernel void bye_kernel(__global bool *a)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);

    // Variables used to draw each pixel and define color
    GLfloat x;
    GLfloat y = 1.0;

    for (int i = 0; i<HEIGHT; i++)
    {
        x = -1.0;
        for (int j = 0; j<WIDTH; j++)
        {
            glBegin(GL_POLYGON);

            if (a[i*WIDTH + j])
                glColor3f(0.0, 0.0, 0.0); //white
            else
                glColor3f(1.0, 1.0, 1.0);

            glVertex2f(x, y - Y_SIZE);
            glVertex2f(x, y);
            glVertex2f(x + X_SIZE, y);
            glVertex2f(x + X_SIZE, y - Y_SIZE);
            glEnd();

            x += X_SIZE;
        }
        y -= Y_SIZE;
    }
    glutSwapBuffers();
}