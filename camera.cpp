#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <cmath>
#include "camera.h"
#include "renderable.h"

#define PI 3.14159265
#define NEARCLIP 0.001
#define FARCLIP 400.0
#define FOV 70

Camera::Camera(int width, int height){
    x = y = z = rX = rY = rZ = 0.0f;
    this->width = width;
    this->height = height;
    sf::Window *window = new sf::Window(sf::VideoMode(width, height), "Joeb3219 | FPS", sf::Style::Default, sf::ContextSettings(32));
    windowResized(width, height);
    screen = window;
    mouseGrabbed = focused = true;
    sf::Mouse::setPosition(sf::Vector2i(width / 2, height / 2), *screen);
}

void Camera::update(sf::Vector3f cameraPos, sf::Vector3f angle){
    this->x = cameraPos.x;
    this->y = cameraPos.y;
    this->z = cameraPos.z;
    this->rX = angle.x;
    this->rY = angle.y;
    this->rZ = angle.z;
    if(mouseGrabbed) sf::Mouse::setPosition(sf::Vector2i(width / 2, height / 2), *screen);
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::P)) mouseGrabbed = !mouseGrabbed;
}

sf::Vector3f Camera::getLookingAt(){
    float rYRadians = (PI / 180.0) * rY;
    float rXRadians = (PI / 180.0) * (rX + 90);
    float xPrime = x - ((float) cos(rXRadians) * 8.f * fabs(cos(rYRadians)));
    float yPrime = y - ((float) sin(rYRadians) * 8.f);
    float zPrime = z - ((float) sin(rXRadians) * 8.f * fabs(cos(rYRadians)));
    return sf::Vector3f(xPrime, yPrime, zPrime);
}

sf::Vector2i Camera::getRelativeMousePosition(){
    if(!mouseGrabbed) return sf::Vector2i(0, 0);
    sf::Vector2i vec = sf::Mouse::getPosition(*screen);
    vec.y = (height / 2) - vec.y;
    vec.x = vec.x - (width / 2);
    return vec;
}

void Camera::windowResized(int width, int height){
    this->width = width;
    this->height = height;
    if(DEBUG == 1) std::cout << "Camera::windowResized: " << width << ", " << height << std::endl;

    glViewport(0, 0, width, height);
    glOrtho(0, width, 0, height, NEARCLIP, FARCLIP);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV, (width * 1.0) / height, NEARCLIP, FARCLIP);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnable(GL_NORMALIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Camera::gluPerspective(float fov, float aspectRatio, float near, float far){
    GLdouble fW, fH;
    fH = tan(fov / 360 * PI) * near;
    fW = fH * (width * 1.0 / height);
    glFrustum(-fW, fW, -fH, fH, near, far);
}

// Handles events of the screen, and then returns true if program should continue executing, or false otherwise.
bool Camera::handleEvents(){
    sf::Event event;
    while (screen->pollEvent(event)){
        if (event.type == sf::Event::Closed) return false;
        else if(event.type == sf::Event::Resized) windowResized(event.size.width, event.size.height);
        else if (event.type == sf::Event::LostFocus) focused = false;
        else if (event.type == sf::Event::GainedFocus) focused = true;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) return false;
    return true;
}

void Camera::renderHUD(){
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, 128, 0, 128, 0, 1);
    glPopMatrix();
}

void Camera::preRender(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 1.f, 1.f);
    glLoadIdentity();
    glDepthRange(0, 1);
    glMatrixMode(GL_MODELVIEW);
	glRotatef(rY, 1, 0, 0);
	glRotatef(rX, 0, 1, 0);
	glRotatef(rZ, 0, 0, 1);
	glTranslatef(-x, -y, -z);
    glMatrixMode(GL_MODELVIEW);
}

void Camera::postRender(){
        screen->display();
}

Camera::~Camera(){

}
