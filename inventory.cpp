#include <iostream>
#include "inventory.h"
#include "fileIO.h"

/*
 * Inventory structure:
 * Inventories are stored as an array of 32-bit integers.
 * The left 8 bits (0xFF000000) of the int are the quantity of the item in the stack.
 * The next 12 bits (0x00FFF000) of the int are the meta-data about the item.
 * The next 12 bits (0x00000FFF) of the int are the item ID.
 */

namespace VOX_Inventory{

    Item *items;

    Item::Item(){}

    Item::Item(int id, int meta, VOX_FileIO::Tree *tree, std::string blockPath){
        this->id = id;
        this->meta = meta;
        std::string string_name, string_texture;
        string_name = tree->search(blockPath + ":name");
        string_texture = tree->search(blockPath + ":texture");

        if(!string_name.empty()) name = string_name;

        if(!string_texture.empty()){
            std::string currNum("");
            char c;
            int j = 0;
            for(unsigned int i = 0; i < string_texture.size(); i ++){
                c = string_texture.at(i);
                if(c == ':' || c == ','){
                    texCoords[j++] = atoi(currNum.c_str());
                    currNum = std::string("");
                }else currNum += c;
            }
            texCoords[j++] = atoi(currNum.c_str());
            for(int i = 0; i < 2; i ++){
                if(i % 2 == 1) texCoords[i] = (1.0f / 32.0f) * texCoords[i];
                else texCoords[i] = 1.0f - ((1.0f / 32.0f) * (texCoords[i] + 1));
            }
        }
    }

    int extractDataFromId(int id, bool metaData){
        if(metaData) return (id & 0x00FFF000) >> 16;
        return id & 0x00000FFF;
    }

    bool Inventory::modifySlot(int slot, char num){
        if(slot < 0 || slot >= numSlots) return false;
        int item = contents[slot] & 0x00FFFFFF, quantity = (contents[slot] & 0xFF000000) >> 24;
        if(num > 0){
            if(getMaxStack(slot) <= quantity + num) return false;
            setContents(slot, item, quantity + num);
            return true;
        }
        if(quantity + num < 0) return false;
        setContents(slot, item, quantity + num);
        return true;
    }

    // Adds the indicated item through the inventory, dividing it up as needed.
    // Returns true if operation finished successfully (ie: num equals zero after all is said and done).
    bool Inventory::addItem(int id, char num){
        int maxStack = getMaxStack(id);
        for(int i = 0; i < numSlots; i ++){
            int slotID = contents[i] & 0xFFFFFF, quantity = (contents[i] & 0xFF000000) >> 24;
            if(slotID == NULL_ITEM) quantity = 0;
            if(slotID == NULL_ITEM || slotID == id){
                // We found a slot!
                if(slotID != NULL_ITEM && quantity + num > maxStack){
                    setContents(i, id, maxStack);
                    num = (num + quantity) - maxStack;
                }else{
                    setContents(i, id, quantity + num);
                    return true;
                }
            }
        }
        return false;
    }

    unsigned int Inventory::getSelectedSlot(bool quantity){
        return getSlot(selectedSlot, quantity);
    }

    unsigned int Inventory::getSlot(int slot, bool quantity){
        if(slot > numSlots || slot < 0) return 0;
        if(quantity) return contents[slot];
        return contents[slot] & 0x00FFFFFF;
    }

    bool Inventory::setContents(int slot, int id, char num){
        if(slot > numSlots || slot < 0) return false;
        if(num == 0) contents[slot] = 0xFFFFFFFF;
        else contents[slot] = (num << 24) | (id);
        return true;
    }

    // Since the number of slots can't change, we make it private and allow getting but not setting.
    int Inventory::getNumSlots(){
        return this->numSlots;
    }

    Inventory::Inventory(int numSlots){
        this->numSlots = numSlots;
        this->contents = new unsigned int[numSlots];
        this->selectedSlot = 0;
        for(int i = 0; i < numSlots; i ++) contents[i] = 0xFFFFFFFF;
    }

    Inventory::~Inventory(){
        delete [] contents;
    }

    bool isBlock(int id){
        return ((id & 0x00000FFF) < 2048); // We dedicate the first 2048 IDs to blocks, and the next 2048 IDs to items.
    }

    int getDefaultMetaData(int id){
        if(!isBlock(id)) return items[id - ITEMS_BEGIN].meta;
        return 0;
    }

    int getMaxStack(int id){
        switch(id){
            case 2048: return 1;
            default: return 64;
        }
    }

}
