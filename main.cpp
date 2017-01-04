#include <iostream>
#include <vector>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <sys/time.h>
#include <GL/glut.h>
#include "main.h"
#include "camera.h"
#include "renderable.h"
#include "fileIO.h"
#include "world.h"
#include "inventory.h"

long int getCurrentTime(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int currentTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    return currentTime;
}

VOX_World::Block* initBlocks(){
    VOX_World::Block* blocks = new VOX_World::Block[16];
    VOX_FileIO::Tree blockData(fopen("res/blocks.txt", "r"));
    VOX_FileIO::Tree_Node *blocksNode = blockData.fetchNode("blocks");
    std::string blockLabel, blockID;
    for(unsigned int i = 0; i < blocksNode->children.size(); i ++){
        blockLabel = blocksNode->children.at(i)->label;
        blockID = blockData.search("blocks:" + blockLabel + ":id");
        if(blockID.empty()){
            std::cout << "Malformed block data: " << blockLabel << std::endl;
            continue;
        }
        blocks[atoi(blockID.c_str())] = VOX_World::Block(&blockData, "blocks:" + blockLabel);
    }
    return blocks;
}

VOX_Inventory::Item* initItems(){
    VOX_Inventory::Item *items = new VOX_Inventory::Item[2048];
    VOX_FileIO::Tree itemData(fopen("res/items.txt", "r"));
    VOX_FileIO::Tree_Node *blocksNode = itemData.fetchNode("items");
    std::string itemLabel, itemID;
    int id, meta;
    for(unsigned int i = 0; i < blocksNode->children.size(); i ++){
        itemLabel = blocksNode->children.at(i)->label;
        itemID = itemData.search("items:" + itemLabel + ":id");
        if(itemID.empty()){
            std::cout << "Malformed item data: " << itemLabel << std::endl;
            continue;
        }
        id = atoi(itemID.substr(0, itemID.find_first_of(":")).c_str());
        meta = atoi( itemID.substr(itemID.find_first_of(":") + 1, itemID.size()).c_str() );
        items[id - ITEMS_BEGIN] = VOX_Inventory::Item(id, meta, &itemData, "items:" + itemLabel);
    }
    return items;
}

int main(int argc, char **argv){
    glutInit(&argc, argv);
    Camera *camera = new Camera(800, 600);

    double msPerTick = 1000.0 / Settings::tickRate, fps = 0;
    long int currentTime = getCurrentTime();
    long int timeSinceFPSCalculation = currentTime;
    int frames = 0;

    VOX_Graphics::textureAtlas = VOX_FileIO::loadBitmapTexture("res/textures.bmp");
    VOX_World::blocks = initBlocks();
    VOX_Inventory::items = initItems();
    VOX_World::World *world = new VOX_World::World(1337);

    VOX_World::Player player(world, 8.f, 90.f, 8.f);

    bool running = true;
    while (running){
        running = camera->handleEvents();
        camera->preRender();

        // Strictly 3D space rendering
        world->render();

        // Text & HUD rendering
        camera->pre2DRender();

        VOX_Graphics::renderString(8, camera->height - 13, std::string("FPS: ") + std::to_string(fps));
        sf::Vector3f playerPos = player.getPosition();
        VOX_Graphics::renderString(8, camera->height - 26, std::string("[x,y,z]: ") + std::to_string(playerPos.x)
                + ", " + std::to_string(playerPos.y) + ", " + std::to_string(playerPos.z));
        sf::Vector3f lookingAt = player.getLookingAt();
        VOX_Graphics::renderString(8, camera->height - 39, std::string("Looking at: ") +
                VOX_World::blocks[world->getBlock(lookingAt.x, lookingAt.y, lookingAt.z, false)].name + ": " + std::to_string(lookingAt.x)
                + ", " + std::to_string(lookingAt.y) + ", " + std::to_string(lookingAt.z));

        player.renderInventory(camera->width);

        // Cleanup
        camera->postRender();
        frames ++;
        while((currentTime + msPerTick) < getCurrentTime()){
            if(!camera->focused) continue;
            player.setMouseChange(camera->getRelativeMousePosition());
            player.update();
            camera->update(player.getPosition(), player.getViewAngles());
            currentTime += msPerTick;
            world->update();
        }
        if(frames > 100){
            fps = (frames / ((getCurrentTime() - timeSinceFPSCalculation) / 1000.0));
            timeSinceFPSCalculation = currentTime;
            frames = 0;
        }
    }

    delete camera;
    delete world;

    return 0;
}
