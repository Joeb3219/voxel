#include <iostream>
#include <SFML/OpenGL.hpp>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "fileIO.h"
#include "inventory.h"
#include "world.h"

namespace VOX_FileIO{

    VOX_World::Block* initBlocks(){
        VOX_FileIO::Tree blockData(fopen("res/blocks.txt", "r"));
        VOX_FileIO::Tree_Node *blocksNode = blockData.fetchNode("blocks");
        VOX_World::Block* blocks = new VOX_World::Block[blocksNode->children.size()];
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
        VOX_FileIO::Tree itemData(fopen("res/items.txt", "r"));
        VOX_FileIO::Tree_Node *itemsNode = itemData.fetchNode("items");
        VOX_Inventory::Item *items = new VOX_Inventory::Item[itemsNode->children.size()];
        std::string itemLabel, itemID;
        int id, meta;
        for(unsigned int i = 0; i < itemsNode->children.size(); i ++){
            itemLabel = itemsNode->children.at(i)->label;
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

    

    VOX_World::Region* loadRegion(VOX_World::World *world, int x, int z){
        VOX_World::Region *region;
        std::string path("saves/");
        path += std::to_string(x * REGION_SIZE) + std::string(":") + std::to_string(z * REGION_SIZE) + std::string(".txt");
        FILE *file = fopen(path.c_str(), "r");
        if(file != 0){
            region = new VOX_World::Region(world, x, z, file);
	    // We let the region's code handle closing the file since it is asynch.
        }else{
            region = new VOX_World::Region(world, x, z);
        }
        return region;
    }

    Tree_Node* Tree_Node::getChild(std::string label){
        Tree_Node *child;
        for(unsigned int i = 0; i < children.size(); i ++){
            child = children[i];
            if(child->label.compare(label) == 0) return child;
        }
        return 0;
    }

    Tree_Node::~Tree_Node(){
        for(Tree_Node *t : children) delete t;
    }

    void Tree_Node::addChild(Tree_Node *child){
        children.push_back(child);
    }

    Tree::Tree(FILE *file){
        this->head = new Tree_Node;
        head->label = std::string("head");
        parse(this->head, file);
    }

    Tree::~Tree(){
        delete head;
    }

    Tree_Node* Tree::fetchNode(std::string path){
        std::string currentSearch;
        Tree_Node *ptr = this->head, *temp;
        while(ptr != 0 && !path.empty()){
            currentSearch = path.substr(0, path.find_first_of(":"));
            path = path.substr(path.find_first_of(":") + 1, path.size());
            temp = ptr->getChild(currentSearch);
            if(temp == 0) return ptr;
            ptr = temp;
        }
        if(ptr != 0) return ptr;
        return 0;
    }

    std::string Tree::search(std::string path){
        std::string currentSearch;
        Tree_Node *ptr = this->head, *temp;
        while(ptr != 0 && !path.empty()){
            currentSearch = path.substr(0, path.find_first_of(":"));
            path = path.substr(path.find_first_of(":") + 1, path.size());
            temp = ptr->getChild(currentSearch);
            if(temp == 0) return ptr->content;
            ptr = temp;
        }
        if(ptr != 0) return ptr->content;
        return std::string("");
    }

    void Tree::parse(Tree_Node *parent, FILE *file){
        std::string token, label;
        while((token = getToken(file)).empty() == false){
            label = getLabelFromTag(token);
            if(label.compare(token) == 0){ // If the label is the same as the token, it's not a tag.
                parent->content = label;
                getToken(file); // Consume the next token, which will close this one.
                return;
            }else{     // We are dealing with a tag
                if(label.compare(parent->label) == 0){  // This is the close tag
                    return;
                }else{
                    Tree_Node *child = new Tree_Node;
                    child->label = label;
                    parent->addChild(child);
                    parse(child, file);
                }
            }
        }
    }


}

std::string VOX_FileIO::getLabelFromTag(std::string tag){
    if(tag.size() < 2 || (tag.size() >= 1 && tag.at(0) != '<')) return tag;
    std::string result = tag.substr(1, tag.size() - 2);
    if(result.size() >= 1 && result.at(0) == '/') result = result.substr(1, result.size());
    return result;
}

std::string VOX_FileIO::getToken(FILE *file, int mode){
    std::string result("");
    char c;
    bool beganTag = false, beganChars = false;
    while( (c = fgetc(file)) != EOF ){
        if(c == '\0') break;
        if(c == '\n'){
            if(result.empty()) continue;
            else break;
        }
        if(c == '\t' || (!beganChars && c == ' ')) continue;
        if(c == '<'){
            if(result.empty()){
                beganChars = true;
                beganTag = true;
                result += c;
            }else{
                fseek(file, -1, SEEK_CUR);
                break;
            }
        }else{
            beganChars = true;
            result += c;
            if(c == '>' && beganTag == true) break;
        }
    }
    return result;
}

/**
 *  Converts a bitmap file into a Texture.
 *  The Bitmap must be wellformed, with the following criteria:
 *  0x0: 'B', 0x1, 'M'
 *  0x0A: position of where image data begins.
 *  0x12: width of image, 0x16: height of image.
 *  Will return 0 if any errors occur.
 */
GLuint VOX_FileIO::loadBitmapTexture(const char *fileName){
    GLuint texture;
    int width = 0, height = 0, dataPos = 0;     // dataPos is used to track where data begins, width/height are width/height in header image.
    unsigned char * data;                       // Each segment of data is a single byte.
    FILE * file = fopen(fileName, "r");

    if ( file == NULL ){
        std::cout << "Attempting to load texture: " << fileName << ", but it does not exist." << std::endl;
        return 0;
    }

    // Load the image's header to get data we need about the image.
    data = new unsigned char[54];               // malloc(54 * sizeof(unsigned char));  // We malloc 54 bytes of memory so that we can read the header.
    fread(data, 1, 54, file);                   // Read the first 54 bytes of the bitmap.
    if(data[0] != 'B' && data[1] != 'M'){
        std::cout << "Attempting to load texture: " << fileName << ", but it has an invalid header." << std::endl;
        return 0;
    }

    // The following three calls assume we're working with 32-bit+ integers.
    // Starting at their indication positions, will convert 4 bytes into an integer.
    // TODO: Make this safe for systems that don't use 32-bit+ integers.
    dataPos = *(int*)&(data[0x0A]);
    width = *(int*)&(data[0x12]);
    height = *(int*)&(data[0x16]);

    delete [] data;

    // Load the image body at the dataPos position (and onward) to get actual color data.
    fseek(file, dataPos, SEEK_SET);         // Jump to position of color data.

    std::cout << "Image: " << fileName << ", Height: " << height << ", width: " << width << ", DataPos: " << dataPos << std::endl;

    data = new unsigned char[width * height * 3];
    fread(data, 1, width * height * 3, file);
    fclose( file );                       // We no longer need the file, close it.

    // Now that we have an array of colors, have OpenGL map them to a texture.
    glGenTextures( 1, &texture );
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    delete [] data;

    return texture;
}
