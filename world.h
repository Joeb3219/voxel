#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <vector>
#include "lib/fastNoise/FastNoise.h"

#define WORLD_HEIGHT 128
#define REGION_SIZE 16
#define TYPICAL_GROUND 48
#define REGIONS_FROM_PLAYER_LOAD 2
#define NUM_REGIONS_LOADED 25

namespace VOX_Mob{
    class Player;
}

namespace VOX_FileIO{
    class Tree;
}

namespace VOX_Inventory{
    class Inventory;
}

namespace VOX_World{

    class World;

    enum Biome{GRASSLAND};

    Biome getBiome(double elevation, double moisture);

    class Block{
    public:
        Block(VOX_FileIO::Tree *tree, std::string blockPath);
        Block();
        int id = -1, meta = 0, drops, damage;
        std::string name;
        bool visible = false, solid = false;
        float texCoords[12] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    };

    extern Block *blocks;

    class Region{
    private:
        int DL_ID = 0;
        void buildDisplayList();
        void convertCoordinates(float *x, float *y, float *z);
        World *world;
    public:
        bool needsUpdate;
        int xOffset, zOffset;
        Region(World *world, float xOffset, float zOffset);
        Region(World *world, float xOffset, float zOffset, FILE *file);
        Region();
        ~Region();
        Biome biome;
        void modifyMeta(float x, float y, float z, unsigned short newMeta, bool correctCoords = false);
        void setBlock(int x, int y, int z, int blockID);
        unsigned short getBlock(int x, int y, int z);
        unsigned short blocks[WORLD_HEIGHT][REGION_SIZE * REGION_SIZE];
        bool isInRegion(float x, float y, float z);
        bool checkSurroundingsIsVisible(int x, int y, int z);
        void render();
        void update();
    };

    class World{
    private:
        VOX_Mob::Player *player;
        Region **regions;
    public:
        FastNoise *height, *moisture, *density;
        World(int seed = 0);
        ~World();
        void setPlayer(VOX_Mob::Player *player);
        Region* getRegion(float x, float y, float z);
        Block getBlock(unsigned short identifier);
        unsigned short getBlock(float x, float y, float z, bool data = true);
        void setBlock(float x, float y, float z, unsigned short blockData);
        sf::Vector3f getCollision(sf::Vector3f start, sf::Vector3f end);
        Region* loadRegion(int x, int z);
        void pruneRegions();
        void update();
        void render();
    };

}
