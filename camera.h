#ifndef CAMERA_H_
#define CAMERA_H_

#include <SFML/Window.hpp>
#include "renderable.h"
#define DEBUG 0

class Camera{

private:
    sf::Window *screen;
    float x, y, z;
    float rX, rY, rZ;
    bool mouseGrabbed;
    void gluPerspective(float fov, float aspectRatio, float near, float far);

public:
    bool focused;
    int width, height;
    Camera(int width, int height);
    bool handleEvents();
    void preRender();
    void postRender();
    void renderHUD();
    void windowResized(int width, int height);
    void update(sf::Vector3f cameraPos, sf::Vector3f angle);
    sf::Vector2i getRelativeMousePosition();
    ~Camera();
//    sf::Vector3f getCurrentPosition();
    sf::Vector3f getLookingAt();
};

#endif
