#ifndef RENDERABLE_H_
#define RENDERABLE_H_

#include <string>
#include <vector>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

namespace VOX_Graphics{

    enum Face{FACE_FRONT = 0, FACE_LEFT = 1, FACE_BACK = 2, FACE_RIGHT = 3, FACE_TOP = 4, FACE_BOTTOM = 5};

    extern GLuint textureAtlas;

    class Renderable{
    public:
        virtual void render(float x, float y, float z) = 0;
        float x, y;
    };

    class Cube{
    private:
        Cube();
    public:
        static Cube &getInstance();
        void render(float x, float y, float z, float *texCoords);
        void renderFace(float x, float y, float z, Face face, float *texCoors);
    };

    void renderString(int x, int y, std::string str);

}

#endif
