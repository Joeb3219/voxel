#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <ctime>
#include "lib/fastNoise/FastNoise.h"
#include "renderable.h"
#include "math.h"

namespace VOX_Graphics{

    GLuint Ground::generateDL(){
        GLuint terrainDL;
    	int i, j, ind;
        float xOffset = 0, zOffset = 0;

    	terrainDL = glGenLists(1);

    	glNewList(terrainDL,GL_COMPILE);

        for (j = 0; j < size; j++) {

    		glBegin(GL_TRIANGLE_STRIP);
            for (i = 0 ; i < size-1; i++) {
    			ind = 3*((i)*size + j + 1);
    			glColor3f(terrainColors[ind], terrainColors[ind+1], terrainColors[ind+2]);
    			glNormal3f(terrainNormals[ind], terrainNormals[ind+1], terrainNormals[ind+2]);
    			glVertex3f(i + xOffset, terrainHeights[(i) * size + (j + 1)], (j+1) + zOffset);
    			ind = 3*(i * size + j);
    			glColor3f(terrainColors[ind], terrainColors[ind+1], terrainColors[ind+2]);
    			glNormal3f(terrainNormals[ind], terrainNormals[ind+1], terrainNormals[ind+2]);
    			glVertex3f(i + xOffset, terrainHeights[(i) * size + j], j + zOffset);
    		}
    		glEnd();
    	}
    	glEndList();

    	return terrainDL;
    }

    Ground::~Ground(){
        delete [] terrainHeights;
        delete [] terrainColors;
        delete [] terrainNormals;
    }

    Ground::Ground(int size, int seed){
        this->size = size;
        terrainHeights = new double [size * size];
        terrainColors = new double [3 * size * size];
        terrainNormals = new double [3 * size * size];

        FastNoise myNoise;
        myNoise.SetNoiseType(FastNoise::SimplexFractal);
        myNoise.SetSeed(seed);
        myNoise.SetFractalOctaves(2);

        double a, b;
        for(a = 0; a < size; a ++){
            for(b = 0; b < size; b ++){
                int ind = (int) (a*size + b);
                double noise = VOX_Math::convertScale(myNoise.GetNoise(a,b), -1.0f, 1.f, 0.0f, 1.f);   // Convert noise from b/w -1 -> 1 to 0 -> 1.
                terrainHeights[ind] = noise * 80.0;
            }
        }
        generateColorsAndDisplay();
    }

