#include <iostream>
#include <SFML/Window.hpp>
#include <cmath>

#define GRAVITY_PER_TICK -0.08f/30

namespace VOX_Math{

    sf::Vector3f normalizeVector(sf::Vector3f vector){
        float length = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
        return sf::Vector3f(vector.x / length, vector.y / length, vector.z / length);
    }

    sf::Vector3f crossVector(sf::Vector3f v1, sf::Vector3f v2){
        return sf::Vector3f(
                v1.y*v2.z - v1.z*v2.y,
                v1.z*v2.x - v1.x*v2.z,
                v1.x*v2.y - v1.y*v2.x
        );
    }

    float convertScale(float x, float oldMin, float oldMax, float newMin, float newMax){
        return (( (x - oldMin) / (oldMax - oldMin)) * (newMax - newMin)) + newMin;
    }

    void calculateFalling(float *yPosition, float *yVelocity, int ticksPassed){
        (*yPosition) += (*yVelocity) * ticksPassed;
        (*yVelocity) += GRAVITY_PER_TICK * ticksPassed;
    }

    float norm(sf::Vector3f vec){
        return sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    }

}
