#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <vector>
#include "lib/fastNoise/FastNoise.h"

#define WORLD_HEIGHT 128
#define REGION_SIZE 16
#define TYPICAL_GROUND 48

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
        bool visible, solid;
        GLuint texture;
    };

    class Region{
    private:
        int DL_ID;
        void buildDisplayList();
    public:
        bool needsUpdate;
        int xOffset, zOffset;
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
        Region* getRegion(float x, float y, float z);
        Block getBlock(float x, float y, float z);
        void setBlock(float x, float y, float z, Block block);
        sf::Vector3f getCollision(sf::Vector3f start, sf::Vector3f end);
        void update();
        void render();
    };

    class Mob{
    private:
        float x, y, z;
    public:
        virtual sf::Vector3f getPosition() = 0;
        virtual void update() = 0;
        virtual void render() = 0;
    };

    class Player : public Mob{
    private:
        float x, y, z, rX, rY, rZ;
        float yVelocity = 0;
        int tickCounter = 0;
        World *world;
        float moveSpeed = 0.05f;
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
