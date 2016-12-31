#ifndef FILEIO_H_
#define FILEIO_H_

#include <SFML/OpenGL.hpp>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <string>
#include "world.h"

namespace VOX_FileIO{

    GLuint loadBitmapTexture(const char *fileName);
    VOX_World::Block loadBlock(const char *fileName);
    std::string getToken(FILE *file, int mode = 0);
    std::string getLabelFromTag(std::string tag);

    class Tree_Node{
    private:
        std::vector<Tree_Node *> children;
    public:
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
        std::string search(std::string path);
    };

}

#endif
