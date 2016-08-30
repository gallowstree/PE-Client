//
// Created by daniel on 21/08/16.
//

#ifndef TEST_CLIENT_WORLD_H
#define TEST_CLIENT_WORLD_H


#include "Player.h"
#include "ResourceHolder.h"
#include "Projectile.h"
#include "Area.h"

enum Textures
{
    CROSSHAIR,
    PLAYER_RED,
    PLAYER_GREEN,
    BULLET_SMALL,
    FLOOR_PURPLE_CHESS,
    SKULL,
    RED_DEAD,
    GREEN_DEAD
};

class World
{
public:
    World(sf::RenderWindow&  window, std::vector<Player> * players, std::map<int16_t, Projectile> *projectiles);
    sf::RenderWindow&  window;
    int playerID = -1;
    void render();
    void update(sf::Time elapsedTime);
    void updateProjectiles(const sf::Time &elapsedTime);
    bool findPlayer(int16_t playerID, std::vector<Player> * players);
    bool gameOver(int16_t winner);

    sf::Text goverText[4];
    sf::Font worldFont;
    static ResourceHolder<sf::Texture, Textures> textureHolder;

    static void loadTextures();

private:
    std::vector<Player> * players;
    std::map<int16_t,Projectile> * projectiles;
    int parseMapParameter(std::string & line);
    sf::FloatRect bounds;
    float area_size;
    sf::View camera;
    sf::Sprite cursorSprite;
    sf::Vector2f camCenter;
    std::vector<Entity> world_entities;
    std::vector<const char *> maps = {"maps/level1.txt","maps/level2.txt"};
    std::vector<Area*> areas;
    sf::Sprite menuIcon;

    void updateCrosshair();

    void readMap(int map);
    void calculateCamCenter();
    void createStaticObjects();
    void deleteInvalidProjectiles();
    void checkProjectileCollisions(Projectile &proj);
    void indexMovingEntities();
    std::vector<int16_t> areasForEntity(const Entity &entity);

};


#endif //TEST_CLIENT_WORLD_H
