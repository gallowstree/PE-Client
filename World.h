//
// Created by daniel on 21/08/16.
//

#ifndef TEST_CLIENT_WORLD_H
#define TEST_CLIENT_WORLD_H

#include "Textures.h"
#include "Player.h"
#include "ResourceHolder.h"
#include "Projectile.h"
#include "FloorSection.h"
#include "Area.h"
#include <SFML/Audio.hpp>


class Area;


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
    static ResourceHolder<sf::Texture, Textures> textureHolder;
    sf::View camera;
    static void loadTextures();

	bool mouseIsDown;
	sf::Sound sfxShotGun;
	std::vector<int16_t> enabledPickups;
private:
    std::vector<Player> * players;
    std::map<int16_t,Projectile> * projectiles;
    sf::FloatRect bounds;
    float area_size;
    sf::Sprite cursorSprite;
    sf::Vector2f camCenter;
    sf::SoundBuffer sfxShotGunBuffer;
    sf::SoundBuffer sfxNoAmmoBuffer;
	sf::Sound sfxNoAmmo;
    std::vector<Entity*> world_entities;
    std::vector<const char *> maps = {"maps/level1.txt","maps/level2.txt"};
    std::vector<const char *> playlist = {"files/sound/roller_mobster.ogg","files/sound/infiltrate_acid_spit.ogg","files/sound/cant_catch_me.ogg"};
    std::vector<Area*> areas;
    sf::Sprite menuIcon;
    sf::RectangleShape radar;
	sf::Text ammoText;
	sf::Font hudFont;
	sf::Sprite ammoSpirte;
	sf::Text goverText[4];
	sf::Font worldFont;
	sf::Music * bgMusic = new sf::Music();
    void setRadarPosition();
    void updateCrosshair();
    void calculateCamCenter();
    void createStaticObjects();
    void deleteInvalidProjectiles();
    void checkProjectileCollisions(Projectile &proj);
    void indexMovingEntities();
    std::vector<int16_t> areasForEntity(const Entity &entity);
    void loadSounds();
    void readMap2(int map);
    void selectTrack();


	Player *getCurrentPlayer();

};


#endif //TEST_CLIENT_WORLD_H
