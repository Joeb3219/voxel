#include <iostream>
#include <string>
#include <vector>
#include "world.h"
#include "math.h"
#include "fileIO.h"
#include "lib/fastNoise/FastNoise.h"
#include "renderable.h"

#define PI 3.14159265

namespace VOX_World{

    Block *blocks;

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
                int totalHeight = (heightMap[x*REGION_SIZE + z])*TYPICAL_GROUND + TYPICAL_GROUND;
                for(int y = 0; y < WORLD_HEIGHT; y ++){
                    if(y >= totalHeight) blocks[y][(x*REGION_SIZE + z)] = BlockIds::AIR;
                    else{
                        blocks[y][(x*REGION_SIZE + z)] = BlockIds::GRASS;
                    }
                }
            }
        }

    }

    void Region::buildDisplayList(){
        DL_ID = glGenLists(1);
        glNewList(DL_ID, GL_COMPILE);
        float xPrime, yPrime, zPrime;
        for(int i = 0; i < 256; i ++){
            if(blocks[i] == 0) break;   // A null pointer
            glBindTexture(GL_TEXTURE_2D, VOX_World::blocks[i].texture);
            for(int x = 0; x < REGION_SIZE; x ++){
                for(int z = 0; z < REGION_SIZE; z ++){
                    for(int y = 0; y < WORLD_HEIGHT; y ++){
                        xPrime = x + xOffset;
                        yPrime = y;
                        zPrime = z + zOffset;
                        short id = blocks[y][(x*REGION_SIZE + z)];
                        if(id != i) continue;
                        if(!isBlockVisible(x, y, z)) continue;
                        VOX_Graphics::Cube::getInstance().render(xPrime, yPrime, zPrime);
                    }
                }
            }
        }

        glEndList();
    }

    bool Region::isBlockVisible(int x, int y, int z){
        if(!VOX_World::blocks[blocks[y][(x * REGION_SIZE) + z]].visible) return false;                         // Check if this is visible
        if(y == 0 || y == WORLD_HEIGHT || x == 0 || x == REGION_SIZE - 1 || z == 0 || z == REGION_SIZE - 1) return true;
        if(!VOX_World::blocks[blocks[y + 1][(x * REGION_SIZE) + z]].visible) return true;  // Check if above is visible
        if(!VOX_World::blocks[blocks[y - 1][(x * REGION_SIZE) + z]].visible) return true;             // Check if below is visible
        if(!VOX_World::blocks[blocks[y][((x + 1) * REGION_SIZE) + z]].visible) return true;// Check if right is visible
        if(!VOX_World::blocks[blocks[y][((x - 1) * REGION_SIZE) + z]].visible) return true;          // Check if left is visible
        if(!VOX_World::blocks[blocks[y][((x) * REGION_SIZE) + z - 1]].visible) return true;          // Check if behind is visible
        if(!VOX_World::blocks[blocks[y][((x) * REGION_SIZE) + z + 1]].visible) return true;// Check if forward is visible
        return false;
    }

    void Region::update(){
        int ind;
        for(int x = 0; x < REGION_SIZE; x ++){
            for(int y = 0; y < WORLD_HEIGHT; y ++){
                for(int z = 0; z < REGION_SIZE; z ++){
                    ind = x*REGION_SIZE + z;
                    if(blocks[y][ind] == BlockIds::GRASS && VOX_World::blocks[blocks[y + 1][ind]].visible == true) blocks[y][ind] = BlockIds::DIRT;
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

    Block::Block(const char* str){
        FILE *file = fopen(str, "r");
        if(file == 0){
            std::cout << "Block: File " << str << " does not exist." << std::endl;
            return;
        }
        VOX_FileIO::Tree data(file);
        std::string string_id, string_name, string_texture, string_visible, string_solid;
        string_id = data.search("block:data:id");
        string_name = data.search("block:data:name");
        string_texture = data.search("block:data:texture");
        string_visible = data.search("block:data:visible");
        string_solid = data.search("block:data:solid");

        if(!string_id.empty()) id = atoi(string_id.c_str());
        if(!string_name.empty()) name = string_name;
        if(!string_texture.empty()) texture = VOX_FileIO::loadBitmapTexture(string_texture.c_str());
        else texture = 0;
        if(!string_visible.empty() && std::string("true").compare(string_visible) == 0) visible = true;
        else visible = false;
        if(!string_solid.empty() && std::string("true").compare(string_solid) == 0) solid = true;
        else solid = false;
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

    Region* World::getRegion(float x, float y, float z){
        int xPrime = abs(x);
        int zPrime = abs(z);
        int xRegion = (((xPrime + REGION_SIZE) / REGION_SIZE * REGION_SIZE) - REGION_SIZE);
        int zRegion = (((zPrime + REGION_SIZE) / REGION_SIZE * REGION_SIZE) - REGION_SIZE);
        if(x < 0) xRegion = -xRegion - REGION_SIZE;
        if(z < 0) zRegion = -zRegion - REGION_SIZE;
        for(unsigned int i = 0; i < regions.size(); i ++){
            if(regions[i]->xOffset == xRegion && regions[i]->zOffset == zRegion) return regions[i];
        }
        return 0;
    }

    Block World::getBlock(short identifier){
        return blocks[identifier & 0x00FF];
    }

    short World::getBlock(float x, float y, float z, bool data){
        int xPrime = (int) x;
        int yPrime = (int) y;
        int zPrime = (int) z;
        if(yPrime >= WORLD_HEIGHT || yPrime < 0) return BlockIds::AIR;
        Region *r = getRegion(x, y, z);
        if(r != 0){
            xPrime -= (r->xOffset);
            zPrime -= (r->zOffset);
            if(data) return r->blocks[yPrime][xPrime * REGION_SIZE + zPrime];
            return r->blocks[yPrime][xPrime * REGION_SIZE + zPrime] & 0x00FF;
        }
        return BlockIds::AIR;
    }

    void World::setBlock(float x, float y, float z, short blockData){
        int xPrime = (int) x;
        int yPrime = (int) y;
        int zPrime = (int) z;
        if(yPrime >= WORLD_HEIGHT || y < 0) return;
        Region *r = getRegion(x, y, z);
        if(r != 0){
            xPrime -= (r->xOffset);
            zPrime -= (r->zOffset);
            r->blocks[yPrime][xPrime * REGION_SIZE + zPrime] = blockData;
            r->needsUpdate = true;
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
        int steps = 32; // Moves roughly 1/4 of a block at a time.
        sf::Vector3f currentPos = getPosition();
        currentPos.y += 3;
        sf::Vector3f lookingAt;
        float rYRadians = (PI / 180.0) * rY;
        float rXRadians = (PI / 180.0) * (rX + 90);
        lookingAt.x = x - ((float) cos(rXRadians) * 8.f * fabs(cos(rYRadians)));
        lookingAt.y = (y + 3) - ((float) sin(rYRadians) * 8.f);
        lookingAt.z = z - ((float) sin(rXRadians) * 8.f * fabs(cos(rYRadians)));
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

        // Jumping / falling code.
        float newY = y, newX = x, newZ = z;
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
                if(blocks[world->getBlock(x, j + 3, z, false)].solid == true){
                    yVelocity = 0;
                    y = (float) ((int) j + 3);
                    break;
                }
            }
        }

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

        sf::Vector3f lookingAt = getLookingAt();
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && tickCounter > 35){
            tickCounter = 0;
            world->setBlock(lookingAt.x, lookingAt.y, lookingAt.z, BlockIds::AIR);
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && tickCounter > 35){
            if(world->getBlock(lookingAt.x, lookingAt.y, lookingAt.z, false) != BlockIds::AIR){
                tickCounter = 0;
                lookingAt = getLookingAt(true); // Recompute the looking at to get the adjacent block.
                world->setBlock(lookingAt.x, lookingAt.y, lookingAt.z, BlockIds::DIRT);
            }
        }

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

    Player::Player(World *world, float x, float y, float z){
        this->x = x;
        this->y = y;
        this->z = z;
        this->world = world;
    }

}
