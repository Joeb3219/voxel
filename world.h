#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <unordered_map>
#include "lib/fastNoise/FastNoise.h"

#define WORLD_HEIGHT 128
#define REGION_SIZE 16
#define TYPICAL_GROUND 48
#define REGIONS_FROM_PLAYER_LOAD 2
#define REGIONS_FROM_PLAYER_RENDER 2
#define REGIONS_FROM_PLAYER_UNLOAD 3
#define NUM_FLOATS_PER_FACE 20

#define IS_SOLID(x, y, z) VOX_World::blocks[(int)world->getBlock(x, y, z)->id].solid
#define BLOCK_NAME(x, y, z) VOX_World::blocks[(int)world->getBlock(x, y, z)->id].name

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

    typedef struct BlockData{
        // We order the data as lighting|other|meta|id.
        char lighting, other, meta, id;
    } BlockData;

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
        void buildDisplayList();
        World *world;
        bool faceBuildingThreadSpawned = false;
        float *vertexArray = 0; // This is an interleved array. We store in the following order: n1n2n3 v1v2v3t1t2 v1v2v3t1t2 v1v2v3t1t2 v1v2v3t1t2
        void spawnRegion();
        void loadRegionFromMemory(FILE *file);
    public:
        int numFacesDrawn = 0;
        bool loaded = false;
        bool needsUpdate, updatingMesh = false;
        int xOffset, zOffset;
        Region(World *world, float xOffset, float zOffset);
        Region(World *world, float xOffset, float zOffset, FILE *file);
        Region();
        ~Region();
        Biome biome;
        void generateDisplayedFaces();
        void convertCoordinates(float *x, float *y, float *z, bool toWorld = false);
        void setBlock(int x, int y, int z, int blockID);
        BlockData* getBlock(int x, int y, int z);
        BlockData* blocks[WORLD_HEIGHT][REGION_SIZE * REGION_SIZE];
        bool isInRegion(float x, float y, float z);
        bool checkSurroundingsIsVisible(float x, float y, float z);
        void render();
        void update();
    };

    class World{
    private:
        VOX_Mob::Player *player;
        std::unordered_map<std::string, Region*> *regionMap;
    public:
        int facesRendered = 0;
        FastNoise *height, *moisture, *density;
        World(int seed = 0);
        ~World();
        void setPlayer(VOX_Mob::Player *player);
        Region* getRegion(float x, float y, float z);
        Block getBlock(unsigned int identifier);
        BlockData* getBlock(float x, float y, float z);
        void setBlock(float x, float y, float z, unsigned int blockData);
        sf::Vector3f getCollision(sf::Vector3f start, sf::Vector3f end);
        Region* loadRegion(int x, int z);
        void pruneRegions();
        void update();
        void render();
    };

}
