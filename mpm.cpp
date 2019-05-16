#include "mpm.hpp"

MPM::MPM() {
    Particle* p1 = new Particle();
    p1 -> mass = 1;
    p1 -> position << 0, 0;
    p1 -> velocity = Eigen::Vector2d::Zero();
    particles.push_back(p1);
    
    Particle* p2 = new Particle();
    p2 -> mass = 1;
    p2 -> position << 0.5, 0.5;
    p2 -> velocity = Eigen::Vector2d::Zero();
    particles.push_back(p2);
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
    for (auto& particle : particles) {
        particle->render();
    }
}