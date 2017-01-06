#ifndef FILEIO_H_
#define FILEIO_H_

#include <SFML/OpenGL.hpp>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <string>

namespace VOX_World{
    class Region;
    class World;
    class Block;
}

namespace VOX_Inventory{
    class Item;
}

namespace VOX_FileIO{

    GLuint loadBitmapTexture(const char *fileName);
    std::string getToken(FILE *file, int mode = 0);
    std::string getLabelFromTag(std::string tag);
    VOX_World::Region* loadRegion(VOX_World::World *world, int x, int z);
    VOX_Inventory::Item* initItems();
    VOX_World::Block* initBlocks();

    class Tree_Node{
    public:
        std::vector<Tree_Node *> children;
        std::string content, label;
        ~Tree_Node();
        Tree_Node* getChild(std::string label);
        void addChild(Tree_Node* child);
    };

    class Tree{
    private:
        Tree_Node *head;
        void parse(Tree_Node *head, FILE *file);
    public:
        Tree(FILE *file);
        ~Tree();
        Tree_Node* fetchNode(std::string path);
        std::string search(std::string path);
    };

}

#endif
