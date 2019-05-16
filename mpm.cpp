#include "mpm.hpp"

MPM::MPM(int nGrid) {
    Particle* p1 = new Particle();
    p1 -> mass = 1;
    p1 -> position << 0.4, 0.4;
    p1 -> velocity << 1.0, 0.0;
    particles.push_back(p1);
    
    Particle* p2 = new Particle();
    p2 -> mass = 1;
    p2 -> position << 0.5, 0.5;
    p2 -> velocity << 0.5, 0.5;
    particles.push_back(p2);

    grids = std::vector<std::vector<Grid*>>(nGrid, std::vector<Grid*>(nGrid));
    for (int i = 0; i < nGrid; i++) {
        for (int j = 0; j < nGrid; j++) {
            grids[i][j] = new Grid();
            grids[i][j]->position << (((double)i + 0.5) / nGrid), (((double)j + 0.5) / nGrid);
            grids[i][j]->velocity << i * 0.1, j * 0.1;
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