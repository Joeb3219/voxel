#include <SFML/OpenGL.hpp>
#include <string>
#include <vector>
#include "lib/fastNoise/FastNoise.h"

#define WORLD_HEIGHT 128
#define REGION_SIZE 16

namespace VOX_World{

    enum BlockIds{AIR = 0, GRASS = 1, DIRT = 2};

    enum Biome{GRASSLAND};

    Biome getBiome(double elevation, double moisture);

    class Block{
    public:
        Block(const char *str);
        Block();
        int id, meta;
        std::string name;
        bool visible;
        GLuint texture;
    };

    class Region{
    private:
        float xOffset, zOffset;
        int DL_ID;
        void buildDisplayList();
    public:
        bool needsUpdate;
        Region(float xOffset, float zOffset, FastNoise *height, FastNoise *moisture);
        Biome biome;
        Block blocks[WORLD_HEIGHT][REGION_SIZE * REGION_SIZE];
        void render();
        void update();
        bool isBlockVisible(int x, int y, int z);
    };

    class World{
    private:
        std::vector<Region*> regions;
    public:
        static std::vector<Block> blocks;
        World(int seed = 0);
        ~World();
        void update();
        void render();
    };

}