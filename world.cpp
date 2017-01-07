#include <iostream>
#include <string>
#include <vector>
#include "world.h"
#include "mob.h"
#include "math.h"
#include "fileIO.h"
#include "lib/fastNoise/FastNoise.h"
#include "renderable.h"
#include "inventory.h"
#include "camera.h"

namespace VOX_World{

    Block *blocks;

    Region::Region(){}

    Region::Region(World *world, float xOffset, float zOffset){
        this->world = world;
        this->needsUpdate = true;
        this->xOffset = xOffset * REGION_SIZE;
        this->zOffset = zOffset * REGION_SIZE;
        double heightMap[REGION_SIZE * REGION_SIZE];
        double moistureAverage = 0.0, elevationAverage = 0.0;
        for(int x = 0; x < REGION_SIZE; x ++){
            for(int z = 0; z < REGION_SIZE; z ++){
                heightMap[x*REGION_SIZE + z] = VOX_Math::convertScale(world->height->GetNoise(x + this->xOffset,z + this->zOffset), -1.0f, 1.f, 0.0f, 1.f);
                elevationAverage += heightMap[x*REGION_SIZE + z];
                moistureAverage += VOX_Math::convertScale(world->moisture->GetNoise(x + this->xOffset, z + this->zOffset), -1.0f, 1.f, 0.0f, 1.f);
            }
        }

        moistureAverage /= (REGION_SIZE * 1.0 * REGION_SIZE);
        elevationAverage /= (REGION_SIZE * 1.0 * REGION_SIZE);

        this->biome = getBiome(elevationAverage, moistureAverage);
        std::cout << "Moisture average " << moistureAverage << ", Elevation average: " << elevationAverage << std::endl;
        for(int x = 0; x < REGION_SIZE; x ++){
            for(int z = 0; z < REGION_SIZE; z ++){
                int totalHeight = (heightMap[x*REGION_SIZE + z])*TYPICAL_GROUND + TYPICAL_GROUND;
                for(int y = 0; y < WORLD_HEIGHT; y ++){
                    /*if(xOffset < 0 || zOffset < 0){
                        if(x == 0 && z == 0) setBlock(x, y, z, VOX_Inventory::BlockIds::DIAMOND);
                        else setBlock(x, y, z, VOX_Inventory::BlockIds::AIR);
                        continue;
                    }*/
                    float currentDensity = VOX_Math::convertScale(world->density->GetNoise(x * 1.0f + this->xOffset, y * 1.0f, z * 1.0f + this->zOffset), -1.0f, 1.0f, -0.05f, 1.0f);
                    if(y >= totalHeight || currentDensity <= 0.04f){
                        setBlock(x, y, z, VOX_Inventory::BlockIds::AIR);
                        continue;
                    }
                    if(y >= TYPICAL_GROUND){
                        if(currentDensity <= 0.75f) setBlock(x, y, z, VOX_Inventory::BlockIds::GRASS);
                        else setBlock(x, y, z, VOX_Inventory::BlockIds::STONE);
                    }else if(y >= 58){
                        if(currentDensity <= 0.45f) setBlock(x, y, z, VOX_Inventory::BlockIds::GRASS);
                        else if(currentDensity <= 0.55f) setBlock(x, y, z, VOX_Inventory::BlockIds::GRAVEL);
                        else setBlock(x, y, z, VOX_Inventory::BlockIds::STONE);
                    }else if(y >= 50){
                        if(currentDensity <= 0.15f) setBlock(x, y, z, VOX_Inventory::BlockIds::DIRT);
                        else if(currentDensity <= 0.30f) setBlock(x, y, z, VOX_Inventory::BlockIds::GRAVEL);
                        else if(currentDensity <= 0.45f) setBlock(x, y, z, VOX_Inventory::BlockIds::COAL);
                        else setBlock(x, y, z, VOX_Inventory::BlockIds::STONE);
                    }else if(y >= 40){
                        if(currentDensity <= 0.09f) setBlock(x, y, z, VOX_Inventory::BlockIds::DIRT);
                        else if(currentDensity <= 0.15f) setBlock(x, y, z, VOX_Inventory::BlockIds::GRAVEL);
                        else if(currentDensity <= 0.30f) setBlock(x, y, z, VOX_Inventory::BlockIds::COAL);
                        else if(currentDensity <= 0.41f) setBlock(x, y, z, VOX_Inventory::BlockIds::IRON);
                        else setBlock(x, y, z, VOX_Inventory::BlockIds::STONE);
                    }else if(y >= 30){
                        if(currentDensity <= 0.10f) setBlock(x, y, z, VOX_Inventory::BlockIds::DIRT);
                        else if(currentDensity <= 0.20f) setBlock(x, y, z, VOX_Inventory::BlockIds::GRAVEL);
                        else if(currentDensity <= 0.32f) setBlock(x, y, z, VOX_Inventory::BlockIds::COAL);
                        else if(currentDensity <= 0.46f) setBlock(x, y, z, VOX_Inventory::BlockIds::IRON);
                        else setBlock(x, y, z, VOX_Inventory::BlockIds::STONE);
                    }else if(y >= 16){
                        if(currentDensity <= 0.12f) setBlock(x, y, z, VOX_Inventory::BlockIds::DIRT);
                        else if(currentDensity <= 0.15f) setBlock(x, y, z, VOX_Inventory::BlockIds::GRAVEL);
                        else if(currentDensity <= 0.29f) setBlock(x, y, z, VOX_Inventory::BlockIds::COAL);
                        else if(currentDensity <= 0.34f) setBlock(x, y, z, VOX_Inventory::BlockIds::GOLD);
                        else if(currentDensity <= 0.40f) setBlock(x, y, z, VOX_Inventory::BlockIds::REDSTONE);
                        else if(currentDensity <= 0.47f) setBlock(x, y, z, VOX_Inventory::BlockIds::IRON);
                        else setBlock(x, y, z, VOX_Inventory::BlockIds::STONE);
                    }else{
                        if(currentDensity <= 0.05) setBlock(x, y, z, VOX_Inventory::BlockIds::DIRT);
                        else if(currentDensity <= 0.12f) setBlock(x, y, z, VOX_Inventory::BlockIds::GRAVEL);
                        else if(currentDensity <= 0.16f) setBlock(x, y, z, VOX_Inventory::BlockIds::DIAMOND);
                        else if(currentDensity <= 0.22f) setBlock(x, y, z, VOX_Inventory::BlockIds::GOLD);
                        else if(currentDensity <= 0.24f) setBlock(x, y, z, VOX_Inventory::BlockIds::REDSTONE);
                        else if(currentDensity <= 0.35f) setBlock(x, y, z, VOX_Inventory::BlockIds::COAL);
                        else if(currentDensity <= 0.45f) setBlock(x, y, z, VOX_Inventory::BlockIds::IRON);
                        else setBlock(x, y, z, VOX_Inventory::BlockIds::STONE);
                    }
                }
            }
        }

    }

