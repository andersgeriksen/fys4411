#pragma once
#include <vector>
#include <cassert>

class Particle {
public:
    Particle();
    void setPosition(const std::vector<double> &position);
    void adjustPosition(double change, int dimension);
    void setNumberOfDimensions(int numberOfDimensions);
    std::vector<double> getPosition() { return m_position; }
    double lengthSquared();

private:
    int     m_numberOfDimensions = 0;
    std::vector<double> m_position = std::vector<double>();
};

