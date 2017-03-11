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
#include "mob.h"
#include "inventory.h"

long int getCurrentTime(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int currentTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    return currentTime;
}

int main(int argc, char **argv){
    glutInit(&argc, argv);
    Camera *camera = new Camera(800, 600);

    double msPerTick = 1000.0 / Settings::tickRate, fps = 0;
    long int currentTime = getCurrentTime();
    long int timeSinceFPSCalculation = currentTime;
    int frames = 0;

    VOX_Graphics::textureAtlas = VOX_FileIO::loadBitmapTexture("res/textures.bmp");
    VOX_World::blocks = VOX_FileIO::initBlocks();
    VOX_Inventory::items = VOX_FileIO::initItems();
    VOX_World::World *world = new VOX_World::World(1337);

    VOX_Mob::Player *player = new VOX_Mob::Player(world, 2.0f, 90.f, 2.0f);
    world->setPlayer(player);
    camera->setFollowing(player);

    bool running = true;
    while (running){
        running = camera->handleEvents();
        camera->preRender();

        // Strictly 3D space rendering
        world->render();

        // Text & HUD rendering
        camera->pre2DRender();

        VOX_Graphics::renderString(8, camera->height - 13, std::string("FPS: ") + std::to_string(fps));
        sf::Vector3f playerPos = player->getPosition();
        VOX_Graphics::renderString(8, camera->height - 26, std::string("[x,y,z]: ") + std::to_string(playerPos.x)
                + ", " + std::to_string(playerPos.y) + ", " + std::to_string(playerPos.z));
        sf::Vector3f lookingAt = player->getLookingAt();
        VOX_Graphics::renderString(8, camera->height - 39, std::string("Looking at: ") +
                VOX_World::blocks[world->getBlock(lookingAt.x, lookingAt.y, lookingAt.z, false)].name + ": " + std::to_string(lookingAt.x)
                + ", " + std::to_string(lookingAt.y) + ", " + std::to_string(lookingAt.z));
        VOX_Graphics::renderString(8, camera->height - 52, std::string("Faces Rendered : ") +
                std::to_string(world->facesRendered));

        player->renderInventory(camera->width);

        // Cleanup
        camera->postRender();
        frames ++;
        while((currentTime + msPerTick) < getCurrentTime()){
            if(!camera->focused) continue;
            player->setMouseChange(camera->getRelativeMousePosition());
            player->update();
            camera->update();
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