    Region::Region(World *world, float xOffset, float zOffset, FILE *file){
        this->world = world;
        this->xOffset = xOffset * REGION_SIZE;
        this->zOffset = zOffset * REGION_SIZE;
        unsigned short block;
        char c;
        for(int x = 0; x < REGION_SIZE; x ++){
            for(int z = 0; z < REGION_SIZE; z ++){
                for(int y = 0; y < WORLD_HEIGHT; y ++){
                    c = fgetc(file);
                    block = (c << 8);
                    c = fgetc(file);
                    block |= c;
                    blocks[y][x*REGION_SIZE + z] = block;
                }
            }
        }
        this->needsUpdate = true;
    }

    // Destroying a region will save it to saves/xOffet:zOffset.txt
    Region::~Region(){
        std::string fileName("saves/");
        fileName += std::to_string(xOffset) + std::string(":") + std::to_string(zOffset) + std::string(".txt");
        FILE *file = fopen(fileName.c_str(), "w+");
        unsigned short block;

        for(int x = 0; x < REGION_SIZE; x ++){
            for(int z = 0; z < REGION_SIZE; z ++){
                for(int y = 0; y < WORLD_HEIGHT; y ++){
                    block = blocks[y][x*REGION_SIZE + z];
                    fputc((block & 0xFF00) >> 8, file);
                    fputc((block & 0x00FF), file);
                }
            }
        }

        fclose(file);
    }

