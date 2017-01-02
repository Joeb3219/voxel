#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <GL/glut.h>
#include <ctime>
#include "lib/fastNoise/FastNoise.h"
#include "fileIO.h"
#include "renderable.h"
#include "math.h"

namespace VOX_Graphics{

    void renderString(int x, int y, std::string str){
        glRasterPos2i(x, y);
        glColor3f(1.f, 1.f, 1.f);
        for ( unsigned int i = 0; i < str.size(); ++i ) {
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str.at(i));
        }
    }

    Cube::Cube(){
    }

    Cube& Cube::getInstance(){
        static Cube instance;
        return instance;
    }

    void Cube::render(float x, float y, float z){
        float leftBound = 0.0f, rightBound = 1.0f;
        glBegin(GL_QUADS);          // TOP
            glNormal3f(0.0f, 1.0f, 0.0f);
            glTexCoord2f(0.25f, 0.75f); glVertex3f(x + leftBound, y + rightBound, z + rightBound);
            glTexCoord2f(0.50f, 0.75f); glVertex3f(x + rightBound, y + rightBound, z + rightBound);
            glTexCoord2f(0.50f, 1.0f); glVertex3f(x + rightBound, y + rightBound, z + leftBound);
            glTexCoord2f(0.25f, 1.0f); glVertex3f(x + leftBound, y + rightBound, z + leftBound);
        glEnd();
        glBegin(GL_QUADS);          // FRONT
            glNormal3f(0.0f, 0.0f, 1.0f);
            glTexCoord2f(0.25f, 0.50f); glVertex3f(x + leftBound, y + leftBound, z + rightBound);
            glTexCoord2f(0.50f, 0.50f); glVertex3f(x + rightBound, y + leftBound, z + rightBound);
            glTexCoord2f(0.50f, 0.75f); glVertex3f(x + rightBound, y + rightBound, z + rightBound);
            glTexCoord2f(0.25f, 0.75f); glVertex3f(x + leftBound, y + rightBound, z + rightBound);
        glEnd();

        glBegin(GL_QUADS);          // RIGHT
            glNormal3f(1.0f, 0.0f, 0.0f);
            glTexCoord2f(0.50f, 0.50f); glVertex3f(x + rightBound, y + leftBound, z + rightBound);
            glTexCoord2f(0.75f, 0.50f); glVertex3f(x + rightBound, y + leftBound, z + leftBound);
            glTexCoord2f(0.75f, 0.75f); glVertex3f(x + rightBound, y + rightBound, z + leftBound);
            glTexCoord2f(0.50f, 0.75f); glVertex3f(x + rightBound, y + rightBound, z + rightBound);
        glEnd();
        glBegin(GL_QUADS);          // LEFT
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glTexCoord2f(0.0f, 0.50f); glVertex3f(x + leftBound, y + leftBound, z + leftBound);
            glTexCoord2f(0.25f, 0.50f); glVertex3f(x + leftBound, y + leftBound, z + rightBound);
            glTexCoord2f(0.25f, 0.75f); glVertex3f(x + leftBound, y + rightBound, z + rightBound);
            glTexCoord2f(0.0f, 0.75f); glVertex3f(x + leftBound, y + rightBound, z + leftBound);
        glEnd();
        glBegin(GL_QUADS);          // BOTTOM
            glNormal3f(0.0f, -1.0f, 0.0f);
            glTexCoord2f(0.25f, 0.25f); glVertex3f(x + leftBound, y + leftBound, z + rightBound);
            glTexCoord2f(0.50f, 0.25f); glVertex3f(x + rightBound, y + leftBound, z + rightBound);
            glTexCoord2f(0.50f, 0.50f); glVertex3f(x + rightBound, y + leftBound, z + leftBound);
            glTexCoord2f(0.25f, 0.50f); glVertex3f(x + leftBound, y + leftBound, z + leftBound);
        glEnd();
        glBegin(GL_QUADS);          // BACK
            glNormal3f(0.0f, 0.0f, -1.0f);
            glTexCoord2f(0.75f, 0.50f); glVertex3f(x + rightBound, y + leftBound, z + leftBound);
            glTexCoord2f(1.0f, 0.50f); glVertex3f(x + leftBound, y + leftBound, z + leftBound);
            glTexCoord2f(1.0f, 0.75f); glVertex3f(x + leftBound, y + rightBound, z + leftBound);
            glTexCoord2f(0.75f, 0.75f); glVertex3f(x + rightBound, y + rightBound, z + leftBound);
        glEnd();
    }

}
