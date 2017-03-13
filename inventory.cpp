#include <iostream>
#include "inventory.h"
#include "fileIO.h"
#include "renderable.h"
#include "world.h"

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
        else if(isTool(id) && (id & 0x00FFF000) == 0) contents[slot] = 0xFFFFFFFF;
        else contents[slot] = (num << 24) | (id);
        return true;
    }

    int Inventory::getBreakSpeed(VOX_World::Block *block){
        int id = getSelectedSlot(false) & 0x00000FFF;
        if(id == ItemIds::DIAMOND_PICKAXE) return 3;
        return 1;
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

    bool isTool(int id){
        id = id & 0x00000FFF;
        switch(id){
            case ItemIds::DIAMOND_PICKAXE: return true;
            default: return false;
        }
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

    int getItemWithDefaultMeta(int id){
        if(isBlock(id)) return id;
        return ((items[id - ITEMS_BEGIN].meta) << 12) | id;
    }

    PlayerInventory::PlayerInventory(int numSlots) : Inventory(numSlots){
        this->numSlots = numSlots;
        this->contents = new unsigned int[numSlots];
        this->selectedSlot = 0;
        for(int i = 0; i < numSlots; i ++) contents[i] = 0xFFFFFFFF;
    }

    PlayerInventory::~PlayerInventory(){
        delete [] contents;
    }

    void PlayerInventory::render(float width){
        float blockSize = 48.0f, border = 3.0f, increment = 1.0 / 32.0f;
        float bottomLeftY = 28.0 * increment, bottomLeftX = 0.0;
        float inventoryWidth = (blockSize*9) + border, inventoryHeight = blockSize;
        float x = (width - inventoryWidth) / 2.0f, y = 0.0f, drawIncrement = 1.0 / 32.0f;
        float *texCoords;
        int item, quantity;

        x += border;
        y += border;
        glPushAttrib(GL_CURRENT_BIT);

        glBindTexture(GL_TEXTURE_2D, VOX_Graphics::textureAtlas);
        for(int i = 0; i < 9; i ++){
            item = getSlot(i, false) & 0x00000FFF;
            quantity = getSlot(i, true) >> 24;

            if(i == selectedSlot) bottomLeftX = drawIncrement;
            else bottomLeftX = 0;

            glEnable(GL_TEXTURE_2D);
            glBegin(GL_QUADS);
                glTexCoord2f(bottomLeftX, bottomLeftY); glVertex2f(x, y);
                glTexCoord2f(bottomLeftX + increment, bottomLeftY); glVertex2f(x + blockSize, y);
                glTexCoord2f(bottomLeftX + increment, bottomLeftY + increment); glVertex2f(x + blockSize, y + blockSize);
                glTexCoord2f(bottomLeftX, bottomLeftY + increment); glVertex2f(x, y + blockSize);
            glEnd();
            glDisable(GL_TEXTURE_2D);


            glEnable(GL_BLEND);
            glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                if(item != 0x00000FFF){
                    if(VOX_Inventory::isBlock(item)) texCoords = VOX_World::blocks[item].texCoords;
                    else texCoords = VOX_Inventory::items[item - ITEMS_BEGIN].texCoords;
                    glEnable(GL_TEXTURE_2D);
                    glBegin(GL_QUADS);
                        glTexCoord2f(texCoords[1], texCoords[0]); glVertex2f(x + border, y + border);
                        glTexCoord2f(texCoords[1] + drawIncrement, texCoords[0]); glVertex2f(x + blockSize - border, y + border);
                        glTexCoord2f(texCoords[1] + drawIncrement, texCoords[0] + drawIncrement); glVertex2f(x + blockSize - border, y + inventoryHeight - border);
                        glTexCoord2f(texCoords[1], texCoords[0] + drawIncrement); glVertex2f(x + border, y + inventoryHeight - border);
                    glEnd();
                    glDisable(GL_TEXTURE_2D);
                    VOX_Graphics::renderString(x + (blockSize * 1.0 / 2.0), y + blockSize - (blockSize * 6.0 / 7.0), std::to_string(quantity), sf::Vector3f(0.0f, 0.0f, 0.0f));
                }
            glDisable(GL_BLEND);

            x += blockSize;
        }


        glPopAttrib();
    }

}
