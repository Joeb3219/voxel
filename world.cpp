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
                int totalHeight = (heightMap[x*REGION_SIZE + z])*TYPICAL_GROUND + TYPICAL_GROUND;
                for(int y = 0; y < WORLD_HEIGHT; y ++){
                    if(y >= totalHeight) blocks[y][(x*REGION_SIZE + z)] = World::blocks.at(BlockIds::AIR);
                    else{
                        blocks[y][(x*REGION_SIZE + z)] = World::blocks.at(BlockIds::GRASS);
                    }
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
        int xPrime = (int) x;
        int zPrime = (int) z;
        int xRegion = ((xPrime + REGION_SIZE - 1) / REGION_SIZE * REGION_SIZE) - REGION_SIZE;
        int zRegion = ((zPrime + REGION_SIZE - 1) / REGION_SIZE * REGION_SIZE) - REGION_SIZE;
        for(unsigned int i = 0; i < regions.size(); i ++){
            if(regions[i]->xOffset == xRegion && regions[i]->zOffset == zRegion) return regions[i];
        }
        return 0;
    }

    Block World::getBlock(float x, float y, float z){
        int xPrime = (int) x;
        int yPrime = (int) y;
        int zPrime = (int) z;
        if(yPrime >= WORLD_HEIGHT || yPrime < 0) return World::blocks.at(BlockIds::AIR);
        Region *r = getRegion(x, y, z);
        if(r != 0){
            xPrime -= (r->xOffset);
            zPrime -= (r->zOffset);
            return r->blocks[yPrime][xPrime * REGION_SIZE + zPrime];
        }
        return World::blocks.at(BlockIds::AIR);
    }

    void World::setBlock(float x, float y, float z, Block block){
        int xPrime = (int) x;
        int yPrime = (int) y;
        int zPrime = (int) z;
        if(yPrime >= WORLD_HEIGHT || y < 0) return;
        Region *r = getRegion(x, y, z);
        if(r != 0){
            xPrime -= (r->xOffset);
            zPrime -= (r->zOffset);
            r->blocks[yPrime][xPrime * REGION_SIZE + zPrime] = block;
            r->needsUpdate = true;
        }
    }

    sf::Vector3f World::getCollision(sf::Vector3f start, sf::Vector3f end){
        sf::Vector3f stepVector = start - end;
        int steps = abs(end.y - start.y) * 8;
        for(int i = 0; i < steps; i ++){
            if(i != 0) start -= stepVector;
            if(getBlock(start.x, start.y, start.z).solid == true) return start;
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
        //regions.push_back(new Region(1, 0, &height, &moisture));
        //regions.push_back(new Region(0, 1, &height, &moisture));
        //regions.push_back(new Region(1, 1, &height, &moisture));
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
        currentPos.y += 2;
        sf::Vector3f lookingAt;
        float rYRadians = (PI / 180.0) * rY;
        float rXRadians = (PI / 180.0) * (rX + 90);
        lookingAt.x = x - ((float) cos(rXRadians) * 8.f * fabs(cos(rYRadians)));
        lookingAt.y = (y + 2) - ((float) sin(rYRadians) * 8.f);
        lookingAt.z = z - ((float) sin(rXRadians) * 8.f * fabs(cos(rYRadians)));
        sf::Vector3f stepVector = (currentPos - lookingAt) * (1.0f / steps);
        // Now we hone into the vector to find a collision.
        for(int i = 0; i < steps; i ++){
            currentPos -= stepVector;
            if(world->getBlock(currentPos.x, currentPos.y, currentPos.z).solid){
                if(!adjacent) return currentPos;
                return (currentPos + stepVector);
            }
        }
        return lookingAt;
    }

    void Player::update(){
        tickCounter ++;
        std::cout << "[x,y,z,yVel]: " << x << "," << y << "," << z << "," << yVelocity << std::endl;

        // Jumping / falling code.
        float newY = y;
        VOX_Math::calculateFalling(&newY, &yVelocity, 1);
        if(newY < y){
            for(float j = y; j > newY; j -= 0.0125f){
                y = j;
                if(world->getBlock(x, j, z).solid == true){
                    yVelocity = 0;
                    y = (float) ((int) y);
                    break;
                }
            }
        }else{
            for(float j = y; j < newY; j += 0.0125f){
                y = j;
                if(world->getBlock(x, j + 2, z).solid == true){
                    yVelocity = 0;
                    y = (float) ((int) j + 2);
                    break;
                }
            }
        }

        sf::Vector3f lookingAt = getLookingAt();
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && tickCounter > 15){
            tickCounter = 0;
            world->setBlock(lookingAt.x, lookingAt.y, lookingAt.z, world->blocks.at(BlockIds::AIR));
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && tickCounter > 15){
            if(world->getBlock(lookingAt.x, lookingAt.y, lookingAt.z).id != World::blocks.at(BlockIds::AIR).id){
                tickCounter = 0;
                lookingAt = getLookingAt(true); // Recompute the looking at to get the adjacent block.
                world->setBlock(lookingAt.x, lookingAt.y, lookingAt.z, world->blocks.at(BlockIds::DIRT));
            }
        }
        std::cout << "Currently looking at block " << world->getBlock(lookingAt.x, lookingAt.y, lookingAt.z).name << std::endl;
        float rYRadians = (PI / 180.0) * rY;
        float rXRadians = (PI / 180.0) * (rX + 90);
        float rXAdjustedRadians = (PI / 180.0) * (rX);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
            x -= (float) cos(rXAdjustedRadians) * moveSpeed;
            z -= (float) sin(rXAdjustedRadians) * moveSpeed;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
            x += (float) cos(rXAdjustedRadians) * moveSpeed;
            z += (float) sin(rXAdjustedRadians) * moveSpeed;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            x += (float) cos(rXRadians) * moveSpeed * fabs(cos(rYRadians));
            //y += (float) sin(rYRadians) * moveSpeed;
            z += (float) sin(rXRadians) * moveSpeed * fabs(cos(rYRadians));
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            x -= (float) cos(rXRadians) * moveSpeed * fabs(cos(rYRadians));
            //y -= (float) sin(rYRadians) * moveSpeed;
            z -= (float) sin(rXRadians) * moveSpeed * fabs(cos(rYRadians));
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
            if(yVelocity == 0.0f) yVelocity = 0.25f;
        }
        if(rX > 360) rX -= 360;
        if(rX < 0) rX += 360;
        if(rY > 90) rY = 90;
        if(rY < -90) rY = -90;
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
