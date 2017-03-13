#include <SFML/Window.hpp>

namespace VOX_Inventory{
    class Inventory;
    class PlayerInventory;
}

namespace VOX_World{
    class World;
}

namespace VOX_Mob{
    class Mob{
    private:
        float x = 0, y = 0, z = 0;
    public:
        virtual sf::Vector3f getPosition() = 0;
        virtual void update() = 0;
        virtual void render() = 0;
    };

    class Player : public Mob{
    private:
        float x = 0, y = 0, z = 0, rX = 0, rY = 0, rZ = 0;
        float yVelocity = 0;
        int tickCounter = 0;
        VOX_Inventory::PlayerInventory *inventory = 0;
        VOX_World::World *world = 0;
        float moveSpeed = 0.05f;
        void checkMovement(float *x, float *z);
    public:
        Player(VOX_World::World *world, float x, float y, float z);
        ~Player();
        sf::Vector3f getPosition();
        sf::Vector3f getViewAngles();
        void setMouseChange(sf::Vector2i change);
        sf::Vector3f getLookingAt(bool adjacent = false);
        void update();
        void render();
        void renderInventory(float width);
    };
}