    bool Region::isInRegion(float x, float y, float z){
        if(y > WORLD_HEIGHT) return false;
        int xPrime = abs(x);
        int zPrime = abs(z);
        int xRegion = ((int)(((xPrime + REGION_SIZE) / REGION_SIZE) * REGION_SIZE) - REGION_SIZE);
        int zRegion = ((int)(((zPrime + REGION_SIZE) / REGION_SIZE) * REGION_SIZE) - REGION_SIZE);
        if(x < 0) xRegion = -xRegion - REGION_SIZE;
        if(z < 0) zRegion = -zRegion - REGION_SIZE;
        if(xRegion == this->xOffset && zRegion == this->zOffset) return true;
        return false;
    }

    unsigned short Region::getBlock(int x, int y, int z, bool data){
        if(!data) return blocks[y][x*REGION_SIZE + z] & 0x00FF;
        return blocks[y][x*REGION_SIZE + z];
    }

    void Region::setBlock(int x, int y, int z, int blockID){
        int meta = VOX_World::blocks[blockID].damage;
        blocks[y][x*REGION_SIZE + z] = (((meta & 0x00FF) << 8) | (blockID & 0x00FF));
    }

    void Region::convertCoordinates(float *x, float *y, float *z){
        int xPrime = abs(*x);
        int zPrime = abs(*z);
        if( (*x) < 0) (*x) = REGION_SIZE - (REGION_SIZE + xPrime - abs(this->xOffset)) - 1;
        else (*x) = (xPrime - abs(this->xOffset));
        if( (*z) < 0) (*z) = REGION_SIZE - (REGION_SIZE + zPrime - abs(this->zOffset)) - 1;
        else (*z) = (zPrime - abs(this->zOffset));
    }

    void Region::modifyMeta(float x, float y, float z, unsigned short newMeta, bool correctCoords){
        if(!correctCoords) convertCoordinates(&x, &y, &z);
        unsigned short id = blocks[(int)y][(int)x*REGION_SIZE + (int)z];
        blocks[(int)y][(int)x*REGION_SIZE + (int)z] = (newMeta << 8) | (id & 0x00FF);
    }

