#include "mpm.hpp"

MPM::MPM(int nGrid, double timeStep) {
    this->nGrid = nGrid;
    this->time = 0;
    this->timeStep = timeStep;

    Particle* p1 = new Particle();
    p1 -> mass = 1;
    p1 -> position << 0.47, 0.63;
    p1 -> velocity << 1.0, 0.0;
    particles.push_back(p1);
    
    Particle* p2 = new Particle();
    p2 -> mass = 2;
    p2 -> position << 0.5, 0.52;
    p2 -> velocity << 0.5, 0.5;
    particles.push_back(p2);

    Particle* p3 = new Particle();
    p3 -> mass = 1;
    p3 -> position << 0.81, 0.87;
    p3 -> velocity << -0.2, 0.5;
    particles.push_back(p3);

    grids = std::vector<std::vector<Grid*>>(nGrid, std::vector<Grid*>(nGrid));
    for (int i = 0; i < nGrid; i++) {
        for (int j = 0; j < nGrid; j++) {
            grids[i][j] = new Grid();
            grids[i][j]->position << (((double)i + 0.5) / nGrid), (((double)j + 0.5) / nGrid);
            grids[i][j]->linearMomentum << i * 0.1, j * 0.1;
        }
    }
}

MPM::~MPM() {
    for (auto& particle : particles) {
        delete particle;
    }
    for (auto& gridVec : grids) {
        for (auto& grid : gridVec) {
            delete grid;
        }
    }
}

void MPM::step() {
    particleToGrid();
    if (time == 0.) computeParticleDensity();

    time += timeStep;
}

void MPM::render() {
    // Rendering particles
    glColor3f(1, 0, 0);
    for (auto& particle : particles) {
        particle->render();
    }
    
    // Rendering grids
    glColor3f(1, 1, 0);
    for (auto& gridVec : grids) {
        for (auto& grid : gridVec) {
            grid -> render();
        }
    }
}

std::vector<double> MPM::calculateWeights(double x) {
    std::vector<double> xWeight;
    xWeight.push_back(-(1+x) * (1+x) * (1+x) / 6. + (1+x) * (1+x) - 2 * (1+x) + 4./3.);
    xWeight.push_back(0.5 * x * x * x - x * x + 2./3.);
    xWeight.push_back(0.5 * (1-x) * (1-x) * (1-x) - (1-x) * (1-x) + 2./3.);
    xWeight.push_back(-(2-x) * (2-x) * (2-x) / 6. + (2-x) * (2-x) - 2 * (2-x) + 4./3.);
    return xWeight;
}

void MPM::particleToGrid() {
    for (auto& gridVec : grids) {
        for (auto& grid : gridVec) {
            grid -> mass = 0;
            grid -> linearMomentum = Eigen::Vector2d::Zero();
        }
    }
    for (auto& particle : particles) {
        particle->xLeft = (int) (particle->position[0] * nGrid - 0.5);
        particle->yLeft = (int) (particle->position[1] * nGrid - 0.5);
        particle->xDiff = (particle->position[0] * nGrid) - particle->xLeft - 0.5;
        particle->yDiff = (particle->position[1] * nGrid) - particle->yLeft - 0.5;

        particle->xWeight = calculateWeights(particle->xDiff);
        particle->yWeight = calculateWeights(particle->yDiff);
        
        for (int dx = -1; dx <= 2; dx++) {
            for (int dy = -1; dy <= 2; dy++) {
                if (particle->xLeft + dx >= 0 && particle->xLeft + dx < nGrid 
                        && particle->yLeft + dy >= 0 && particle->yLeft + dy < nGrid) {
                    grids[particle->xLeft + dx][particle->yLeft + dy] -> mass += 
                        particle->xWeight[dx+1] * particle->yWeight[dy+1] * particle->mass;
                    grids[particle->xLeft + dx][particle->yLeft + dy] -> linearMomentum += 
                        particle->xWeight[dx+1] * particle->yWeight[dy+1] * particle->mass * particle-> velocity;
                }
            }
        }
    }
}

void MPM::computeParticleDensity() {
    for (auto& particle : particles) {        
        particle->density = 0;
        for (int dx = -1; dx <= 2; dx++) {
            for (int dy = -1; dy <= 2; dy++) {
                if (particle->xLeft + dx >= 0 && particle->xLeft + dx < nGrid 
                        && particle->yLeft + dy >= 0 && particle->yLeft + dy < nGrid) {
                    particle->density += grids[particle->xLeft + dx][particle->yLeft + dy] -> mass * 
                        particle->xWeight[dx+1] * particle->yWeight[dy+1] * nGrid * nGrid;
                }
            }
        }
        particle->volume = particle->mass / particle->density;
        std::cout << "r " << particle->density << std::endl;
        std::cout << "v " << particle->volume << std::endl;
    }
}
