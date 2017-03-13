#ifndef INVENTORY_H_
#define INVENTORY_H_

#define NULL_ITEM 0x00FFFFFF

#include <string>
#include "fileIO.h"

#define ITEMS_BEGIN 2048

namespace VOX_World{
    class Block;
}

namespace VOX_Inventory{

    enum BlockIds{AIR = 0, GRASS = 1, DIRT = 2, STONE = 3, SAND = 4, GRAVEL = 5, WOOD = 6, GOLD = 7,
        IRON = 8, COAL = 9, DIAMOND = 10, REDSTONE = 11};

    enum ItemIds{DIAMOND_PICKAXE = 2048};

    int extractDataFromId(int id, bool metaData = false);
    bool isBlock(int id);
    int getMaxStack(int id);
    bool isTool(int id);
    int getItemWithDefaultMeta(int id);

    class Item{
    public:
        Item(int id, int meta, VOX_FileIO::Tree *tree, std::string blockPath);
        Item();
        std::string name;
        int id, meta;
        float texCoords[2] = {0,0};
    };

    extern Item *items;

    class Inventory{
    private:
        unsigned int *contents;
        int numSlots;
    public:
        int selectedSlot;
        Inventory(int numSlots);
        ~Inventory();
        int getBreakSpeed(VOX_World::Block *block);
        bool setContents(int slot, int id, char num);
        unsigned int getSlot(int slot, bool quantity = true);
        unsigned int getSelectedSlot(bool quantity = true);
        bool modifySlot(int slot, char num);
        bool addItem(int id, char num);
        int getNumSlots();
        virtual void render(float width) = 0;
    };

    class PlayerInventory : public Inventory{
    private:
        unsigned int *contents;
        int numSlots;
    public:
        PlayerInventory(int numSlots);
        ~PlayerInventory();
        void render(float width);
    };

}

#endif
