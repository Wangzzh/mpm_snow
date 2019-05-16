#pragma once

#include "Eigen/Dense"
#include "GL/gl.h"
#include "GL/glut.h"

class Grid
{
public:
    void render();

    Eigen::Vector2d position;

    double mass;
    Eigen::Vector2d velocity;
};