    void Ground::generateColorsAndDisplay(){
        std::cout << "Regenerating colors and display code" << std::endl;
        int colorMode = 1; // 0 = B&W, 1 = beach.
        int smoothing = 1; // 1 = Smooth, 0 = no smoothing.
        int color = 0xFFFFFF;
        for(int x = 0; x < size; x ++){
            for(int y = 0; y < size; y ++){
                int ind = x * size + y;
                double noise = terrainHeights[ind] / 80.0;
                double r, g, b;
                color = 0xFFFFFF;
                if(colorMode == 0){
                    color *= noise;
                    r = ((color & 0xFF0000) >> 16);// * 0.30;
                    g = ((color & 0x00FF00) >> 8);// * 0.59;
                    b = ((color & 0x0000FF));// * 0.11;

                    color = sqrt((r * r + g * g + b * b) / 3.0);

                    terrainColors[3*ind + 0] = terrainColors[3*ind + 1] = terrainColors[3*ind + 2] = (0xFF - color) / 255.0f;
                }else{
                    color *= noise;

                    if(noise <= 0.5){
                        color = 0x40A4DF;
                        r = ((color & 0xFF0000) >> 16) * 1.0 * (1 - VOX_Math::convertScale(noise, 0.f, 0.5f, 0.f, 0.5f));
                        g = ((color & 0x00FF00) >> 8) * 1.0 * (1 - VOX_Math::convertScale(noise, 0.f, 0.5f, 0.f, 0.5f));
                        b = ((color & 0x0000FF)) * 1.0 * (1 - VOX_Math::convertScale(noise, 0.f, 0.5f, 0.f, 0.5f));
                        color = (((int)r) << 16) + (((int)g) << 8) + ((int)b);
                    }else{
                        color = 0xD3EF6F;
                        r = ((color & 0xFF0000) >> 16) * 1.0 * (1 - VOX_Math::convertScale(noise, 0.5f, 1.f, 0.f, 1.f));
                        g = ((color & 0x00FF00) >> 8) * 1.0 * (1 - VOX_Math::convertScale(noise, 0.5f, 1.f, 0.f, 1.f));
                        b = ((color & 0x0000FF)) * 1.0 * (1 - VOX_Math::convertScale(noise, 0.5f, 1.f, 0.f, 1.f));
                        color = (((int)r) << 16) + (((int)g) << 8) + ((int)b);
                    }

                    r = ((color & 0xFF0000) >> 16);
                    g = ((color & 0x00FF00) >> 8);
                    b = ((color & 0x0000FF));

                    terrainColors[3*ind + 0] = r / 255.0f;
                    terrainColors[3*ind + 1] = g / 255.0f;
                    terrainColors[3*ind + 2] = b / 255.0f;
                }
            }
        }

        if(smoothing){
            for(int x = 1; x < size - 1; x ++){
                for(int z = 1; z < size - 1; z ++ ){
                    int ind = 3*(x * size + z);
                    float lWeight = 1.0, rWeight = 1.0, aWeight = 1.0, bWeight = 1.0, meWeight = 2.0;
                    float weightSums = lWeight + rWeight + aWeight + bWeight + meWeight;
                    float r, g, b;
                    r = terrainColors[ind + 0] *  meWeight +
                        terrainColors[3*((x + 1)*size + z) + 0] * bWeight +
                        terrainColors[3*((x - 1)*size + z) + 0] * aWeight +
                        terrainColors[3*((x)*size + z + 1) + 0] * rWeight +
                        terrainColors[3*((x)*size + z - 1) + 0] * lWeight;
                    g = terrainColors[ind + 1] *  meWeight +
                        terrainColors[3*((x + 1)*size + z) + 1] * bWeight +
                        terrainColors[3*((x - 1)*size + z) + 1] * aWeight +
                        terrainColors[3*((x)*size + z + 1) + 1] * rWeight +
                        terrainColors[3*((x)*size + z - 1) + 1] * lWeight;
                    b = terrainColors[ind + 2] *  meWeight +
                        terrainColors[3*((x + 1)*size + z) + 2] * bWeight +
                        terrainColors[3*((x - 1)*size + z) + 2] * aWeight +
                        terrainColors[3*((x)*size + z + 1) + 2] * rWeight +
                        terrainColors[3*((x)*size + z - 1) + 2] * lWeight;
                        terrainColors[ind + 0] = r / weightSums;
                        terrainColors[ind + 1] = g / weightSums;
                        terrainColors[ind + 2] = b / weightSums;
                }
            }
        }

        DL_ID = generateDL();
    }

    double Ground::getHeight(float x, float z){
        int xPrime = (int) x;
        int zPrime = (int) z;
        if(xPrime < 0 || xPrime > size || zPrime < 0 || zPrime > size) return -1;
        return terrainHeights[xPrime * size + zPrime];
    }

    void Ground::modifyHeight(float x, float z, float val){
        int xPrime = (int) x;
        int zPrime = (int) z;
        terrainHeights[xPrime * size + zPrime] += val;
        std::cout << "Set height of " << xPrime << ", " << zPrime << " to " << terrainHeights[xPrime * size + zPrime] << std::endl;
    }

    void Ground::render(){
        glPushMatrix();
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);
        glCallList(DL_ID);
        glDisable(GL_DEPTH_TEST);
        glPopMatrix();
    }

}
