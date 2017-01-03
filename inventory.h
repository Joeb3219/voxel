#ifndef INVENTORY_H_
#define INVENTORY_H_

#define NULL_ITEM 0x00FFFFFF

namespace VOX_Inventory{

    enum BlockIds{AIR = 0, GRASS = 1, DIRT = 2, STONE = 3, SAND = 4, GRAVEL = 5, WOOD = 6, GOLD = 7,
        IRON = 8, COAL = 9, DIAMOND = 10, REDSTONE = 11};

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
        bool addItem(int id, char num);
        int getNumSlots();
        int getMaxStack(int id);
    };

}

#endif
