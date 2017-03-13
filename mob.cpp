#include <iostream>
#include "mob.h"
#include "math.h"
#include "world.h"
#include "renderable.h"
#include "inventory.h"

namespace VOX_Mob{

    sf::Vector3f Player::getPosition(){
        return sf::Vector3f(x, y, z);
    }

    sf::Vector3f Player::getViewAngles(){
        return sf::Vector3f(rX, rY, rZ);
    }

    void Player::setMouseChange(sf::Vector2i change){
        rX += (change.x * 0.08);
        rY -= (change.y * 0.08);
    }

    sf::Vector3f Player::getLookingAt(bool adjacent){
        int steps = 16; // Moves roughly 1/4 of a block at a time.
        sf::Vector3f currentPos = getPosition();
        currentPos.y += 2.5f;
        sf::Vector3f lookingAt = VOX_Math::computeVectorFromPos(currentPos, rX, rY, 4.0f);
        sf::Vector3f stepVector = (currentPos - lookingAt) * (1.0f / steps);
        // Now we hone into the vector to find a collision.
        for(int i = 0; i < steps; i ++){
            currentPos -= stepVector;
            if(VOX_World::blocks[world->getBlock(currentPos.x, currentPos.y, currentPos.z, false)].solid){
                if(!adjacent) return currentPos;
                return (currentPos + stepVector);
            }
        }
        return lookingAt;
    }

    void Player::update(){
        tickCounter ++;

        float newY = y, newX = x, newZ = z;

        checkMovement(&newX, &newZ);

        int numSteps = 8;
        sf::Vector3f curr = sf::Vector3f(x, y, z), goal(newX, y, newZ), stepVector;
        stepVector = (curr - goal) * (1.0f / numSteps);
        for(int i = 0; i < numSteps; i ++){
            curr -= stepVector;
            if(VOX_World::blocks[world->getBlock(curr.x, curr.y + 1, curr.z, false)].solid == true ||
                VOX_World::blocks[world->getBlock(curr.x, curr.y + 2, curr.z, false)].solid){
                curr += stepVector;
                break;
            }
        }
        x = curr.x;
        z = curr.z;

        // Jumping / falling code.
        VOX_Math::calculateFalling(&newY, &yVelocity, 1);
        if(newY < y){
            for(float j = y; j > newY; j -= 0.0125f){
                y = j;
                if(VOX_World::blocks[world->getBlock(x, j, z, false)].solid == true){
                    if(fabs(j - ((int)j)) < 0.0125f){
                        yVelocity = 0;
                        y = (float) ( y);
                        break;
                    }
                }
            }
        }else{
            for(float j = y; j < newY; j += 0.0125f){
                y = j;
                if(VOX_World::blocks[world->getBlock(x, j + 2.5f, z, false)].solid == true){
                    yVelocity = 0;
                    y = (float) ((int) j);
                    break;
                }
            }
        }

        sf::Vector3f lookingAt = getLookingAt();
        unsigned short id, meta, blockLookingAt;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && tickCounter > 10){
            blockLookingAt = world->getBlock(lookingAt.x, lookingAt.y, lookingAt.z, true);
            id = blockLookingAt & 0xFF;
            meta = (blockLookingAt & 0xFF00) >> 8;
            if(id != 0 && meta != 0xFF){
                if(meta == 0){
                    tickCounter = 0;
                    inventory->addItem(VOX_World::blocks[id].drops, 1);
                    world->setBlock(lookingAt.x, lookingAt.y, lookingAt.z, VOX_Inventory::BlockIds::AIR);
                    unsigned int hand = inventory->getSelectedSlot(false);
                    if(VOX_Inventory::isTool(hand)){
                        inventory->setContents(inventory->selectedSlot, ((hand & 0x00FFF000) - 0x00001000) | (hand & 0x00000FFF), 1);
                    }
                }else{
                    int blockDamage = inventory->getBreakSpeed(&VOX_World::blocks[id]);
                    if(meta < blockDamage) meta = 0;
                    else meta -= blockDamage;
                    world->getRegion(lookingAt.x, lookingAt.y, lookingAt.z)->modifyMeta(
                        lookingAt.x, lookingAt.y, lookingAt.z, meta);
                    }
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && tickCounter > 35){
            if(world->getBlock(lookingAt.x, lookingAt.y, lookingAt.z, false) != VOX_Inventory::BlockIds::AIR){
                if(inventory->getSelectedSlot(false) != NULL_ITEM && VOX_Inventory::isBlock(inventory->getSelectedSlot(false))){
                    tickCounter = 0;
                    lookingAt = getLookingAt(true); // Recompute the looking at to get the adjacent block.
                    world->setBlock(lookingAt.x, lookingAt.y, lookingAt.z, inventory->getSelectedSlot(false));
                    inventory->modifySlot(inventory->selectedSlot, -1);
                }
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) inventory->selectedSlot = 0;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) inventory->selectedSlot = 1;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) inventory->selectedSlot = 2;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) inventory->selectedSlot = 3;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) inventory->selectedSlot = 4;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num6)) inventory->selectedSlot = 5;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num7)) inventory->selectedSlot = 6;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num8)) inventory->selectedSlot = 7;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num9)) inventory->selectedSlot = 8;

        if(rX > 360) rX -= 360;
        if(rX < 0) rX += 360;
        if(rY > 90) rY = 90;
        if(rY < -90) rY = -90;
        if(tickCounter % 30 == 0) world->pruneRegions();
    }

    void Player::checkMovement(float *x, float *z){
        float rXRadians = (PI / 180.0) * (rX + 90);
        float rXAdjustedRadians = (PI / 180.0) * (rX);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
            (*x) -= (float) cos(rXAdjustedRadians) * moveSpeed;
            (*z) -= (float) sin(rXAdjustedRadians) * moveSpeed;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
            (*x) += (float) cos(rXAdjustedRadians) * moveSpeed;
            (*z) += (float) sin(rXAdjustedRadians) * moveSpeed;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            (*x) += (float) cos(rXRadians) * moveSpeed;
            (*z) += (float) sin(rXRadians) * moveSpeed;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            (*x) -= (float) cos(rXRadians) * moveSpeed;
            (*z) -= (float) sin(rXRadians) * moveSpeed;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
            if(yVelocity == 0.0f) yVelocity = 0.10f;
        }
    }

    void Player::render(){

    }

    void Player::renderInventory(float width){
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
            item = inventory->getSlot(i, false) & 0x00000FFF;
            quantity = inventory->getSlot(i, true) >> 24;

            if(i == inventory->selectedSlot) bottomLeftX = drawIncrement;
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

    Player::~Player(){
        delete inventory;
    }

    Player::Player(VOX_World::World *world, float x, float y, float z){
        this->x = x;
        this->y = y;
        this->z = z;
        this->world = world;
        this->inventory = new VOX_Inventory::Inventory(40);
        inventory->setContents(0, VOX_Inventory::getItemWithDefaultMeta(2048), 1);
    }

}
