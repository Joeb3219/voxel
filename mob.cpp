#include <iostream>
#include "mob.h"
#include "math.h"
#include "world.h"
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
            if(IS_SOLID(currentPos.x, currentPos.y, currentPos.z)){
                if(!adjacent) return currentPos;
                return (currentPos + stepVector);
            }
        }
        return lookingAt;
    }

    void Player::update(){
        VOX_World::Region *regionIn = world->getRegion(x, y, z);
        if(regionIn != 0){
            if(!regionIn->loaded) return; // Region isn't loaded, let's not do anything yet.
        }
        tickCounter ++;

        float newY = y, newX = x, newZ = z;

        checkMovement(&newX, &newZ);

        int numSteps = 8;
        sf::Vector3f curr = sf::Vector3f(x, y, z), goal(newX, y, newZ), stepVector;
        stepVector = (curr - goal) * (1.0f / numSteps);
        for(int i = 0; i < numSteps; i ++){
            curr -= stepVector;
            if(IS_SOLID(curr.x, curr.y + 1, curr.z) == true ||
                IS_SOLID(curr.x, curr.y + 2, curr.z)){
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
                if(IS_SOLID(x, j, z) == true){
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
                if(IS_SOLID(x, j + 2.5f, z) == true){
                    yVelocity = 0;
                    y = (float) ((int) j);
                    break;
                }
            }
        }

        sf::Vector3f lookingAt = getLookingAt();
        VOX_World::BlockData *blockLookingAt = world->getBlock(lookingAt.x, lookingAt.y, lookingAt.z);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && tickCounter > 10){
            blockLookingAt = world->getBlock(lookingAt.x, lookingAt.y, lookingAt.z);
            if(blockLookingAt->id != 0 && blockLookingAt->meta != 0xFF){
                if(blockLookingAt->meta == 0){
                    tickCounter = 0;
                    inventory->addItem(VOX_World::blocks[(int)blockLookingAt->id].drops, 1);
                    world->setBlock(lookingAt.x, lookingAt.y, lookingAt.z, VOX_Inventory::BlockIds::AIR);
                    unsigned int hand = inventory->getSelectedSlot(false);
                    if(VOX_Inventory::isTool(hand)){
                        inventory->setContents(inventory->selectedSlot, ((hand & 0x00FFF000) - 0x00001000) | (hand & 0x00000FFF), 1);
                    }
                }else{
                    int blockDamage = inventory->getBreakSpeed(&VOX_World::blocks[(int)blockLookingAt->id]);
                    if(blockLookingAt->meta < blockDamage) blockLookingAt->meta = 0;
                    else blockLookingAt->meta -= blockDamage;
                }
            }
        }else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && tickCounter > 35){
            if(blockLookingAt->id != VOX_Inventory::BlockIds::AIR){
                if(inventory->getSelectedSlot(false) != NULL_ITEM && VOX_Inventory::isBlock(inventory->getSelectedSlot(false))){
                    tickCounter = 0;
                    lookingAt = getLookingAt(true); // Recompute the looking at to get the adjacent block.
                    if(!((int) lookingAt.x == (int) x && ((int) lookingAt.y == (int) y + 2 || (int) lookingAt.y == (int) y + 1) && (int) lookingAt.z == (int) z)){
                        world->setBlock(lookingAt.x, lookingAt.y, lookingAt.z, inventory->getSelectedSlot(false));
                        inventory->modifySlot(inventory->selectedSlot, -1);
                    }
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
        inventory->render(width);
    }

    Player::~Player(){
        delete inventory;
    }

    Player::Player(VOX_World::World *world, float x, float y, float z){
        this->x = x;
        this->y = y;
        this->z = z;
        this->world = world;
        this->inventory = new VOX_Inventory::PlayerInventory(40);
        inventory->setContents(0, VOX_Inventory::getItemWithDefaultMeta(2048), 1);
        inventory->setContents(1, VOX_Inventory::BlockIds::LIGHTSTONE, 32);
    }

}
