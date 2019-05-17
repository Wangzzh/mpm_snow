#include <cstdlib>
#include <iostream>

#include "GL/gl.h"
#include "GL/glut.h"

#include "particle.hpp"
#include "grid.hpp"
#include "mpm.hpp"

MPM* mpm;

int n = 100;
double dTime = 0.01;

void display() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // mpm->step();
    mpm->render();
    // std::cout << "step" << std::endl;

    glFlush();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == ' ' ) {
        std::cout << "Step" << std::endl;
        mpm -> step();
        // mpm -> render();
    }
}

void idle() {
    glutPostRedisplay();
}

int main(int argc, char* argv[]) {
    MaterialParameters material = MaterialParameters(140000, 0.2, 10, 0.025, 0.0075);
    mpm = new MPM(n, dTime, material);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("MPM Snow Simulation");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, 0, 100);

    glutMainLoop();
    return 0;
}