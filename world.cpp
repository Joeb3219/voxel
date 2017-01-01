#include <iostream>
#include <string>
#include <vector>
#include "world.h"
#include "math.h"
#include "fileIO.h"
#include "lib/fastNoise/FastNoise.h"
#include "renderable.h"

namespace VOX_World{

    std::vector<Block> World::blocks;

    Region::Region(float xOffset, float zOffset, FastNoise *height, FastNoise *moisture){
        this->needsUpdate = true;
        this->xOffset = xOffset * REGION_SIZE;
        this->zOffset = zOffset * REGION_SIZE;
        double heightMap[REGION_SIZE * REGION_SIZE];
        double moistureAverage = 0.0, elevationAverage = 0.0;
        for(int x = 0; x < REGION_SIZE; x ++){
            for(int z = 0; z < REGION_SIZE; z ++){
                heightMap[x*REGION_SIZE + z] = VOX_Math::convertScale(height->GetNoise(x + this->xOffset,z + this->zOffset), -1.0f, 1.f, 0.0f, 1.f);
                elevationAverage += heightMap[x*REGION_SIZE + z];
                moistureAverage += VOX_Math::convertScale(moisture->GetNoise(x + this->xOffset, z + this->zOffset), -1.0f, 1.f, 0.0f, 1.f);
            }
        }

        moistureAverage /= (REGION_SIZE * 1.0 * REGION_SIZE);
        elevationAverage /= (REGION_SIZE * 1.0 * REGION_SIZE);

        this->biome = getBiome(elevationAverage, moistureAverage);
        std::cout << "Moisture average " << moistureAverage << ", Elevation average: " << elevationAverage << std::endl;
        for(int x = 0; x < REGION_SIZE; x ++){
            for(int z = 0; z < REGION_SIZE; z ++){
                for(int y = 0; y < WORLD_HEIGHT; y ++){
                    if(y < heightMap[x*REGION_SIZE + z]*48) blocks[y][(x*REGION_SIZE + z)] = World::blocks.at(BlockIds::GRASS);
                    else blocks[y][(x*REGION_SIZE + z)] = World::blocks.at(BlockIds::AIR);
                }
            }
        }

    }

    void Region::buildDisplayList(){
        DL_ID = glGenLists(1);
        glNewList(DL_ID, GL_COMPILE);

        for(int x = 0; x < REGION_SIZE; x ++){
            for(int z = 0; z < REGION_SIZE; z ++){
                for(int y = 0; y < WORLD_HEIGHT; y ++){
                    float xPrime, yPrime, zPrime;
                    xPrime = x + xOffset;
                    yPrime = y;
                    zPrime = z + zOffset;
                    Block block = blocks[y][(x*REGION_SIZE + z)];
                    if(!isBlockVisible(x, y, z)) continue;
                    glBindTexture(GL_TEXTURE_2D, block.texture);
                    VOX_Graphics::Cube::getInstance().render(xPrime, yPrime, zPrime);
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
            }
        }

        glEndList();
    }

    bool Region::isBlockVisible(int x, int y, int z){
        if(!blocks[y][(x * REGION_SIZE) + z].visible) return false;                         // Check if this is visible
        if(y == 0 || y == WORLD_HEIGHT || x == 0 || x == REGION_SIZE - 1 || z == 0 || z == REGION_SIZE - 1) return true;
        if(!blocks[y + 1][(x * REGION_SIZE) + z].visible) return true;  // Check if above is visible
        if(!blocks[y - 1][(x * REGION_SIZE) + z].visible) return true;             // Check if below is visible
        if(!blocks[y][((x + 1) * REGION_SIZE) + z].visible) return true;// Check if right is visible
        if(!blocks[y][((x - 1) * REGION_SIZE) + z].visible) return true;          // Check if left is visible
        if(!blocks[y][((x) * REGION_SIZE) + z - 1].visible) return true;          // Check if behind is visible
        if(!blocks[y][((x) * REGION_SIZE) + z + 1].visible) return true;// Check if forward is visible
        return false;
    }

    void Region::update(){
        int ind;
        for(int x = 0; x < REGION_SIZE; x ++){
            for(int y = 0; y < WORLD_HEIGHT; y ++){
                for(int z = 0; z < REGION_SIZE; z ++){
                    ind = x*REGION_SIZE + z;
                    if(blocks[y][ind].id == BlockIds::GRASS && blocks[y + 1][ind].visible == true) blocks[y][ind] = World::blocks.at(BlockIds::DIRT);
                }
            }
        }
        buildDisplayList();
        needsUpdate = false;
    }

    void Region::render(){
        if(DL_ID != 0) glCallList(DL_ID);
    }

    Block::Block(){

    }

    Block::Block(const char* str){
        FILE *file = fopen(str, "r");
        if(file == 0){
            std::cout << "Block: File " << str << " does not exist." << std::endl;
            return;
        }
        VOX_FileIO::Tree data(file);
        std::string string_id, string_name, string_texture, string_visible;
        string_id = data.search("block:data:id");
        string_name = data.search("block:data:name");
        string_texture = data.search("block:data:texture");
        string_visible = data.search("block:data:visible");

        if(!string_id.empty()) id = atoi(string_id.c_str());
        if(!string_name.empty()) name = string_name;
        if(!string_texture.empty()) texture = VOX_FileIO::loadBitmapTexture(string_texture.c_str());
        else texture = 0;
        if(!string_visible.empty() && std::string("true").compare(string_visible) == 0) visible = true;
        else visible = false;
    }

    Biome getBiome(double elevation, double moisture){
        return GRASSLAND;
    }

    void World::update(){
        for(unsigned int i = 0; i < regions.size(); i ++){
            if(regions.at(i)->needsUpdate) regions.at(i)->update();
        }
    }

    void World::render(){
        for(unsigned int i = 0; i < regions.size(); i ++){
            regions.at(i)->render();
        }
    }

    World::World(int seed){
        FastNoise height, moisture;
        height.SetNoiseType(FastNoise::SimplexFractal);
        height.SetSeed(seed);
        height.SetFractalOctaves(4);
        moisture.SetNoiseType(FastNoise::SimplexFractal);
        moisture.SetSeed(seed * 2);
        moisture.SetFractalOctaves(8);
        regions.push_back(new Region(0, 0, &height, &moisture));
        regions.push_back(new Region(1, 0, &height, &moisture));
        regions.push_back(new Region(0, 1, &height, &moisture));
        regions.push_back(new Region(1, 1, &height, &moisture));
    }

    World::~World(){
        for(unsigned int i = 0; i < regions.size(); i ++){
            delete regions[i];
        }
    }

}
