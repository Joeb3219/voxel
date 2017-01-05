#include <iostream>
#include <string>
#include <vector>
#include "world.h"
#include "math.h"
#include "fileIO.h"
#include "lib/fastNoise/FastNoise.h"
#include "renderable.h"
#include "inventory.h"

#define PI 3.14159265

namespace VOX_World{

    Block *blocks;

    Region::Region(){}

    Region::Region(float xOffset, float zOffset, FastNoise *height, FastNoise *moisture, FastNoise *density, Region **regions){
        this->regionList = regions;
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
                int totalHeight = (heightMap[x*REGION_SIZE + z])*TYPICAL_GROUND + TYPICAL_GROUND;
                for(int y = 0; y < WORLD_HEIGHT; y ++){
                    float currentDensity = VOX_Math::convertScale(density->GetNoise(x * 1.0f + this->xOffset, y * 1.0f, z * 1.0f + this->zOffset), -1.0f, 1.0f, -0.05f, 1.0f);
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

    Region::Region(float xOffset, float zOffset, FILE *file, Region **regions){
        this->regionList = regions;
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
        int xRegion = (((xPrime + REGION_SIZE) / REGION_SIZE * REGION_SIZE) - REGION_SIZE);
        int zRegion = (((zPrime + REGION_SIZE) / REGION_SIZE * REGION_SIZE) - REGION_SIZE);
        if(x < 0) xRegion = -xRegion - REGION_SIZE;
        if(z < 0) zRegion = -zRegion - REGION_SIZE;
        if(xRegion == this->xOffset && zRegion == this->zOffset) return true;
        return false;
    }

    unsigned short Region::getBlock(int x, int y, int z){
        return blocks[y][x*REGION_SIZE + z] & 0x00FF;
    }

    void Region::setBlock(int x, int y, int z, int blockID){
        int meta = VOX_World::blocks[blockID].damage;
        blocks[y][x*REGION_SIZE + z] = (((meta & 0x00FF) << 8) | (blockID & 0x00FF));
    }

    void Region::convertCoordinates(float *x, float *y, float *z){
        int xPrime = abs(*x);
        int zPrime = abs(*z);
        (*x) = xPrime - this->xOffset;
        (*z) = zPrime - this->zOffset;
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
            for(int i = 0; i < NUM_REGIONS_LOADED; i ++){
                if(regionList[i] != 0 && regionList[i]->isInRegion(xPrime, yPrime, zPrime)){
                    return VOX_World::blocks[regionList[i]->getBlock(xPrime - regionList[i]->xOffset, y, zPrime - regionList[i]->zOffset)].visible;
                }
            }
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
        for(unsigned int i = 0; i < NUM_REGIONS_LOADED; i ++){
            if(regions[i] != 0 && regions[i]->needsUpdate) regions[i]->update();
        }
    }

    void World::render(){
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
            xPrime -= (r->xOffset);
            zPrime -= (r->zOffset);
            if(data) return r->blocks[yPrime][xPrime * REGION_SIZE + zPrime];
            return r->blocks[yPrime][xPrime * REGION_SIZE + zPrime] & 0x00FF;
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
            xPrime -= (r->xOffset);
            zPrime -= (r->zOffset);
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
                regions[x*5 + z] = loadRegion(x, z);
            }
        }
    }

    Region* World::loadRegion(int x, int z){
        Region *region;
        std::string path("saves/");
        path += std::to_string(x * REGION_SIZE) + std::string(":") + std::to_string(z * REGION_SIZE) + std::string(".txt");
        FILE *file = fopen(path.c_str(), "r");
        if(file != 0){
            region = new Region(x, z, file, regions);
            fclose(file);
        }else{
            region = new Region(x, z, height, moisture, density, regions);
        }
        return region;
    }

    World::~World(){
        for(int i = 0; i < NUM_REGIONS_LOADED; i ++){
            if(regions[i] != 0) delete regions[i];
        }
        delete regions;
    }

    sf::Vector3f Player::getPosition(){
        return sf::Vector3f(x, y, z);
    }

    sf::Vector3f Player::getViewAngles(){
        return sf::Vector3f(rX, rY, rZ);
    }

