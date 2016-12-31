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
    blocks.push_back(VOX_World::Block("res/blocks/base.txt"));
    return blocks;
}

int main(){
    Camera *camera = new Camera(800, 600);

    double msPerTick = 1000.0 / Settings::tickRate, fps = 0;
    long int currentTime = getCurrentTime();
    long int timeSinceFPSCalculation = currentTime;
    int frames = 0;

    std::vector<VOX_World::Block> blocks = initBlocks();

    VOX_Graphics::Cube cube = VOX_Graphics::Cube::getInstance();
    bool running = true;
    while (running){
        running = camera->handleEvents();
        camera->preRender();
        for(float x = -5; x <= 5; x ++){
            for(float y = -5; y <= 5; y ++){
                for(float z = -5; z <= 5; z ++){
                    glBindTexture(GL_TEXTURE_2D, blocks.at(0).texture);
                    cube.render(x*3, y*3, z*3);
                }
            }
        }
        frames ++;
        camera->postRender();
        while((currentTime + msPerTick) < getCurrentTime()){
            camera->update();
            currentTime += msPerTick;
            //std::cout << "FPS: " << fps << std::endl;
        }
        if(frames > 400){
            fps = (frames / ((getCurrentTime() - timeSinceFPSCalculation) / 1000.0));
            timeSinceFPSCalculation = currentTime;
            frames = 0;
        }
    }

    delete camera;

    return 0;
}
