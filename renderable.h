#ifndef RENDERABLE_H_
#define RENDERABLE_H_

#include <string>
#include <vector>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#define NUM_SYMBOLS 58

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

    class Text{
    private:
        static GLuint letterDisplayLists;
        GLuint fullTextmap;
        void renderLetter(char c, float x, float y, float size = 1.0f);
        Text();
    public:
        void renderString(std::string str, float x, float y, float size = 1.0f);
        static Text &getInstance();
    };

}

#endif
