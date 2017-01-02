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

    GLuint textureAtlas;

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

    void Cube::render(float x, float y, float z, float *texCoords){
        float leftBound = 0.0f, rightBound = 1.0f, increment = 1.0f / 32.0f;
        glBegin(GL_QUADS);          // TOP
            glNormal3f(0.0f, 1.0f, 0.0f);
            glTexCoord2f(texCoords[9], texCoords[8]); glVertex3f(x + leftBound, y + rightBound, z + rightBound);
            glTexCoord2f(texCoords[9] + increment, texCoords[8]); glVertex3f(x + rightBound, y + rightBound, z + rightBound);
            glTexCoord2f(texCoords[9] + increment, texCoords[8] + increment); glVertex3f(x + rightBound, y + rightBound, z + leftBound);
            glTexCoord2f(texCoords[9], texCoords[8] + increment); glVertex3f(x + leftBound, y + rightBound, z + leftBound);
        glEnd();
        glBegin(GL_QUADS);          // FRONT
            glNormal3f(0.0f, 0.0f, 1.0f);
            glTexCoord2f(texCoords[1], texCoords[0]); glVertex3f(x + leftBound, y + leftBound, z + rightBound);
            glTexCoord2f(texCoords[1] + increment, texCoords[0]); glVertex3f(x + rightBound, y + leftBound, z + rightBound);
            glTexCoord2f(texCoords[1] + increment, texCoords[0] + increment); glVertex3f(x + rightBound, y + rightBound, z + rightBound);
            glTexCoord2f(texCoords[1], texCoords[0] + increment); glVertex3f(x + leftBound, y + rightBound, z + rightBound);
        glEnd();

        glBegin(GL_QUADS);          // RIGHT
            glNormal3f(1.0f, 0.0f, 0.0f);
            glTexCoord2f(texCoords[7], texCoords[6]); glVertex3f(x + rightBound, y + leftBound, z + rightBound);
            glTexCoord2f(texCoords[7] + increment, texCoords[6]); glVertex3f(x + rightBound, y + leftBound, z + leftBound);
            glTexCoord2f(texCoords[7] + increment, texCoords[6] + increment); glVertex3f(x + rightBound, y + rightBound, z + leftBound);
            glTexCoord2f(texCoords[7], texCoords[6] + increment); glVertex3f(x + rightBound, y + rightBound, z + rightBound);
        glEnd();
        glBegin(GL_QUADS);          // LEFT
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glTexCoord2f(texCoords[3], texCoords[2]); glVertex3f(x + leftBound, y + leftBound, z + leftBound);
            glTexCoord2f(texCoords[3] + increment, texCoords[2]); glVertex3f(x + leftBound, y + leftBound, z + rightBound);
            glTexCoord2f(texCoords[3] + increment, texCoords[2] + increment); glVertex3f(x + leftBound, y + rightBound, z + rightBound);
            glTexCoord2f(texCoords[3], texCoords[2] + increment); glVertex3f(x + leftBound, y + rightBound, z + leftBound);
        glEnd();
        glBegin(GL_QUADS);          // BOTTOM
            glNormal3f(0.0f, -1.0f, 0.0f);
            glTexCoord2f(texCoords[11], texCoords[10]); glVertex3f(x + leftBound, y + leftBound, z + rightBound);
            glTexCoord2f(texCoords[11] + increment, texCoords[10]); glVertex3f(x + rightBound, y + leftBound, z + rightBound);
            glTexCoord2f(texCoords[11] + increment, texCoords[10] + increment); glVertex3f(x + rightBound, y + leftBound, z + leftBound);
            glTexCoord2f(texCoords[11], texCoords[10] + increment); glVertex3f(x + leftBound, y + leftBound, z + leftBound);
        glEnd();
        glBegin(GL_QUADS);          // BACK
            glNormal3f(0.0f, 0.0f, -1.0f);
            glTexCoord2f(texCoords[5], texCoords[4]); glVertex3f(x + rightBound, y + leftBound, z + leftBound);
            glTexCoord2f(texCoords[5] + increment, texCoords[4]); glVertex3f(x + leftBound, y + leftBound, z + leftBound);
            glTexCoord2f(texCoords[5] + increment, texCoords[4] + increment); glVertex3f(x + leftBound, y + rightBound, z + leftBound);
            glTexCoord2f(texCoords[5], texCoords[4] + increment); glVertex3f(x + rightBound, y + rightBound, z + leftBound);
        glEnd();
    }

}
