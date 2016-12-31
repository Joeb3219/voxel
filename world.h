#include <SFML/OpenGL.hpp>

namespace VOX_World{

    class World{
    private:
    //    Region *regions;
    public:
        World(int seed);
        void render();
    };

    class Region{
    private:
        float x, z;
    public:
        void render();
    };

    class Block{
    private:
    public:
        int id, meta;
        char *name;
        GLuint texture;
    };

}
