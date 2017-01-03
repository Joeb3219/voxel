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

    Region::Region(float xOffset, float zOffset, FastNoise *height, FastNoise *moisture, FastNoise *density){
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
                    if(y >= totalHeight || currentDensity <= 0){
                        blocks[y][(x*REGION_SIZE + z)] = BlockIds::AIR;
                        continue;
                    }
                    if(y >= TYPICAL_GROUND){
                        if(currentDensity <= 0.75f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::GRASS;
                        else blocks[y][(x*REGION_SIZE + z)] = BlockIds::STONE;
                    }else if(y >= 58){
                        if(currentDensity <= 0.45f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::GRASS;
                        else if(currentDensity <= 0.55f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::GRAVEL;
                        else blocks[y][(x*REGION_SIZE + z)] = BlockIds::STONE;
                    }else if(y >= 50){
                        if(currentDensity <= 0.15f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::DIRT;
                        else if(currentDensity <= 0.30f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::GRAVEL;
                        else if(currentDensity <= 0.45f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::COAL;
                        else blocks[y][(x*REGION_SIZE + z)] = BlockIds::STONE;
                    }else if(y >= 40){
                        if(currentDensity <= 0.09f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::DIRT;
                        else if(currentDensity <= 0.15f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::GRAVEL;
                        else if(currentDensity <= 0.30f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::COAL;
                        else if(currentDensity <= 0.41f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::IRON;
                        else blocks[y][(x*REGION_SIZE + z)] = BlockIds::STONE;
                    }else if(y >= 30){
                        if(currentDensity <= 0.10f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::DIRT;
                        else if(currentDensity <= 0.20f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::GRAVEL;
                        else if(currentDensity <= 0.32f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::COAL;
                        else if(currentDensity <= 0.46f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::IRON;
                        else blocks[y][(x*REGION_SIZE + z)] = BlockIds::STONE;
                    }else if(y >= 16){
                        if(currentDensity <= 0.12f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::DIRT;
                        else if(currentDensity <= 0.15f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::GRAVEL;
                        else if(currentDensity <= 0.29f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::COAL;
                        else if(currentDensity <= 0.34f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::GOLD;
                        else if(currentDensity <= 0.40f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::REDSTONE;
                        else if(currentDensity <= 0.47f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::IRON;
                        else blocks[y][(x*REGION_SIZE + z)] = BlockIds::STONE;
                    }else{
                        if(currentDensity <= 0.05) blocks[y][(x*REGION_SIZE + z)] = BlockIds::DIRT;
                        else if(currentDensity <= 0.12f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::GRAVEL;
                        else if(currentDensity <= 0.16f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::DIAMOND;
                        else if(currentDensity <= 0.22f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::GOLD;
                        else if(currentDensity <= 0.24f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::REDSTONE;
                        else if(currentDensity <= 0.35f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::COAL;
                        else if(currentDensity <= 0.45f) blocks[y][(x*REGION_SIZE + z)] = BlockIds::IRON;
                        else blocks[y][(x*REGION_SIZE + z)] = BlockIds::STONE;
                    }
                }
            }
        }

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
                        if(blocks[y][(x*REGION_SIZE + z)] != i) continue;
                        //if(!isBlockVisible(x, y, z)) continue;
                        // If y is world height or y isn't world height and nothing above, render.
                        if(y == WORLD_HEIGHT || (y < WORLD_HEIGHT && !VOX_World::blocks[blocks[y + 1][(x * REGION_SIZE) + z]].visible)) cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_TOP, texCoords);  // Check if above is visible
                        if(y == 0 || (y > 0 && !VOX_World::blocks[blocks[y - 1][(x * REGION_SIZE) + z]].visible)) cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_BOTTOM, texCoords);             // Check if below is visible
                        if(x == REGION_SIZE - 1 || (x < REGION_SIZE - 1 && !VOX_World::blocks[blocks[y][((x + 1) * REGION_SIZE) + z]].visible)) cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_RIGHT, texCoords); // Check if right is visible
                        if(x == 0 || (x > 0 && !VOX_World::blocks[blocks[y][((x - 1) * REGION_SIZE) + z]].visible)) cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_LEFT, texCoords);          // Check if left is visible
                        if(z == 0 || (z > 0 && !VOX_World::blocks[blocks[y][((x) * REGION_SIZE) + z - 1]].visible)) cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_BACK, texCoords);          // Check if behind is visible
                        if(z == REGION_SIZE - 1 || (z < REGION_SIZE - 1 && !VOX_World::blocks[blocks[y][((x) * REGION_SIZE) + z + 1]].visible)) cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_FRONT, texCoords);// Check if forward is visible
                    //    cube.render(xPrime, yPrime, zPrime, texCoords);
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
        std::string string_id, string_name, string_visible, string_solid;
        std::string string_texture;
        string_id = data.search("block:data:id");
        string_name = data.search("block:data:name");
        string_visible = data.search("block:data:visible");
        string_solid = data.search("block:data:solid");
        string_texture = data.search("block:data:texture");

        if(!string_id.empty()) id = atoi(string_id.c_str());
        if(!string_name.empty()) name = string_name;
        if(!string_visible.empty() && std::string("true").compare(string_visible) == 0) visible = true;
        else visible = false;
        if(!string_solid.empty() && std::string("true").compare(string_solid) == 0) solid = true;
        else solid = false;

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
        FastNoise height, moisture, density;
        height.SetNoiseType(FastNoise::SimplexFractal);
        height.SetSeed(seed);
        height.SetFractalOctaves(4);
        moisture.SetNoiseType(FastNoise::SimplexFractal);
        moisture.SetSeed(seed * 2);
        moisture.SetFractalOctaves(8);
        density.SetNoiseType(FastNoise::SimplexFractal);
        density.SetSeed(seed / 3);
        density.SetFrequency(0.5f);
        density.SetFractalOctaves(4);
        regions.push_back(new Region(0, 0, &height, &moisture, &density));
        regions.push_back(new Region(1, 0, &height, &moisture, &density));
        regions.push_back(new Region(0, 1, &height, &moisture, &density));
        regions.push_back(new Region(1, 1, &height, &moisture, &density));
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
        currentPos.y += 2.5f;
        sf::Vector3f lookingAt;
        float rYRadians = (PI / 180.0) * rY;
        float rXRadians = (PI / 180.0) * (rX + 90);
        lookingAt.x = x - ((float) cos(rXRadians) * 8.f * fabs(cos(rYRadians)));
        lookingAt.y = (y + 2.5f) - ((float) sin(rYRadians) * 8.f);
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
