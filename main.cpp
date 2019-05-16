#include <cstdlib>
#include <iostream>

#include "GL/gl.h"
#include "GL/freeglut.h"

#include "particle.hpp"
#include "grid.hpp"
#include "mpm.hpp"

void display() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 0, 0);
    glBegin(GL_TRIANGLES);
        glVertex3f(0, 0, 0);
        glVertex3f(1, 0, 0);
        glVertex3f(0.5, 0.5, 0);
    glEnd();
    glFlush();
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("MPM Snow Simulation");
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}