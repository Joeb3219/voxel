#ifndef RENDERABLE_H_
#define RENDERABLE_H_

#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

namespace VOX_Graphics{

    class Renderable{
    public:
        virtual void render() = 0;
        float x, y;
    };

    class Ground : public Renderable{
    private:
        double *terrainHeights;
        double *terrainNormals;
        double *terrainColors;
        GLuint DL_ID;
        GLuint generateDL();
        int size;
    public:
        Ground(int size, int seed);
        double getHeight(float x, float z);
        void modifyHeight(float x, float z, float val);
        void generateColorsAndDisplay();
        ~Ground();
        void render();
        float x, y;
    };
}

#endif