    void Region::buildDisplayList(){
        if(DL_ID != 0) glDeleteLists(DL_ID, 1);
        DL_ID = glGenLists(1);
        glNewList(DL_ID, GL_COMPILE);
        glBindTexture(GL_TEXTURE_2D, VOX_Graphics::textureAtlas);
        VOX_Graphics::Cube cube = VOX_Graphics::Cube::getInstance();
        float xPrime, yPrime, zPrime, *texCoords;
        for(int i = 0; i < 256; i ++){
            if(&VOX_World::blocks[i] == 0 || !VOX_World::blocks[i].visible) continue;   // A null pointer or invisible
            texCoords = &VOX_World::blocks[i].texCoords[0];
            for(int x = 0; x < REGION_SIZE; x ++){
                for(int z = 0; z < REGION_SIZE; z ++){
                    for(int y = 0; y < WORLD_HEIGHT; y ++){
                        xPrime = x + xOffset;
                        yPrime = y;
                        zPrime = z + zOffset;
                        if((getBlock(x, y, z)) != i) continue;
                        if(!checkSurroundingsIsVisible(x, y + 1, z)) cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_TOP, texCoords);
                        if(!checkSurroundingsIsVisible(x, y - 1, z)) cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_BOTTOM, texCoords);
                        if(!checkSurroundingsIsVisible(x + 1, y, z)) cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_RIGHT, texCoords);
                        if(!checkSurroundingsIsVisible(x - 1, y, z)) cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_LEFT, texCoords);
                        if(!checkSurroundingsIsVisible(x, y, z + 1)) cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_FRONT, texCoords);
                        if(!checkSurroundingsIsVisible(x, y, z - 1)) cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_BACK, texCoords);
                    }
                }
            }
        }

        glEndList();
    }

    bool Region::checkSurroundingsIsVisible(int x, int y, int z){
        float xPrime = x + this->xOffset;
        float yPrime = y;
        float zPrime = z + this->zOffset;
        if(y <= 0 || y >= WORLD_HEIGHT - 1) return false;
        if((x >= 0 && x < REGION_SIZE - 1) && (z >= 0 && z < REGION_SIZE - 1)) return VOX_World::blocks[getBlock(x, y, z)].visible;
        else{
            return VOX_World::blocks[world->getBlock(xPrime, yPrime, zPrime, false)].visible;
        }
        return false;
    }

    void Region::update(){
        if(needsUpdate){
            for(int x = 0; x < REGION_SIZE; x ++){
                for(int y = 0; y < WORLD_HEIGHT; y ++){
                    for(int z = 0; z < REGION_SIZE; z ++){
                        if(getBlock(x, y, z) == VOX_Inventory::BlockIds::GRASS && VOX_World::blocks[getBlock(x, y + 1, z)].visible == true) setBlock(x, y, z, VOX_Inventory::BlockIds::DIRT);
                    }
                }
            }
        }
        buildDisplayList();
        needsUpdate = false;
    }

    void Region::render(){
        glPushMatrix();
        if(DL_ID != 0) glCallList(DL_ID);
        glPopMatrix();
    }

    Block::Block(){

    }

    Block::Block(VOX_FileIO::Tree *tree, std::string blockPath){
        std::string string_id, string_name, string_visible, string_solid, string_damage, string_drops;
        std::string string_texture;
        string_id = tree->search(blockPath + ":id");
        string_name = tree->search(blockPath + ":name");
        string_visible = tree->search(blockPath + ":visible");
        string_solid = tree->search(blockPath + ":solid");
        string_texture = tree->search(blockPath + ":texture");
        string_damage = tree->search(blockPath + ":damage");
        string_drops = tree->search(blockPath + ":drops");

        if(!string_id.empty()) id = atoi(string_id.c_str());
        if(!string_name.empty()) name = string_name;
        if(!string_visible.empty() && std::string("true").compare(string_visible) == 0) visible = true;
        else visible = false;
        if(!string_solid.empty() && std::string("true").compare(string_solid) == 0) solid = true;
        else solid = false;
        if(!string_damage.empty()) damage = atoi(string_damage.c_str());
        else damage = 0xFF;
        if(!string_drops.empty()) drops = atoi(string_drops.c_str());
        else drops = id;

        if(!string_texture.empty()){
            std::string currNum("");
            char c;
            int j = 0;
            for(unsigned int i = 0; i < string_texture.size(); i ++){
                c = string_texture.at(i);
                if(c == ':' || c == ','){
                    texCoords[j++] = atoi(currNum.c_str());
                    currNum = std::string("");
                }else currNum += c;
            }
            texCoords[j++] = atoi(currNum.c_str());
            for(int i = 0; i < 12; i ++){
                if(i % 2 == 1) texCoords[i] = (1.0f / 32.0f) * texCoords[i];
                else texCoords[i] = 1.0f - ((1.0f / 32.0f) * (texCoords[i] + 1));
            }
        }
    }

    Biome getBiome(double elevation, double moisture){
        return GRASSLAND;
    }

    void World::update(){
        for(int i = 0; i < NUM_REGIONS_LOADED; i ++){
            if(regions[i] != 0 && regions[i]->needsUpdate) regions[i]->update();
        }
    }

    void World::render(){
        // Frustum culling: Determine which regions are within frustum.
        //sf::Vector3f currentPos = player->getPosition(), lookingAtLeft, lookingAtRight, angle = player->getViewAngles();
        //currentPos.y += 2.5f;
        //lookingAtLeft = VOX_Math::computeVectorFromPos(currentPos, angle.x - FOV / 2, angle.y - FOV / 2, 1.0f);
        //lookingAtRight = VOX_Math::computeVectorFromPos(currentPos, angle.x + FOV / 2, angle.y + FOV / 2, 1.0f);
        for(int i = 0; i < NUM_REGIONS_LOADED; i ++){

            if(regions[i] != 0) regions[i]->render();
        }
    }

    Region* World::getRegion(float x, float y, float z){
        for(int i = 0; i < NUM_REGIONS_LOADED; i ++){
            if(regions[i] != 0 && regions[i]->isInRegion(x, y, z)) return regions[i];
        }
        return 0;
    }

    Block World::getBlock(unsigned short identifier){
        return blocks[identifier & 0x00FF];
    }

    unsigned short World::getBlock(float x, float y, float z, bool data){
        int xPrime = (int) x;
        int yPrime = (int) y;
        int zPrime = (int) z;
        if(yPrime >= WORLD_HEIGHT || yPrime < 0) return VOX_Inventory::BlockIds::AIR;
        Region *r = getRegion(x, y, z);
        if(r != 0){
            xPrime = abs(xPrime) - abs(r->xOffset);
            zPrime = abs(zPrime) - abs(r->zOffset);
            if(x < 0) xPrime = REGION_SIZE - (REGION_SIZE + xPrime) - 1;
            if(z < 0) zPrime = REGION_SIZE - (REGION_SIZE + zPrime) - 1;
            if(data) return r->blocks[yPrime][xPrime*REGION_SIZE + zPrime];
            return r->blocks[yPrime][xPrime*REGION_SIZE + zPrime] & 0x00FF;
        }
        return VOX_Inventory::BlockIds::AIR;
    }

    void World::setBlock(float x, float y, float z, unsigned short blockData){
        int xPrime = (int) x;
        int yPrime = (int) y;
        int zPrime = (int) z;
        if(yPrime >= WORLD_HEIGHT || y < 0) return;
        Region *r = getRegion(x, y, z);
        if(r != 0){
            xPrime = abs(xPrime) - abs(r->xOffset);
            zPrime = abs(zPrime) - abs(r->zOffset);
            if(x < 0) xPrime = REGION_SIZE - (REGION_SIZE + xPrime) - 1;
            if(z < 0) zPrime = REGION_SIZE - (REGION_SIZE + zPrime) - 1;
            r->setBlock(xPrime, yPrime, zPrime, blockData & 0x00FF);
            r->needsUpdate = true;
            if(xPrime == 0){
                r = getRegion(x - REGION_SIZE, y, z);
                if(r != 0) r->needsUpdate = true;
            }else if(xPrime == REGION_SIZE - 1){
                r = getRegion(x + REGION_SIZE, y, z);
                if(r != 0) r->needsUpdate = true;
            }else if(zPrime == 0){
                r = getRegion(x, y, z - REGION_SIZE);
                if(r != 0) r->needsUpdate = true;
            }else if(zPrime == REGION_SIZE - 1){
                r = getRegion(x, y, z + REGION_SIZE);
                if(r != 0) r->needsUpdate = true;
            }
        }
    }

    sf::Vector3f World::getCollision(sf::Vector3f start, sf::Vector3f end){
        sf::Vector3f stepVector = start - end;
        int steps = abs(end.y - start.y) * 8;
        for(int i = 0; i < steps; i ++){
            if(i != 0) start -= stepVector;
            if(blocks[getBlock(start.x, start.y, start.z, false)].solid == true) return start;
        }
        return end;
    }

    void World::pruneRegions(){
        if(1 == 1) return;
        sf::Vector3f playerPos = player->getPosition();
        Region **newRegions = new Region*[NUM_REGIONS_LOADED];
        Region *regionCurrentlyIn = getRegion(playerPos.x, 64, playerPos.z);
        if(regionCurrentlyIn == 0) return;
        int xPrime, zPrime, index = 0;
        for(int x = -REGIONS_FROM_PLAYER_LOAD; x <= REGIONS_FROM_PLAYER_LOAD; x ++){
            for(int z = -REGIONS_FROM_PLAYER_LOAD; z <= REGIONS_FROM_PLAYER_LOAD; z ++){
                Region *tempRegion = 0;
                xPrime = (regionCurrentlyIn->xOffset / REGION_SIZE) + x;
                zPrime = (regionCurrentlyIn->zOffset / REGION_SIZE) + z;

                for(int i = 0; i < NUM_REGIONS_LOADED; i ++){
                    if(regions[i] != 0 && (regions[i]->xOffset / REGION_SIZE) == xPrime
                        && (regions[i]->zOffset / REGION_SIZE) == zPrime){
                            tempRegion = regions[i];
                        }
                }
                if(tempRegion == 0) tempRegion = VOX_FileIO::loadRegion(this, xPrime, zPrime);
                newRegions[index++] = tempRegion;
                tempRegion = 0;
            }
        }

        for(int i = 0; i < NUM_REGIONS_LOADED; i ++){
            if(regions[i] != 0){
                if( (abs(regionCurrentlyIn->xOffset - regions[i]->xOffset) > REGIONS_FROM_PLAYER_LOAD) ||
                    (abs(regionCurrentlyIn->zOffset - regions[i]->zOffset) > REGIONS_FROM_PLAYER_LOAD)){
                        //delete regions[i];
                        regions[i] = 0;
                }
            }
        }

        int j = 0, firstNullSpace = 0;
        bool matchFound = false;
        for(int i = 0; i < NUM_REGIONS_LOADED; i ++){
            matchFound = false;
            for(j = 0; j < NUM_REGIONS_LOADED; j ++){
                if(regions[j] == 0) firstNullSpace = j;
                if(regions[j] == newRegions[i]){
                    matchFound = true;
                    break;
                }
            }
            if(!matchFound) regions[firstNullSpace] = newRegions[i];
        }
    }

    World::World(int seed){
        regions = new Region*[NUM_REGIONS_LOADED];
        height = new FastNoise();
        height->SetNoiseType(FastNoise::SimplexFractal);
        height->SetSeed(seed);
        height->SetFractalOctaves(4);
        moisture = new FastNoise();
        moisture->SetNoiseType(FastNoise::SimplexFractal);
        moisture->SetSeed(seed * 2);
        moisture->SetFractalOctaves(8);
        density = new FastNoise();
        density->SetNoiseType(FastNoise::SimplexFractal);
        density->SetSeed(seed / 3);
        density->SetFrequency(0.5f);
        density->SetFractalOctaves(4);
        for(int i = 0; i < NUM_REGIONS_LOADED; i ++) regions[i] = 0;
        for(int x = 0; x < 3; x ++){
            for(int z = 0; z < 3; z ++){
                regions[x*5 + z] = VOX_FileIO::loadRegion(this, x - 1, z - 1);
            }
        }
    }

    void World::setPlayer(VOX_Mob::Player *player){
        this->player = player;
    }

    World::~World(){
        for(int i = 0; i < NUM_REGIONS_LOADED; i ++){
            if(regions[i] != 0) delete regions[i];
        }
        delete regions;
    }

}
