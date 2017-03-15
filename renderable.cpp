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

    void renderString(int x, int y, std::string str, sf::Vector3f color){
        glPushAttrib(GL_CURRENT_BIT);
        glColor3f(color.x, color.y, color.z);
        glRasterPos2i(x, y);
        for(unsigned int i = 0; i < str.size(); i++){
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, str.at(i));
        }
        glPopAttrib();
    }

    Cube::Cube(){
    }

    Cube& Cube::getInstance(){
        static Cube instance;
        return instance;
    }

    float* Cube::renderFace(float x, float y, float z, Face face, float *texCoords, unsigned char lightLevel){
        float leftBound = 0.0f, rightBound = 1.0f, increment = 1.0f / 32.0f;
        float color = (0.8/15.0f)*lightLevel;
        color += 0.1f;
        if(face == Face::FACE_TOP){
            return new float[32]{//0.0f, 1.0f, 0.0f,
                color, color, color, x + leftBound, y + rightBound, z + rightBound, texCoords[9], texCoords[8],
                color, color, color, x + rightBound, y + rightBound, z + rightBound, texCoords[9] + increment, texCoords[8],
                color, color, color, x + rightBound, y + rightBound, z + leftBound, texCoords[9] + increment, texCoords[8] + increment,
                color, color, color, x + leftBound, y + rightBound, z + leftBound, texCoords[9], texCoords[8] + increment};
        }else if(face == Face::FACE_FRONT){
            return new float[32]{//0.0f, 0.0f, 1.0f,
                color, color, color, x + leftBound, y + leftBound, z + rightBound, texCoords[1], texCoords[0],
                color, color, color, x + rightBound, y + leftBound, z + rightBound, texCoords[1] + increment, texCoords[0],
                color, color, color, x + rightBound, y + rightBound, z + rightBound, texCoords[1] + increment, texCoords[0] + increment,
                color, color, color, x + leftBound, y + rightBound, z + rightBound, texCoords[1], texCoords[0] + increment};
        }else if(face == Face::FACE_RIGHT){
            return new float[32]{//1.0f, 0.0f, 0.0f,
                color, color, color, x + rightBound, y + leftBound, z + rightBound, texCoords[7], texCoords[6],
                color, color, color, x + rightBound, y + leftBound, z + leftBound, texCoords[7] + increment, texCoords[6],
                color, color, color, x + rightBound, y + rightBound, z + leftBound, texCoords[7] + increment, texCoords[6] + increment,
                color, color, color, x + rightBound, y + rightBound, z + rightBound, texCoords[7], texCoords[6] + increment};
        }else if(face == Face::FACE_LEFT){
            return new float[32]{//-1.0f, 0.0f, 0.0f,
                color, color, color, x + leftBound, y + leftBound, z + leftBound, texCoords[3], texCoords[2],
                color, color, color, x + leftBound, y + leftBound, z + rightBound, texCoords[3] + increment, texCoords[2],
                color, color, color, x + leftBound, y + rightBound, z + rightBound, texCoords[3] + increment, texCoords[2] + increment,
                color, color, color, x + leftBound, y + rightBound, z + leftBound, texCoords[3], texCoords[2] + increment};
        }else if(face == Face::FACE_BOTTOM){
            return new float[32]{//0.0f, -1.0f, 0.0f,
                color, color, color, x + leftBound, y + leftBound, z + rightBound, texCoords[11], texCoords[10],
                color, color, color, x + rightBound, y + leftBound, z + rightBound, texCoords[11] + increment, texCoords[10],
                color, color, color, x + rightBound, y + leftBound, z + leftBound, texCoords[11] + increment, texCoords[10] + increment,
                color, color, color, x + leftBound, y + leftBound, z + leftBound, texCoords[11], texCoords[10] + increment};
        }else if(face == Face::FACE_BACK){
            return new float[32]{//0.0f, 0.0f, -1.0f,
                color, color, color, x + rightBound, y + leftBound, z + leftBound, texCoords[5], texCoords[4],
                color, color, color, x + leftBound, y + leftBound, z + leftBound, texCoords[5] + increment, texCoords[4],
                color, color, color, x + leftBound, y + rightBound, z + leftBound, texCoords[5] + increment, texCoords[4] + increment,
                color, color, color, x + rightBound, y + rightBound, z + leftBound, texCoords[5], texCoords[4] + increment};
        }
        return 0;
    }

}