    void Player::setMouseChange(sf::Vector2i change){
        rX += (change.x * 0.08);
        rY -= (change.y * 0.08);
    }

    sf::Vector3f Player::getLookingAt(bool adjacent){
        int steps = 16; // Moves roughly 1/4 of a block at a time.
        sf::Vector3f currentPos = getPosition();
        currentPos.y += 2.5f;
        sf::Vector3f lookingAt;
        float rYRadians = (PI / 180.0) * rY;
        float rXRadians = (PI / 180.0) * (rX + 90);
        lookingAt.x = x - ((float) cos(rXRadians) * 4.f * fabs(cos(rYRadians)));
        lookingAt.y = (y + 2.5f) - ((float) sin(rYRadians) * 4.f);
        lookingAt.z = z - ((float) sin(rXRadians) * 4.f * fabs(cos(rYRadians)));
        sf::Vector3f stepVector = (currentPos - lookingAt) * (1.0f / steps);
        // Now we hone into the vector to find a collision.
        for(int i = 0; i < steps; i ++){
            currentPos -= stepVector;
            if(blocks[world->getBlock(currentPos.x, currentPos.y, currentPos.z, false)].solid){
                if(!adjacent) return currentPos;
                return (currentPos + stepVector);
            }
        }
        return lookingAt;
    }

