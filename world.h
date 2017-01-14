#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>
#include "lib/fastNoise/FastNoise.h"

#define WORLD_HEIGHT 128
#define REGION_SIZE 16
#define TYPICAL_GROUND 48
#define REGIONS_FROM_PLAYER_LOAD 2
#define REGIONS_FROM_PLAYER_RENDER 2

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
        World *world;
        unsigned char displayedFaces[WORLD_HEIGHT][REGION_SIZE*REGION_SIZE];
        bool readyToBuildMesh = false, faceBuildingThreadSpawned = false;
    public:
        bool needsUpdate, updatingMesh = false;
        int xOffset, zOffset;
        Region(World *world, float xOffset, float zOffset);
        Region(World *world, float xOffset, float zOffset, FILE *file);
        Region();
        ~Region();
        Biome biome;
        void generateDisplayedFaces();
        void convertCoordinates(float *x, float *y, float *z, bool toWorld = false);
        void modifyMeta(float x, float y, float z, unsigned short newMeta, bool correctCoords = false);
        void setBlock(int x, int y, int z, int blockID);
        unsigned short getBlock(int x, int y, int z, bool data = false);
        unsigned short blocks[WORLD_HEIGHT][REGION_SIZE * REGION_SIZE];
        bool isInRegion(float x, float y, float z);
        bool checkSurroundingsIsVisible(float x, float y, float z);
        void render();
        void update();
    };

    class World{
    private:
        VOX_Mob::Player *player;
        std::unordered_map<std::string, Region*> *regionMap;
        std::vector<sf::Vector2i> *regionsLoadingQueue;
        std::vector<Region*> *regionsLoadedQueue;
        std::thread *regionLoadingThread;
        std::mutex regionLoadingLock;
        int regionLoadingThreadTickTracker = 0;
        bool regionLoadingThreadRunning = true;
        void thread_loadRegions(bool *running, int *tickTracker);
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
