#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <vector>
#include "lib/fastNoise/FastNoise.h"

#define WORLD_HEIGHT 128
#define REGION_SIZE 64
#define TYPICAL_GROUND 48

namespace VOX_World{

    enum BlockIds{AIR = 0, GRASS = 1, DIRT = 2, STONE = 3, SAND = 4, GRAVEL = 5, WOOD = 6, GOLD = 7,
        IRON = 8, COAL = 9, DIAMOND = 10, REDSTONE = 11};

    enum Biome{GRASSLAND};

    Biome getBiome(double elevation, double moisture);

    class Block{
    public:
        Block(const char *str);
        Block();
        int id = -1, meta = 0;
        std::string name;
        bool visible = false, solid = false;
        float texCoords[12] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    };

    extern Block *blocks;

    class Region{
    private:
        int DL_ID = 0;
        void buildDisplayList();
    public:
        bool needsUpdate;
        int xOffset, zOffset;
        Region(float xOffset, float zOffset, FastNoise *height, FastNoise *moisture);
        Biome biome;
        short blocks[WORLD_HEIGHT][REGION_SIZE * REGION_SIZE];
        void render();
        void update();
        bool isBlockVisible(int x, int y, int z);
    };

    class World{
    private:
        std::vector<Region*> regions;
    public:
        World(int seed = 0);
        ~World();
        Region* getRegion(float x, float y, float z);
        Block getBlock(short identifier);
        short getBlock(float x, float y, float z, bool data = true);
        void setBlock(float x, float y, float z, short blockData);
        sf::Vector3f getCollision(sf::Vector3f start, sf::Vector3f end);
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
        World *world = 0;
        float moveSpeed = 0.05f;
        void checkMovement(float *x, float *z);
    public:
        Player(World *world, float x, float y, float z);
        sf::Vector3f getPosition();
        sf::Vector3f getViewAngles();
        void setMouseChange(sf::Vector2i change);
        sf::Vector3f getLookingAt(bool adjacent = false);
        void update();
        void render();
    };

}
