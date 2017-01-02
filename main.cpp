#include <iostream>
#include <vector>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <sys/time.h>
#include "main.h"
#include "camera.h"
#include "renderable.h"
#include "world.h"

long int getCurrentTime(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int currentTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    return currentTime;
}

std::vector<VOX_World::Block> initBlocks(){
    std::vector<VOX_World::Block> blocks;
    blocks.push_back(VOX_World::Block("res/blocks/air.txt"));
    blocks.push_back(VOX_World::Block("res/blocks/grass.txt"));
    blocks.push_back(VOX_World::Block("res/blocks/dirt.txt"));
    return blocks;
}

int main(){
    Camera *camera = new Camera(800, 600);

    double msPerTick = 1000.0 / Settings::tickRate, fps = 0;
    long int currentTime = getCurrentTime();
    long int timeSinceFPSCalculation = currentTime;
    int frames = 0;

    VOX_World::World::blocks = initBlocks();
    VOX_World::World *world = new VOX_World::World(1337);
    VOX_Graphics::Cube cube = VOX_Graphics::Cube::getInstance();

    VOX_World::Player player(world, 8.f, 90.f, 8.f);

    bool running = true;
    while (running){
        running = camera->handleEvents();
        camera->preRender();
        world->render();
        frames ++;
        camera->postRender();
        while((currentTime + msPerTick) < getCurrentTime()){
            if(!camera->focused) continue;
            player.setMouseChange(camera->getRelativeMousePosition());
            player.update();
            camera->update(player.getPosition(), player.getViewAngles());
            currentTime += msPerTick;
            world->update();
            //std::cout << "FPS: " << fps << std::endl;
        }
        if(frames > 100){
            fps = (frames / ((getCurrentTime() - timeSinceFPSCalculation) / 1000.0));
            timeSinceFPSCalculation = currentTime;
            frames = 0;
        }
    }

    delete camera;

    return 0;
}
