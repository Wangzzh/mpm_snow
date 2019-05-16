#include "particle.hpp"

void Particle::render() {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(position[0], position[1], 0.);
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();
}