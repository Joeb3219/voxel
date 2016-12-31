#ifndef RENDERABLE_H_
#define RENDERABLE_H_

#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

namespace VOX_Graphics{

    class Renderable{
    public:
        virtual void render(float x, float y, float z) = 0;
        float x, y;
    };

    class Cube{
    private:
        Cube();
        int DL_ID;
    public:
        static Cube &getInstance();
        void render(float x, float y, float z);
    };

}

#endif
