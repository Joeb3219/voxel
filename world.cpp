#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <list>
#include <mutex>
#include <unordered_map>
#include <algorithm>
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
    BlockData *NULL_BLOCK = new BlockData();

    void Region::loadRegionFromMemory(FILE *file){
        int numCharactersInFile = 98304, i =0;
        char contents[numCharactersInFile + 1];
        fread(&contents, sizeof(char), numCharactersInFile, file);
        for(int x = 0; x < REGION_SIZE; x ++){
            for(int z = 0; z < REGION_SIZE; z ++){
                for(int y = 0; y < WORLD_HEIGHT; y ++){
                    BlockData *block = new BlockData();
                    block->other = contents[i++];
                    block->meta = contents[i++];
                    block->id = contents[i++];
                    blocks[y][x*REGION_SIZE + z] = block;
                }
            }
        }
        fclose(file);
	    loaded = true;
        this->needsUpdate = this->updatingMesh = true;
    }

    void Region::spawnRegion(){
        double heightMap[REGION_SIZE * REGION_SIZE];
        double moistureAverage = 0.0, elevationAverage = 0.0;
        for(int x = 0; x < REGION_SIZE; x ++){
            for(int z = 0; z < REGION_SIZE; z ++){
                heightMap[x*REGION_SIZE + z] = VOX_Math::convertScale(world->height->GetNoise(x + this->xOffset,z + this->zOffset), -1.0f, 1.f, 0.0f, 1.f);
                elevationAverage += heightMap[x*REGION_SIZE + z];
                moistureAverage += heightMap[x*REGION_SIZE + z];
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
                    float currentDensity = VOX_Math::convertScale(world->density->GetNoise(x * 1.0f + this->xOffset, y * 1.0f, z * 1.0f + this->zOffset), -1.0f, 1.0f, -0.05f, 1.0f);
                    if(y >= totalHeight || currentDensity <= 0.04f){
                        BlockData *blockData = new BlockData();
                        blockData->lighting = 15;
                        blockData->other = 0;
                        blockData->meta = 0;
                        blockData->id = VOX_Inventory::BlockIds::AIR;
                        blocks[y][x*REGION_SIZE + z] = blockData;
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

        // CAVE GENERATION
        // We use a perlin function and determine if the abs(perlin(x,y,z)) is below a threshold.
        // If it is, we apply coherent noise to generate a width of the tunnel to dig out.
        for(int x = 0; x < REGION_SIZE; x ++){
            for(int z = 0; z < REGION_SIZE; z ++){
                int totalHeight = (heightMap[x*REGION_SIZE + z])*TYPICAL_GROUND + TYPICAL_GROUND;
                for(int y = 4; y < totalHeight + 2; y ++){
                    float noise = VOX_Math::convertScale(world->cave->GetNoise(x * 1.0f + this->xOffset, y * 1.0f, z * 1.0f + this->zOffset), -1.0f, 1.0f, 0.0f, 1.0f);
                    float levelRelativeToTotalHeight = 1.0f - ((y * 1.0f) / totalHeight);
                    float acceptableNoiseLevel = VOX_Math::convertScale(levelRelativeToTotalHeight, 0.0f, 1.0f, 0.13f, .25f);
                    if(noise <= acceptableNoiseLevel){
                        float coherentNoise = VOX_Math::convertScale(world->coherent->GetNoise(x * 1.0f + this->xOffset, y * 1.0f, z * 1.0f + this->zOffset), -1.0f, 1.0f, 0.0f, 1.0f);
                        coherentNoise = ((coherentNoise) * TYPICAL_CAVE_WIDTH) + TYPICAL_CAVE_WIDTH;
                        for(int xP = x - (coherentNoise / 2); xP <= x + (coherentNoise / 2); xP ++){
                            if(xP < 0 || xP >= REGION_SIZE) continue;
                            for(int zP = z - (coherentNoise / 2); zP <= z + (coherentNoise / 2); zP ++){
                                if(zP < 0 || zP >= REGION_SIZE) continue;
                                for(int yP = y - (coherentNoise / 2); yP <= y + (coherentNoise / 2); yP ++){
                                    if(yP < 0 || yP >= WORLD_HEIGHT - 1) continue;
                                    setBlock(xP, yP, zP, VOX_Inventory::BlockIds::AIR);
                                }
                            }
                        }
                    }
                }
            }
        }
        loaded = true;
        this->needsUpdate = this->updatingMesh = true;
    }

    int Region::getFirstAirLightBlock(int x, int z){
        BlockData *data;
        for(int y = WORLD_HEIGHT - 2; y > 0; y --){
            data = blocks[y][x*REGION_SIZE + z];
            if(data != 0 &&
                data != NULL_BLOCK &&
                data->id != 0) return y + 1;
        }
        return 0;
    }

    Region::Region(){}
    Region::Region(World *world, float xOffset, float zOffset){
        // We offload loading to another thread, and then set loaded to true when we're ready.
        this->world = world;
        this->xOffset = xOffset * REGION_SIZE;
        this->zOffset = zOffset * REGION_SIZE;
        std::thread thread_loadRegion(&Region::spawnRegion, this);
        thread_loadRegion.detach();
    }

    Region::Region(World *world, float xOffset, float zOffset, FILE *file){
        this->world = world;
        this->xOffset = xOffset * REGION_SIZE;
        this->zOffset = zOffset * REGION_SIZE;
        std::thread thread_loadRegion(&Region::loadRegionFromMemory, this, file);
        thread_loadRegion.detach();
    }

    // Destroying a region will save it to saves/xOffet:zOffset.txt
    Region::~Region(){
        std::cout << "Saving region: " << xOffset << ", " << zOffset << std::endl;
        std::string fileName("saves/");
        fileName += std::to_string(xOffset) + std::string(":") + std::to_string(zOffset) + std::string(".txt");
        FILE *file = fopen(fileName.c_str(), "w+");
        BlockData *block;

        for(int x = 0; x < REGION_SIZE; x ++){
            for(int z = 0; z < REGION_SIZE; z ++){
                for(int y = 0; y < WORLD_HEIGHT; y ++){
                    block = blocks[y][x*REGION_SIZE + z];
                    fputc(block->other, file);
                    fputc(block->meta, file);
                    fputc(block->id, file);
                    delete block;
                }
            }
        }

        fclose(file);

        delete [] vertexArray;
    }

    bool Region::isInRegion(float x, float y, float z){
        convertCoordinates(&x, &y, &z, false);
        if(x < 0 || x >= REGION_SIZE) return false;
        if(z < 0 || z >= REGION_SIZE) return false;
        return true;
    }

    BlockData* Region::getBlock(int x, int y, int z){
        if(y >= WORLD_HEIGHT || y < 0) return NULL_BLOCK;
        return blocks[y][x*REGION_SIZE + z];
    }

    void Region::setBlock(int x, int y, int z, int blockID){
        BlockData *block = new BlockData();
        block->lighting = block->other = 0;
        block->meta = VOX_World::blocks[blockID].damage;
        block->id = blockID;
        blocks[y][x*REGION_SIZE + z] = block;
    }

    void Region::convertCoordinates(float *x, float *y, float *z, bool toWorld){
        float xTemp = (*x), zTemp = (*z);
        if(toWorld){
            (*z) = this->zOffset + zTemp;
            (*x) = this->xOffset + xTemp;
        }else{
            (*z) = zTemp - this->zOffset;
            (*x) = xTemp - this->xOffset;
        }
    }

    void Region::generateDisplayedFaces(){
        float xPrime, yPrime, zPrime, *texCoords;
        VOX_Graphics::Cube cube = VOX_Graphics::Cube::getInstance();
        std::vector<float*> faces;
        for(float x = 0; x < REGION_SIZE; x ++){
            for(float z = 0; z < REGION_SIZE; z ++){
                for(float y = 0; y < WORLD_HEIGHT; y ++){
                    xPrime = x;
                    yPrime = y;
                    zPrime = z;
                    convertCoordinates(&xPrime, &yPrime, &zPrime, true);
                    BlockData *blockData = getBlock(x, y, z);
                    Block block = VOX_World::blocks[(int)blockData->id];
                    if(!block.visible) continue;
                    texCoords = &block.texCoords[0];
                    if(!checkSurroundingsIsVisible(xPrime, yPrime + 1, zPrime)){
                        faces.push_back(cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_TOP, texCoords, blockData->lighting_top));
                    }
                    if(!checkSurroundingsIsVisible(xPrime, yPrime - 1, zPrime)){
                        faces.push_back(cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_BOTTOM, texCoords, blockData->lighting_bottom));
                    }
                    if(!checkSurroundingsIsVisible(xPrime + 1, yPrime, zPrime)){
                        faces.push_back(cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_RIGHT, texCoords, blockData->lighting_right));
                    }
                    if(!checkSurroundingsIsVisible(xPrime - 1, yPrime, zPrime)){
                        faces.push_back(cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_LEFT, texCoords, blockData->lighting_left));
                    }
                    if(!checkSurroundingsIsVisible(xPrime, yPrime, zPrime + 1)){
                        faces.push_back(cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_FRONT, texCoords, blockData->lighting_front));
                    }
                    if(!checkSurroundingsIsVisible(xPrime, yPrime, zPrime - 1)){
                        faces.push_back(cube.renderFace(xPrime, yPrime, zPrime, VOX_Graphics::Face::FACE_BACK, texCoords, blockData->lighting_back));
                    }

                }
            }
        }
        float *newVertexArray = new float[NUM_FLOATS_PER_FACE * faces.size()];
        for(unsigned int i = 0; i < faces.size(); i ++){
            for(int j = 0; j < NUM_FLOATS_PER_FACE; j ++){
                newVertexArray[i * NUM_FLOATS_PER_FACE + j] = faces.at(i)[j];
            }
            delete [] faces.at(i);
        }
        if(vertexArray != 0) delete [] vertexArray;
        vertexArray = newVertexArray;
        numFacesDrawn = faces.size();
        updatingMesh = faceBuildingThreadSpawned = false;
    }

    bool Region::checkSurroundingsIsVisible(float x, float y, float z){
        if(y <= 0 || y >= WORLD_HEIGHT - 1) return false;
        if(isInRegion(x, y, z)){
            convertCoordinates(&x, &y, &z);
            return VOX_World::blocks[(int)getBlock(x, y, z)->id].visible;
        }
        else return VOX_World::blocks[(int)world->getBlock(x, y, z)->id].visible;
        return false;
    }

    void Region::update(){
        if(!loaded) return; // We aren't loaded, how could we be updating?
        if(needsUpdate){
            BlockData *data;
            for(int x = 0; x < REGION_SIZE; x ++){
                for(int z = 0; z < REGION_SIZE; z ++){
                    for(int y = 0; y < WORLD_HEIGHT; y ++){
                        data = blocks[y][x * REGION_SIZE + z];
                        data->lighting_back = data->lighting_top = data->lighting_bottom = data->lighting_front = data->lighting_left = data->lighting_right = VOX_World::blocks[(int) data->id].light;
                    }
                }
            }
            for(int x = 0; x < REGION_SIZE; x ++){
                for(int z = 0; z < REGION_SIZE; z ++){
                    //int firstAirLight = getFirstAirLightBlock(x, z);
                    //for(int y = firstAirLight; y < WORLD_HEIGHT; y ++){
                    //    data = blocks[y][x*REGION_SIZE + z];
                    //    if(VOX_World::blocks[(int)getBlock(x, y + 1, z)->id].visible == true) data->id = VOX_Inventory::BlockIds::DIRT;
                    //    if(data->id == VOX_Inventory::BlockIds::AIR) world->radiateLight(x, y, z, 15, ORIGIN);
                    //    if(VOX_World::blocks[(int) data->id].light > 0) world->radiateLight(x, y, z, VOX_World::blocks[(int) data->id].light, ORIGIN);
                    //}
                    for(int y = 0; y < WORLD_HEIGHT - 1; y ++){
                        data = blocks[y][x * REGION_SIZE + z];
                        if(data->id == VOX_Inventory::BlockIds::GRASS && VOX_World::blocks[(int)getBlock(x, y + 1, z)->id].solid) data->id = VOX_Inventory::BlockIds::DIRT;
                        if(VOX_World::blocks[(int) data->id].light > 0){
                            float xP = x, yP = y, zP = z;
                            convertCoordinates(&xP, &yP, &zP, true);
                            std::cout << "Propogating a light source about " << xP << ", " << yP << ", " << zP << " with level " << VOX_World::blocks[(int) data->id].light << std::endl;
                            data->lighting_back = data->lighting_top = data->lighting_bottom = data->lighting_front = data->lighting_left = data->lighting_right = VOX_World::blocks[(int) data->id].light;
                            world->radiateLight((int) xP, (int) yP, (int) zP, VOX_World::blocks[(int) data->id].light);
                        }
                    }
                }
            }
            needsUpdate = false;
        }
        if(updatingMesh){
            if(!faceBuildingThreadSpawned){
                std::thread buildFaces(&Region::generateDisplayedFaces, this);
                buildFaces.detach();
                faceBuildingThreadSpawned = true;
            }
        }
    }

    void Region::render(){
        glPushMatrix();
        if(vertexArray != 0){
            // Strategy: Store a color3f for every vertex which describes the light on a scale of 0-15.
            // Color is 1.0-(1.0*(15-(15-i))). IE: 15 light -> 1.0 color, 0 light = 0 color.
            glBindTexture(GL_TEXTURE_2D, VOX_Graphics::textureAtlas);
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(3, GL_FLOAT, (3 + 2 + 3) * 4, vertexArray);
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, (3 + 2 + 3) * 4, vertexArray + 3);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, (3 + 2 + 3) * 4, vertexArray + 6);
            glDrawArrays(GL_QUADS, 0, numFacesDrawn * 4);
            glDisableClientState(GL_COLOR_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glPopMatrix();
    }

    Block::Block(){

    }

    Block::Block(VOX_FileIO::Tree *tree, std::string blockPath){
        std::string string_id, string_name, string_visible, string_solid, string_damage, string_drops, string_light;
        std::string string_texture;
        string_id = tree->search(blockPath + ":id");
        string_name = tree->search(blockPath + ":name");
        string_visible = tree->search(blockPath + ":visible");
        string_solid = tree->search(blockPath + ":solid");
        string_texture = tree->search(blockPath + ":texture");
        string_damage = tree->search(blockPath + ":damage");
        string_drops = tree->search(blockPath + ":drops");
        string_light = tree->search(blockPath + ":light");

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
        if(!string_light.empty()) light = atoi(string_light.c_str());
        else light = 0;

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

    void World::radiateLight(int x, int y, int z, int level){
        std::list<sf::Vector3f> queue, visited, discoveredNodes;
        std::vector<Region*> regionsAffected;
        Region *r;

        queue.push_back(sf::Vector3f(x, y, z));

        while(level > 0){
            level --; // We are now operating on one light level less than before (ie: the source is brighter than the next block).
            while(!queue.empty()){
                sf::Vector3f node = queue.front();
                sf::Vector3f up = sf::Vector3f(node.x, node.y + 1, node.z), down = sf::Vector3f(node.x, node.y - 1, node.z),
                    left = sf::Vector3f(node.x - 1, node.y, node.z), right = sf::Vector3f(node.x + 1, node.y, node.z),
                    front = sf::Vector3f(node.x, node.y, node.z + 1), back = sf::Vector3f(node.x, y, node.z - 1);

                queue.pop_front();
                visited.push_back(node);

                BlockData *block;

                block = getBlock(up.x, up.y, up.z);
                block->lighting_bottom = (unsigned char) std::max((int) block->lighting_bottom, level);
                if(!blocks[(int) block->id].solid && std::find(visited.begin(), visited.end(), up) == visited.end()) discoveredNodes.push_back(up);

                block = getBlock(down.x, down.y, down.z);
                block->lighting_top = (unsigned char) std::max((int) block->lighting_top, level);
                if(!blocks[(int) block->id].solid && std::find(visited.begin(), visited.end(), down) == visited.end()) discoveredNodes.push_back(down);

                block = getBlock(left.x, left.y, left.z);
                block->lighting_right = (unsigned char) std::max((int) block->lighting_right, level);
                if(!blocks[(int) block->id].solid && std::find(visited.begin(), visited.end(), left) == visited.end()){
                    discoveredNodes.push_back(left);
                    r = getRegion(left.x, left.y, left.z);
                    if(std::find(regionsAffected.begin(), regionsAffected.end(), r) == regionsAffected.end()) regionsAffected.push_back(r);
                }

                block = getBlock(right.x, right.y, right.z);
                block->lighting_left = (unsigned char) std::max((int) block->lighting_left, level);
                if(!blocks[(int) block->id].solid && std::find(visited.begin(), visited.end(), right) == visited.end()){
                    discoveredNodes.push_back(right);
                    r = getRegion(right.x, right.y, right.z);
                    if(std::find(regionsAffected.begin(), regionsAffected.end(), r) == regionsAffected.end()) regionsAffected.push_back(r);
                }

                block = getBlock(front.x, front.y, front.z);
                block->lighting_back = (unsigned char) std::max((int) block->lighting_back, level);
                if(!blocks[(int) block->id].solid && std::find(visited.begin(), visited.end(), front) == visited.end()){
                        discoveredNodes.push_back(front);
                        r = getRegion(front.x, front.y, front.z);
                        if(std::find(regionsAffected.begin(), regionsAffected.end(), r) == regionsAffected.end()) regionsAffected.push_back(r);
                }

                block = getBlock(back.x, back.y, back.z);
                block->lighting_front = (unsigned char) std::max((int) block->lighting_front, level);
                if(!blocks[(int) block->id].solid && std::find(visited.begin(), visited.end(), back) == visited.end()){
                        discoveredNodes.push_back(back);
                        r = getRegion(back.x, back.y, back.z);
                        if(std::find(regionsAffected.begin(), regionsAffected.end(), r) == regionsAffected.end()) regionsAffected.push_back(r);
                }
            }

            while(!discoveredNodes.empty()){
                sf::Vector3f node = discoveredNodes.front();
                discoveredNodes.pop_front();
                if(std::find(queue.begin(), queue.end(), node) == queue.end()) queue.push_back(node);
            }
        }

        Region *currentlyIn = getRegion(x, y, z);
        for(unsigned int i = 0; i < regionsAffected.size(); i ++){
            r = regionsAffected.at(i);
            if(r != currentlyIn && r->loaded) r->updatingMesh = true;
        }
    }

    void World::update(){
        for(auto &p : *regionMap){
            Region *r = p.second;
            if(r != 0 && r->loaded && (r->needsUpdate || r->updatingMesh)) r->update();
        }
    }

    void World::render(){
        facesRendered = 0;
        sf::Vector3f currentPos = player->getPosition();
        sf::Vector3f viewAngles = player->getViewAngles();
        Region *currentlyIn = getRegion(currentPos.x, currentPos.y, currentPos.z);
        if(currentlyIn == 0) return; // In a non-existent region.
        int currentXOffset = currentlyIn->xOffset, currentZOffset = currentlyIn->zOffset;
        sf::Vector3f origin = VOX_Math::computeVectorFromPos(currentPos, viewAngles.x + 180, viewAngles.y + 180, - 4.0f);
        sf::Vector3f leftBound = VOX_Math::computeVectorFromPos(origin, viewAngles.x - FOV / 2, viewAngles.y, FARCLIP + 4.0f);
        sf::Vector3f rightBound = VOX_Math::computeVectorFromPos(origin, viewAngles.x + FOV / 2, viewAngles.y, FARCLIP + 4.0f);

        for(auto& p: *regionMap){
            Region *r = p.second;
            if( abs( (r->xOffset - currentXOffset) / REGION_SIZE) <= REGIONS_FROM_PLAYER_RENDER &&
                abs( (r->zOffset - currentZOffset) / REGION_SIZE) <= REGIONS_FROM_PLAYER_RENDER){
                bool shouldRender = false;
                for(int x = -5; x < REGION_SIZE + 5; x ++){
                    for(int z = -5; z < REGION_SIZE + 5; z ++){
                        if(VOX_Math::insideTriangle(origin, leftBound, rightBound, sf::Vector3f(r->xOffset + x, 0, r->zOffset + z))) shouldRender = true;
                    }
                }
                if(shouldRender){
                    r->render();
                    facesRendered += r->numFacesDrawn;
                }
            }
        }
    }

    Region* World::getRegion(float x, float y, float z){
        for(auto &p : *regionMap){
            Region *r = p.second;
            if(r != 0 && r->isInRegion(x, y, z)) return r;
        }
        return 0;
    }

    Block World::getBlock(unsigned int identifier){
        return blocks[identifier & 0x00FF];
    }

    BlockData* World::getBlock(float x, float y, float z){
        if(y >= WORLD_HEIGHT || y < 0) return NULL_BLOCK;
        Region *r = getRegion(x, y, z);
        if(r != 0){
            if(!r->loaded) return NULL_BLOCK;
            r->convertCoordinates(&x, &y, &z);
            return r->getBlock(x, y, z);
        }
        return NULL_BLOCK;
    }

    void World::setBlock(float x, float y, float z, unsigned int blockData){
        if(y >= WORLD_HEIGHT || y < 0) return;
        Region *r = getRegion(x, y, z), *neighbor;
        if(r != 0){
            r->convertCoordinates(&x, &y, &z, false);
            r->setBlock(x, y, z, blockData & 0x00FF);
            r->convertCoordinates(&x, &y, &z, true);
            r->needsUpdate = r->updatingMesh = true;
            if((int)(x - r->xOffset) == 0){
                neighbor = getRegion(x - REGION_SIZE, y, z);
                if(neighbor != 0) neighbor->updatingMesh = true;
            }else if((int)(x - r->xOffset) == REGION_SIZE - 1){
                neighbor = getRegion(x + REGION_SIZE - 1, y, z);
                if(neighbor != 0) neighbor->updatingMesh = true;
            }
            if((int)(z - r->zOffset) == 0){
                neighbor = getRegion(x, y, z - REGION_SIZE);
                if(neighbor != 0) neighbor->updatingMesh = true;
            }else if((int)(z - r->zOffset) == REGION_SIZE - 1){
                neighbor = getRegion(x, y, z + REGION_SIZE);
                if(neighbor != 0) neighbor->updatingMesh = true;
            }
        }
    }

    sf::Vector3f World::getCollision(sf::Vector3f start, sf::Vector3f end){
        sf::Vector3f stepVector = start - end;
        int steps = abs(end.y - start.y) * 8;
        for(int i = 0; i < steps; i ++){
            if(i != 0) start -= stepVector;
            if(blocks[(int)getBlock(start.x, start.y, start.z)->id].solid == true) return start;
        }
        return end;
    }

    void World::pruneRegions(){
        sf::Vector3f currentPos = player->getPosition();
        Region *currentlyIn = getRegion(currentPos.x, currentPos.y, currentPos.z);
        if(currentlyIn == 0) return; // In a non-existent region.

        std::string label;
        int rX, rZ;
        for(int x = -REGIONS_FROM_PLAYER_LOAD; x <= REGIONS_FROM_PLAYER_LOAD; x ++){
            for(int z = -REGIONS_FROM_PLAYER_LOAD; z <= REGIONS_FROM_PLAYER_LOAD; z ++){
                rX = x + (currentlyIn->xOffset / REGION_SIZE);
                rZ = z + (currentlyIn->zOffset / REGION_SIZE);
                label = std::to_string(rX) + std::string(":") + std::to_string(rZ);
                auto p = regionMap->find(label);
                if(p == regionMap->end()){
                    std::cout << "Found region: " << rX << ", " << rZ << " to load!" << std::endl;
                    regionMap->insert({label, VOX_FileIO::loadRegion(this, rX, rZ)});
                }
            }
        }

        for(auto &p: *regionMap){
            Region *r = p.second;
            if(r != 0 && r->loaded &&
                abs((r->xOffset - currentlyIn->xOffset) / REGION_SIZE) > REGIONS_FROM_PLAYER_UNLOAD &&
                abs((r->zOffset - currentlyIn->zOffset) / REGION_SIZE) > REGIONS_FROM_PLAYER_UNLOAD){
                delete r;
                regionMap->erase(p.first);
            }
        }
    }

    World::World(int seed){
        regionMap = new std::unordered_map<std::string, Region*>;
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
        cave = new FastNoise();
        cave->SetNoiseType(FastNoise::SimplexFractal);
        cave->SetSeed(seed * 3);
        cave->SetFrequency(0.5f);
        cave->SetFractalOctaves(4);
        coherent = new FastNoise();
        coherent->SetNoiseType(FastNoise::CubicFractal);
        coherent->SetSeed(seed / 5);
        coherent->SetFrequency(0.5f);
        coherent->SetFractalOctaves(4);
        for(int x = 0; x < 3; x ++){
            for(int z = 0; z < 3; z ++){
                std::string regionName = std::to_string(x - 1) + std::string(":") + std::to_string(z - 1);
                regionMap->insert({regionName, VOX_FileIO::loadRegion(this, x - 1, z - 1)});
            }
        }
    }

    void World::setPlayer(VOX_Mob::Player *player){
        this->player = player;
    }

    World::~World(){
        for(auto &p: *regionMap){
            Region *r = p.second;
            if(r != 0){
                delete r;
            }
        }
        delete density;
        delete moisture;
        delete height;
    }

}
