#pragma once

#include "Eigen/Dense"
#include "GL/gl.h"
#include "GL/glut.h"

class Grid
{
public:
    double mass;
    Eigen::Vector2d velocity;
};