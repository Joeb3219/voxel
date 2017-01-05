#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <vector>
#include "fileIO.h"
#include "inventory.h"
#include "lib/fastNoise/FastNoise.h"

#define WORLD_HEIGHT 128
#define REGION_SIZE 16
#define TYPICAL_GROUND 48
#define NUM_REGIONS_LOADED 26

namespace VOX_World{

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
        Region **regionList;
        void convertCoordinates(float *x, float *y, float *z);
    public:
        bool needsUpdate;
        int xOffset, zOffset;
        Region(float xOffset, float zOffset, FastNoise *height, FastNoise *moisture, FastNoise *density, Region **regionsList);
        Region(float xOffset, float zOffset, FILE *file, Region **regions);
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
        Region **regions;
        FastNoise *height, *moisture, *density;
    public:
        World(int seed = 0);
        ~World();
        Region* getRegion(float x, float y, float z);
        Block getBlock(unsigned short identifier);
        unsigned short getBlock(float x, float y, float z, bool data = true);
        void setBlock(float x, float y, float z, unsigned short blockData);
        sf::Vector3f getCollision(sf::Vector3f start, sf::Vector3f end);
        Region* loadRegion(int x, int z);
        void update();
        void render();
    };

    class Mob{
    private:
        float x = 0, y = 0, z = 0;
    public:
        virtual sf::Vector3f getPosition() = 0;
        virtual void update() = 0;
        virtual void render() = 0;
    };

    class Player : public Mob{
    private:
        float x = 0, y = 0, z = 0, rX = 0, rY = 0, rZ = 0;
        float yVelocity = 0;
        int tickCounter = 0;
        VOX_Inventory::Inventory *inventory = 0;
        World *world = 0;
        float moveSpeed = 0.05f;
        void checkMovement(float *x, float *z);
    public:
        Player(World *world, float x, float y, float z);
        ~Player();
        sf::Vector3f getPosition();
        sf::Vector3f getViewAngles();
        void setMouseChange(sf::Vector2i change);
        sf::Vector3f getLookingAt(bool adjacent = false);
        void update();
        void render();
        void renderInventory(float width);
    };

}
