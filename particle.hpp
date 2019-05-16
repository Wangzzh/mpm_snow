#pragma once

#include <iostream>

#include "Eigen/Dense"
#include "GL/gl.h"
#include "GL/glut.h"

class Particle
{
public:
    void render();

    double mass;
    Eigen::Vector2d position;
    Eigen::Vector2d velocity;
};