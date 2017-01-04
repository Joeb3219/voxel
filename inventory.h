#ifndef INVENTORY_H_
#define INVENTORY_H_

#define NULL_ITEM 0x00FFFFFF

#include <string>

namespace VOX_Inventory{

    enum BlockIds{AIR = 0, GRASS = 1, DIRT = 2, STONE = 3, SAND = 4, GRAVEL = 5, WOOD = 6, GOLD = 7,
        IRON = 8, COAL = 9, DIAMOND = 10, REDSTONE = 11};

    enum ItemIds{DIAMOND_PICKAXE = 2048};

    bool extractDataFromId(bool metaData = false);
    bool isBlock(int id);
    int getMaxStack(int id);

    class Item{
    public:
        std::string name;
        int id;
    };

    class Inventory{
    private:
        unsigned int *contents;
        int numSlots;
    public:
        int selectedSlot;
        Inventory(int numSlots);
        ~Inventory();
        bool setContents(int slot, int id, char num);
        unsigned int getSlot(int slot, bool quantity = true);
        unsigned int getSelectedSlot(bool quantity = true);
        bool modifySlot(int slot, char num);
        bool addItem(int id, char num);
        int getNumSlots();
    };

}

#endif
