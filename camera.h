#ifndef CAMERA_H_
#define CAMERA_H_

#include <SFML/Window.hpp>
#include "renderable.h"
#define DEBUG 0

#define NEARCLIP 0.0001
#define FARCLIP 400.0
#define FOV 60

namespace VOX_Mob{
    class Player;
}

class Camera{

private:
    sf::Window *screen;
    float x = 0.0f, y = 0.0f, z = 0.0f;
    float rX = 0.0f, rY = 0.0f, rZ = 0.0f;
    bool mouseGrabbed;
    void gluPerspective(float fov, float aspectRatio, float near, float far);
    VOX_Mob::Player *following;
public:
    bool focused = true;
    int width = 0, height = 0;
    Camera(int width, int height);
    void setFollowing(VOX_Mob::Player *player);
    bool handleEvents();
    void preRender();
    void postRender();
    void pre2DRender();
    void windowResized(int width, int height);
    void update();
    sf::Vector2i getRelativeMousePosition();
    ~Camera();
};

#endif
