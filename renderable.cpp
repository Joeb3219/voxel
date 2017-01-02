#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <ctime>
#include "lib/fastNoise/FastNoise.h"
#include "fileIO.h"
#include "renderable.h"
#include "math.h"

namespace VOX_Graphics{

    GLuint Text::letterDisplayLists;

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

    Text::Text(){
        fullTextmap = VOX_FileIO::loadBitmapTexture("res/text.bmp");
        letterDisplayLists = glGenLists(NUM_SYMBOLS);
        int numPerRow = 32, row = 0, col = 0;
        GLuint dl_id;
        for(int i = 0; i < NUM_SYMBOLS; i ++){
            if(row == numPerRow){
                col ++;
                row = 0;
            }else row ++;

            dl_id = letterDisplayLists + i;
            glNewList(dl_id, GL_COMPILE);
                glBindTexture(GL_TEXTURE_2D, fullTextmap);
                glBegin(GL_QUADS);
                    glTexCoord2f(1.0f / row, 1.0f / (col + 1)); glVertex2f(0.f, 0.f);
                    glTexCoord2f(1.0f / (row + 1), 1.0f / (col + 1)); glVertex2f(1.f, 0.f);
                    glTexCoord2f(1.0f / (row + 1), 1.0f / (col)); glVertex2f(1.f, 1.f);
                    glTexCoord2f(1.0f / (row), 1.0f / (col)); glVertex2f(0.f, 1.f);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, 0);
            glEndList();
        }
    }

    void Text::renderLetter(char c, float x, float y, float size){
        //glTranslatef(x, y, 1.f);
        //glCallList(letterDisplayLists + (c - 0x20));
        int row = (c - 0x20) / 32;
        int col = (c - 0x20) % 32;
            glBindTexture(GL_TEXTURE_2D, fullTextmap);
            glBegin(GL_QUADS);
                glTexCoord2f(1.0f / row, 1.0f / (col + 1)); glVertex3f(x + 0.f, y + 0.f, 0.f);
                glTexCoord2f(1.0f / (row + 1), 1.0f / (col + 1)); glVertex3f(x + 1.f, y + 0.f, 0.f);
                glTexCoord2f(1.0f / (row + 1), 1.0f / (col)); glVertex3f(x + 1.f, y + 1.f, 0.f);
                glTexCoord2f(1.0f / (row), 1.0f / (col)); glVertex3f(x + 0.f, y + 1.f, 0.f);
            glEnd();
            glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Text::renderString(std::string str, float x, float y, float size){
        glColor3f(1.0f, 1.0f, 1.0f);
        for(unsigned int i = 0; i < str.size(); i ++){
            renderLetter(str.at(i), x + (i * size), y);
        }
    }

    Text &Text::getInstance(){
        static Text text;
        return text;
    }

}
