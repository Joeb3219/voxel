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
        float leftBound = 0.0f, rightBound = 1.0f;
        DL_ID = glGenLists(1);
    	glNewList(DL_ID,GL_COMPILE);
        glBegin(GL_QUADS);          // TOP
            glNormal3f(0.0f, 1.0f, 0.0f);
            glTexCoord2f(0.25f, 0.75f); glVertex3f(leftBound, rightBound, rightBound);
            glTexCoord2f(0.50f, 0.75f); glVertex3f(rightBound, rightBound, rightBound);
            glTexCoord2f(0.50f, 1.0f); glVertex3f(rightBound, rightBound, leftBound);
            glTexCoord2f(0.25f, 1.0f); glVertex3f(leftBound, rightBound, leftBound);
        glEnd();
        glBegin(GL_QUADS);          // FRONT
            glNormal3f(0.0f, 0.0f, 1.0f);
            glTexCoord2f(0.25f, 0.50f); glVertex3f(leftBound, leftBound, rightBound);
            glTexCoord2f(0.50f, 0.50f); glVertex3f(rightBound, leftBound, rightBound);
            glTexCoord2f(0.50f, 0.75f); glVertex3f(rightBound, rightBound, rightBound);
            glTexCoord2f(0.25f, 0.75f); glVertex3f(leftBound, rightBound, rightBound);
        glEnd();

        glBegin(GL_QUADS);          // RIGHT
            glNormal3f(1.0f, 0.0f, 0.0f);
            glTexCoord2f(0.50f, 0.50f); glVertex3f(rightBound, leftBound, rightBound);
            glTexCoord2f(0.75f, 0.50f); glVertex3f(rightBound, leftBound, leftBound);
            glTexCoord2f(0.75f, 0.75f); glVertex3f(rightBound, rightBound, leftBound);
            glTexCoord2f(0.50f, 0.75f); glVertex3f(rightBound, rightBound, rightBound);
        glEnd();
        glBegin(GL_QUADS);          // LEFT
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glTexCoord2f(0.0f, 0.50f); glVertex3f(leftBound, leftBound, leftBound);
            glTexCoord2f(0.25f, 0.50f); glVertex3f(leftBound, leftBound, rightBound);
            glTexCoord2f(0.25f, 0.75f); glVertex3f(leftBound, rightBound, rightBound);
            glTexCoord2f(0.0f, 0.75f); glVertex3f(leftBound, rightBound, leftBound);
        glEnd();
        glBegin(GL_QUADS);          // BOTTOM
            glNormal3f(0.0f, -1.0f, 0.0f);
            glTexCoord2f(0.25f, 0.25f); glVertex3f(leftBound, leftBound, rightBound);
            glTexCoord2f(0.50f, 0.25f); glVertex3f(rightBound, leftBound, rightBound);
            glTexCoord2f(0.50f, 0.50f); glVertex3f(rightBound, leftBound, leftBound);
            glTexCoord2f(0.25f, 0.50f); glVertex3f(leftBound, leftBound, leftBound);
        glEnd();
        glBegin(GL_QUADS);          // BACK
            glNormal3f(0.0f, 0.0f, -1.0f);
            glTexCoord2f(0.75f, 0.50f); glVertex3f(rightBound, leftBound, leftBound);
            glTexCoord2f(1.0f, 0.50f); glVertex3f(leftBound, leftBound, leftBound);
            glTexCoord2f(1.0f, 0.75f); glVertex3f(leftBound, rightBound, leftBound);
            glTexCoord2f(0.75f, 0.75f); glVertex3f(rightBound, rightBound, leftBound);
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
