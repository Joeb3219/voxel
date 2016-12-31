#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <ctime>
#include "lib/fastNoise/FastNoise.h"
#include "renderable.h"
#include "math.h"

namespace VOX_Graphics{

    Cube::Cube(){
        float size = 0.5f;
        DL_ID = glGenLists(1);
    	glNewList(DL_ID,GL_COMPILE);
        glBegin(GL_QUADS);          // TOP
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(-size, size, size);
            glVertex3f(size, size, size);
            glVertex3f(size, size, -size);
            glVertex3f(-size, size, -size);
        glEnd();
        glBegin(GL_QUADS);          // FRONT
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-size, -size, size);
            glVertex3f(size, -size, size);
            glVertex3f(size, size, size);
            glVertex3f(-size, size, size);
        glEnd();

        glBegin(GL_QUADS);          // RIGHT
            glNormal3f(1.0f, 0.0f, 0.0f);
            glVertex3f(size, -size, size);
            glVertex3f(size, -size, -size);
            glVertex3f(size, size, -size);
            glVertex3f(size, size, size);
        glEnd();
        glBegin(GL_QUADS);          // LEFT
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glVertex3f(-size, -size, size);
            glVertex3f(-size, size, size);
            glVertex3f(-size, size, -size);
            glVertex3f(-size, -size, -size);
        glEnd();
        glBegin(GL_QUADS);          // BOTTOM
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(-size, -size, size);
            glVertex3f(size, -size, size);
            glVertex3f(size, -size, -size);
            glVertex3f(-size, -size, -size);
        glEnd();
        glBegin(GL_QUADS);          // BACK
            glNormal3f(0.0f, 0.0f, -1.0f);
            glVertex3f(size, size, -size);
            glVertex3f(size, -size, -size);
            glVertex3f(-size, -size, -size);
            glVertex3f(-size, size, -size);
        glEnd();
        glEndList();
    }

    Cube& Cube::getInstance(){
        static Cube instance;
        return instance;
    }

    void Cube::render(float x, float y, float z){
        glPushMatrix();
        glTranslatef(x, y, z);
        glColor3f(.2f, .4f, .3f);
        glCallList(DL_ID);
        glPopMatrix();
    }

}
