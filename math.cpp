#include <iostream>
#include <SFML/Window.hpp>
#include <cmath>
#include "math.h"

#define GRAVITY_PER_TICK -0.11f/30

namespace VOX_Math{

    float areaOfTriangle(sf::Vector3f origin, sf::Vector3f left, sf::Vector3f right){
        return fabs((origin.x*(left.z-right.z) + left.x*(right.z-origin.z)+ right.x*(origin.z-left.z))/2.0);
    }

    bool insideTriangle(sf::Vector3f origin, sf::Vector3f left, sf::Vector3f right, sf::Vector3f point){
        float totalArea = areaOfTriangle (origin, left, right);
        float a = areaOfTriangle (point, left, right);
        float b = areaOfTriangle (origin, point, right);
        float c = areaOfTriangle (origin, left, point);
        return fabs(totalArea - (a + b + c)) <= 0.001;
    }

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

    sf::Vector3f computeVectorFromPos(sf::Vector3f origin, float rX, float rY, float dist){
        sf::Vector3f lookingAt;
        float rYRadians = (PI / 180.0) * rY;
        float rXRadians = (PI / 180.0) * (rX + 90);
        lookingAt.x = origin.x - ((float) cos(rXRadians) * dist * fabs(cos(rYRadians)));
        lookingAt.y = (origin.y) - ((float) sin(rYRadians) * dist);
        lookingAt.z = origin.z - ((float) sin(rXRadians) * dist * fabs(cos(rYRadians)));
        return lookingAt;
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
