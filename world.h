#include <SFML/OpenGL.hpp>
#include <string>

namespace VOX_World{

    class Block{
    public:
        Block(const char *str);
        int id, meta;
        const char *name;
        GLuint texture;
    };

    class Region{
    private:
        float x, z;
    public:
    //    void render();
    };

    class World{
    private:
//        Region *regions;
    public:
    //    World(int seed);
    //    void render();
    };

}