    void Player::update(){
        tickCounter ++;

        float newY = y, newX = x, newZ = z;

        checkMovement(&newX, &newZ);

        int numSteps = 4;
        sf::Vector3f curr = sf::Vector3f(x, y, z), goal(newX, y, newZ), stepVector;
        stepVector = (curr - goal) * (1.0f / numSteps);
        for(int i = 0; i < numSteps; i ++){
            curr -= stepVector;
            if(blocks[world->getBlock(curr.x, curr.y + 1, curr.z, false)].solid == true){
                curr += stepVector;
                break;
            }
        }
        x = curr.x;
        z = curr.z;

        // Jumping / falling code.
        VOX_Math::calculateFalling(&newY, &yVelocity, 1);
        if(newY < y){
            for(float j = y; j > newY; j -= 0.0125f){
                y = j;
                if(blocks[world->getBlock(x, j, z, false)].solid == true){
                    if(fabs(j - ((int)j)) < 0.0125f){
                        yVelocity = 0;
                        y = (float) ( y);
                        break;
                    }
                }
            }
        }else{
            for(float j = y; j < newY; j += 0.0125f){
                y = j;
                if(blocks[world->getBlock(x, j + 2.5f, z, false)].solid == true){
                    yVelocity = 0;
                    y = (float) ((int) j + 2.5f);
                    break;
                }
            }
        }

        sf::Vector3f lookingAt = getLookingAt();
        unsigned short id, meta, blockLookingAt;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && tickCounter > 10){
            blockLookingAt = world->getBlock(lookingAt.x, lookingAt.y, lookingAt.z, true);
            id = blockLookingAt & 0xFF;
            meta = (blockLookingAt & 0xFF00) >> 8;
            if(id != 0 && meta != 0xFF){
                if(meta == 0){
                    tickCounter = 0;
                    inventory->addItem(blocks[id].drops, 1);
                    world->setBlock(lookingAt.x, lookingAt.y, lookingAt.z, VOX_Inventory::BlockIds::AIR);
                }else world->getRegion(lookingAt.x, lookingAt.y, lookingAt.z)->modifyMeta(
                        (int) lookingAt.x, (int) lookingAt.y, (int) lookingAt.z, meta - 1);
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && tickCounter > 35){
            if(world->getBlock(lookingAt.x, lookingAt.y, lookingAt.z, false) != VOX_Inventory::BlockIds::AIR){
                if(inventory->getSelectedSlot(false) != NULL_ITEM && VOX_Inventory::isBlock(inventory->getSelectedSlot(false))){
                    tickCounter = 0;
                    lookingAt = getLookingAt(true); // Recompute the looking at to get the adjacent block.
                    world->setBlock(lookingAt.x, lookingAt.y, lookingAt.z, inventory->getSelectedSlot(false));
                    inventory->modifySlot(inventory->selectedSlot, -1);
                }
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) inventory->selectedSlot = 0;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) inventory->selectedSlot = 1;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) inventory->selectedSlot = 2;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) inventory->selectedSlot = 3;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) inventory->selectedSlot = 4;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num6)) inventory->selectedSlot = 5;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num7)) inventory->selectedSlot = 6;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num8)) inventory->selectedSlot = 7;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num9)) inventory->selectedSlot = 8;

        if(rX > 360) rX -= 360;
        if(rX < 0) rX += 360;
        if(rY > 90) rY = 90;
        if(rY < -90) rY = -90;
    }

    void Player::checkMovement(float *x, float *z){
        float rXRadians = (PI / 180.0) * (rX + 90);
        float rXAdjustedRadians = (PI / 180.0) * (rX);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
            (*x) -= (float) cos(rXAdjustedRadians) * moveSpeed;
            (*z) -= (float) sin(rXAdjustedRadians) * moveSpeed;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
            (*x) += (float) cos(rXAdjustedRadians) * moveSpeed;
            (*z) += (float) sin(rXAdjustedRadians) * moveSpeed;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            (*x) += (float) cos(rXRadians) * moveSpeed;
            (*z) += (float) sin(rXRadians) * moveSpeed;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            (*x) -= (float) cos(rXRadians) * moveSpeed;
            (*z) -= (float) sin(rXRadians) * moveSpeed;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
            if(yVelocity == 0.0f) yVelocity = 0.10f;
        }
    }

    void Player::render(){

    }

    void Player::renderInventory(float width){
        float blockSize = 48.0f, border = 3.0f;
        float inventoryWidth = (blockSize*9) + border, inventoryHeight = blockSize;
        float x = (width - inventoryWidth) / 2.0f, y = 0.0f, drawIncrement = 1.0 / 32.0f;
        float *texCoords;
        int item, quantity;

        // Used for drawing inventory textures
        glBindTexture(GL_TEXTURE_2D, VOX_Graphics::textureAtlas);

        glPushAttrib(GL_CURRENT_BIT);
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + inventoryWidth + (border * 9), y);
            glVertex2f(x + inventoryWidth + (border * 9), y + inventoryHeight + border * 2);
            glVertex2f(x, y + inventoryHeight + border * 2);
        glEnd();

        x += border;
        y += border;

        for(int i = 0; i < 9; i ++){
            item = inventory->getSlot(i, false);
            quantity = inventory->getSlot(i, true) >> 24;

            if(i == inventory->selectedSlot) glColor3f(0.5f, 0.5f, 0.5f);
            else glColor3f(0.8f, 0.8f, 0.8f);
            glBegin(GL_QUADS);
                glVertex2f(x, y);
                glVertex2f(x + blockSize, y);
                glVertex2f(x + blockSize, y + inventoryHeight);
                glVertex2f(x, y + inventoryHeight);
            glEnd();

            if(item != NULL_ITEM){
                if(VOX_Inventory::isBlock(item)) texCoords = blocks[item].texCoords;
                else texCoords = VOX_Inventory::items[item - ITEMS_BEGIN].texCoords;
                glColor3f(1.0f, 1.0f, 1.0f);
                glEnable(GL_TEXTURE_2D);
                glBegin(GL_QUADS);
                    glTexCoord2f(texCoords[1], texCoords[0]); glVertex2f(x + border, y + border);
                    glTexCoord2f(texCoords[1] + drawIncrement, texCoords[0]); glVertex2f(x + blockSize - border, y + border);
                    glTexCoord2f(texCoords[1] + drawIncrement, texCoords[0] + drawIncrement); glVertex2f(x + blockSize - border, y + inventoryHeight - border);
                    glTexCoord2f(texCoords[1], texCoords[0] + drawIncrement); glVertex2f(x + border, y + inventoryHeight - border);
                glEnd();
                glDisable(GL_TEXTURE_2D);
                VOX_Graphics::renderString(x, y + 16, std::to_string(quantity));
            }

            x += border + blockSize;
        }


        glPopAttrib();
    }

    Player::~Player(){
        delete inventory;
    }

    Player::Player(World *world, float x, float y, float z){
        this->x = x;
        this->y = y;
        this->z = z;
        this->world = world;
        this->inventory = new VOX_Inventory::Inventory(40);
        inventory->setContents(0, 2048, 1);
    }

}
