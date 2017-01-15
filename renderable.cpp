#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <GL/glut.h>
#include <ctime>
#include <algorithm>
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

    float* Cube::renderFace(float x, float y, float z, Face face, float *texCoords){
        float leftBound = 0.0f, rightBound = 1.0f, increment = 1.0f / 32.0f;
        if(face == Face::FACE_TOP){
            return new float[20]{//0.0f, 1.0f, 0.0f,
                x + leftBound, y + rightBound, z + rightBound, texCoords[9], texCoords[8],
                x + rightBound, y + rightBound, z + rightBound, texCoords[9] + increment, texCoords[8],
                x + rightBound, y + rightBound, z + leftBound, texCoords[9] + increment, texCoords[8] + increment,
                x + leftBound, y + rightBound, z + leftBound, texCoords[9], texCoords[8] + increment};
        }else if(face == Face::FACE_FRONT){
            return new float[20]{//0.0f, 0.0f, 1.0f,
                x + leftBound, y + leftBound, z + rightBound, texCoords[1], texCoords[0],
                x + rightBound, y + leftBound, z + rightBound, texCoords[1] + increment, texCoords[0],
                x + rightBound, y + rightBound, z + rightBound, texCoords[1] + increment, texCoords[0] + increment,
                x + leftBound, y + rightBound, z + rightBound, texCoords[1], texCoords[0] + increment};
        }else if(face == Face::FACE_RIGHT){
            return new float[20]{//1.0f, 0.0f, 0.0f,
                x + rightBound, y + leftBound, z + rightBound, texCoords[7], texCoords[6],
                x + rightBound, y + leftBound, z + leftBound, texCoords[7] + increment, texCoords[6],
                x + rightBound, y + rightBound, z + leftBound, texCoords[7] + increment, texCoords[6] + increment,
                x + rightBound, y + rightBound, z + rightBound, texCoords[7], texCoords[6] + increment};
        }else if(face == Face::FACE_LEFT){
            return new float[20]{//-1.0f, 0.0f, 0.0f,
                x + leftBound, y + leftBound, z + leftBound, texCoords[3], texCoords[2],
                x + leftBound, y + leftBound, z + rightBound, texCoords[3] + increment, texCoords[2],
                x + leftBound, y + rightBound, z + rightBound, texCoords[3] + increment, texCoords[2] + increment,
                x + leftBound, y + rightBound, z + leftBound, texCoords[3], texCoords[2] + increment};
        }else if(face == Face::FACE_BOTTOM){
            return new float[20]{//0.0f, -1.0f, 0.0f,
                x + leftBound, y + leftBound, z + rightBound, texCoords[11], texCoords[10],
                x + rightBound, y + leftBound, z + rightBound, texCoords[11] + increment, texCoords[10],
                x + rightBound, y + leftBound, z + leftBound, texCoords[11] + increment, texCoords[10] + increment,
                x + leftBound, y + leftBound, z + leftBound, texCoords[11], texCoords[10] + increment};
        }else if(face == Face::FACE_BACK){
            return new float[20]{//0.0f, 0.0f, -1.0f,
                x + rightBound, y + leftBound, z + leftBound, texCoords[5], texCoords[4],
                x + leftBound, y + leftBound, z + leftBound, texCoords[5] + increment, texCoords[4],
                x + leftBound, y + rightBound, z + leftBound, texCoords[5] + increment, texCoords[4] + increment,
                x + rightBound, y + rightBound, z + leftBound, texCoords[5], texCoords[4] + increment};
        }
        return 0;
    }

    void Cube::render(float x, float y, float z, float *texCoords){
        renderFace(x, y, z, Face::FACE_TOP, texCoords);
        renderFace(x, y, z, Face::FACE_FRONT, texCoords);
        renderFace(x, y, z, Face::FACE_RIGHT, texCoords);
        renderFace(x, y, z, Face::FACE_LEFT, texCoords);
        renderFace(x, y, z, Face::FACE_BOTTOM, texCoords);
        renderFace(x, y, z, Face::FACE_BACK, texCoords);
    }

}
