#ifndef FPS_MATH_H
#define FPS_MATH_H

#include <SFML/Window.hpp>

#define PI 3.14159265

namespace VOX_Math{

    sf::Vector3f normalizeVector(sf::Vector3f vector);
    sf::Vector3f crossVector(sf::Vector3f v1, sf::Vector3f v2);
    float convertScale(float x, float oldMin, float oldMax, float newMin, float newMax);
    void calculateFalling(float *yPosition, float *yVelocity, int ticksPassed);
    float norm(sf::Vector3f vec);

}

#endif
