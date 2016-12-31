#include <iostream>
#include <string>
#include "world.h"
#include "fileIO.h"

namespace VOX_World{

    Block::Block(const char* str){
        FILE *file = fopen(str, "r");
        if(file == 0){
            std::cout << "Block: File " << str << " does not exist." << std::endl;
            return;
        }
        VOX_FileIO::Tree data(file);
        id = atoi(data.search("block:data:id").c_str());
        name = data.search("block:data:name").c_str();
        texture = VOX_FileIO::loadBitmapTexture(data.search("block:data:texture").c_str());
    }

}
