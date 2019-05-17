#include "mpm.hpp"

MaterialParameters::MaterialParameters(double youngsModulus, double poissonsRatio, 
        double hardening, double criticalCompression, double criticalStretch) {
    this->E = youngsModulus;
    this->nu = poissonsRatio;
    this->xsi = hardening;
    this->thetaC = criticalCompression;
    this->thetaS = criticalStretch;
    this->lambda = E * nu / (1 + nu) / (1 - 2 * nu);
    this->mu = E / 2 / (1 + nu);
}

MPM::MPM(int nGrid, double timeStep, MaterialParameters material) {
    this->nGrid = nGrid;
    this->time = 0;
    this->timeStep = timeStep;
    this->material = material;

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
    computeGridForce();

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

        particle->calculateWeights();
        
        for (int dx = -1; dx <= 2; dx++) {
            for (int dy = -1; dy <= 2; dy++) {
                int xGrid = particle->xLeft + dx;
                int yGrid = particle->yLeft + dy;
                if (xGrid >= 0 && xGrid < nGrid && yGrid >= 0 && yGrid < nGrid) {
                    grids[xGrid][yGrid] -> mass += particle->xWeight[dx+1] * particle->yWeight[dy+1] * particle->mass;
                    grids[xGrid][yGrid] -> linearMomentum += particle->xWeight[dx+1] * particle->yWeight[dy+1] * particle->mass * particle-> velocity;
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
                int xGrid = particle->xLeft + dx;
                int yGrid = particle->yLeft + dy;
                if (xGrid >= 0 && xGrid < nGrid && yGrid >= 0 && yGrid < nGrid) {
                    particle->density += grids[xGrid][yGrid] -> mass * particle->xWeight[dx+1] * particle->yWeight[dy+1] * nGrid * nGrid;
                }
            }
        }
        particle->volume = particle->mass / particle->density;

        // std::cout << "r " << particle->density << std::endl;
        // std::cout << "v " << particle->volume << std::endl;
    }
}

void MPM::computeGridForce() {
    for (auto& gridVec : grids) {
        for (auto& grid : gridVec) {
            grid -> force = Eigen::Vector2d::Zero();
            grid -> newPosition = grid -> position + timeStep * grid -> linearMomentum / grid -> mass;
        }
    }
    for (auto& particle : particles) {   
        std::cout << "--------------------------------------------" << std::endl;

        // Calculate FEHat
        Eigen::Matrix2d FEHat = particle->FE;
        for (int dx = -1; dx <= 2; dx++) {
            for (int dy = -1; dy <= 2; dy++) {
                int xGrid = particle->xLeft + dx;
                int yGrid = particle->yLeft + dy;
                if (xGrid >= 0 && xGrid < nGrid && yGrid >= 0 && yGrid < nGrid) {
                    Eigen::Vector2d weightGradient;
                    weightGradient << particle->xWeightGradient[dx+1] * particle->yWeight[dy+1], 
                                      particle->yWeightGradient[dy+1] * particle->xWeight[dx+1];
                    FEHat += (grids[xGrid][yGrid] -> newPosition - grids[xGrid][yGrid] -> position)
                        * weightGradient.transpose() * particle->FE;
                }
            }
        }

        for (int dx = -1; dx <= 2; dx++) {
            for (int dy = -1; dy <= 2; dy++) {
                int xGrid = particle->xLeft + dx;
                int yGrid = particle->yLeft + dy;
                if (xGrid >= 0 && xGrid < nGrid && yGrid >= 0 && yGrid < nGrid) {
                    double JE = particle->FE.determinant();
                    double JP = particle->FP.determinant();
                    double lambda = material.lambda * exp(material.xsi * (1 - JP));
                    double mu = material.mu * exp(material.xsi * (1 - JP));
                    std::cout << "JE: " << JE << std::endl;
                    std::cout << "JP: " << JP << std::endl;
                    std::cout << "lambda: " << lambda << std::endl;
                    std::cout << "mu: " << mu << std::endl;

                    Eigen::JacobiSVD<Eigen::MatrixXd> svd(particle->FE, Eigen::ComputeThinU | Eigen::ComputeThinV);
                    Eigen::Matrix2d U = svd.matrixU();
                    Eigen::Matrix2d V = svd.matrixV();
                    Eigen::Matrix2d RE = U * V.transpose();
                    Eigen::Matrix2d S = U.inverse() * particle->FE * V.transpose().inverse();
                    std::cout << "U" << std::endl << U << std::endl;
                    std::cout << "V" << std::endl << V << std::endl;
                    std::cout << "S" << std::endl << S << std::endl;
                    std::cout << "R" << std::endl << RE << std::endl;

                    Eigen::Matrix2d dPdF = 2 * mu * (particle->FE - RE) + lambda * (JE - 1) * JE * particle->FE.transpose().inverse();
                    Eigen::Matrix2d sigma;
                    sigma = 1. / JP * dPdF * particle->FE.transpose();
                    std::cout << "dpdf" << std::endl << dPdF << std::endl;
                    std::cout << "sigma" << std::endl << sigma << std::endl;


                    Eigen::Vector2d weightGradient;
                    weightGradient << particle->xWeightGradient[dx+1] * particle->yWeight[dy+1], 
                                      particle->yWeightGradient[dy+1] * particle->xWeight[dx+1];
                    grids[xGrid][yGrid] -> force -= particle->volume * sigma * weightGradient;
                }
            }
        }
    }
}
