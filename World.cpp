//
// Created by daniel on 21/08/16.
//

#include <cstring>
#include "World.h"
#include "Wall.h"
#include "FloorSection.h"
#include "Pickup.h"
#include <fstream>
#include <math.h>
#include <sstream>


ResourceHolder<sf::Texture, Textures> World::textureHolder;

World::World(sf::RenderWindow&  window, std::vector<Player> * players,  std::map<int16_t, Projectile> *projectiles,int16_t selectedMap):
window(window),
players(players),
projectiles(projectiles),
ammoSpirte()
{

    readMap2(selectedMap);

    int noAreasX = 0;
    int noAreasY = 0;

    noAreasX = static_cast<int>(bounds.width / area_size);
    noAreasY = static_cast<int>(bounds.height / area_size);

    camera.reset(sf::FloatRect(0,0, window.getSize().x, window.getSize().y));
    camera.setViewport(sf::FloatRect(0,0, 1.0f, 1.0f));

    for (int x = 0; x < noAreasX; x++)
    {
        for (int y = 0; y < noAreasY; y++)
        {
            Area* newArea = new Area(x*area_size, y*area_size, area_size, area_size);
            areas.push_back(newArea);
        }
    }

    createStaticObjects();
    cursorSprite.setTexture(textureHolder.get(Textures::CROSSHAIR));
    ammoSpirte.setTexture(textureHolder.get(Textures::HUD_AMMO));
    ammoSpirte.setPosition(10, window.getSize().y - ammoText.getCharacterSize() - 10);

    menuIcon.setTexture(textureHolder.get(Textures::SKULL));
    worldFont.loadFromFile("files/sansation.ttf");
    hudFont.loadFromFile("files/hud.ttf");

    radar.setSize((sf::Vector2f(bounds.width/20,bounds.height/20)));
    radar.setFillColor(sf::Color(0,0,0,100));
    radar.setOutlineThickness(1);
    radar.setOutlineColor(sf::Color(255,255,255,150));
    loadSounds();

    ammoText.setFont(hudFont);
    ammoText.setString("");
    ammoText.setPosition(20 + ammoSpirte.getTextureRect().width , window.getSize().y - ammoText.getCharacterSize() - 10);
    ammoText.setColor(sf::Color::White);
    ammoText.setCharacterSize(20);
}

void World::updateCrosshair()
{
    sf::Vector2f mousePositionFloat = window.mapPixelToCoords((sf::Mouse::getPosition(window)));
    sf::IntRect cursorSpriteRect = cursorSprite.getTextureRect();
    cursorSprite.setPosition(mousePositionFloat - sf::Vector2f(cursorSpriteRect.width*1.5f , cursorSpriteRect.height*1.5f));
}

void World::loadTextures()
{
    World::textureHolder.load(Textures::CROSSHAIR, "files/crosshair.png");
    World::textureHolder.load(Textures::PLAYER_RED, "files/sprite.png");
    World::textureHolder.load(Textures::PLAYER_GREEN, "files/sprite2.png");
    World::textureHolder.load(Textures::BULLET_SMALL, "files/small_bullet.png");
    World::textureHolder.load(Textures::FLOOR_PURPLE_CHESS, "files/floor-purple-chess.png");
    World::textureHolder.load(Textures::FLOOR_BLUE_BRICK, "files/floor-blue-brick.png");
    World::textureHolder.load(Textures::FLOOR_W00D, "files/floor-wood.png");
    World::textureHolder.load(Textures::FLOOR_2, "files/floor2.png");
    World::textureHolder.load(Textures::SKULL, "files/skull-icon.png");
    World::textureHolder.load(Textures::RED_DEAD, "files/red_dead.png");
    World::textureHolder.load(Textures::GREEN_DEAD, "files/green_dead.png");
    World::textureHolder.load(Textures::HUD_AMMO, "files/hud-ammo.png");
    World::textureHolder.load(Textures::PICKUP_AMMO, "files/ammo.png");
    World::textureHolder.load(Textures::PICKUP_HEALTH, "files/health.png");
    World::textureHolder.load(Textures::WALL, "files/wall.png");
    World::textureHolder.load(Textures::GHOST, "files/ghost.png");
}

void World::loadSounds()
{
    sfxShotGunBuffer.loadFromFile("files/sound/shotgun.wav");
    sfxShotGun.setBuffer(sfxShotGunBuffer);
    sfxShotGun.setVolume(30);

    sfxNoAmmoBuffer.loadFromFile("files/sound/empty-gun.ogg");
    sfxNoAmmo.setBuffer(sfxNoAmmoBuffer);
    sfxNoAmmo.setVolume(30);

    sfxScreamBuffer.loadFromFile("files/sound/scream.wav");
    sfxScream.setBuffer(sfxScreamBuffer);
    sfxScream.setVolume(30);

    sfxReloadBuffer.loadFromFile("files/sound/reload_bullets.wav");
    sfxReload.setBuffer(sfxReloadBuffer);
    sfxReload.setVolume(30);


    selectTrack();
}

void World::selectTrack()
{
    srand(time(NULL));
    int track = rand() % playlist.size();
    bgMusic->setVolume(40);
    bgMusic->openFromFile(playlist[track]);
    bgMusic->play(); //let me program to my music, motherfucker!
}

void World::readMap2(int map)
{
    std::ifstream mapFile(maps[map]);
    std::string line = "";

    while (std::getline(mapFile, line))
    {
        std::vector<const char*> params;
        std::stringstream ss(line);
        std::string item;
        while (getline(ss, item, ','))
        {
            auto paramString = (char *) malloc(item.length());
            strcpy(paramString, item.c_str());
            if (strlen(paramString) > 0)
                params.push_back(paramString);
        }

        if (strncmp(params[0], "1", strlen(params[0])) == 0) //World info
        {
            bounds.width  = atoi(params[1]);
            bounds.height = atoi(params[2]);
            area_size     = atoi(params[3]);

            printf("%f, %f, %f \n", bounds.width, bounds.height, area_size);
        }
        else if (strncmp(params[0], "0", strlen(params[0])) == 0) //Wall
        {
            world_entities.push_back(new Wall(atoi(params[1]), atoi(params[2]), atoi(params[3]), atoi(params[4]),textureHolder.get(Textures::WALL) ));
        }
        else if (strncmp(params[0], "2", strlen(params[0])) == 0)//Floor
        {
            auto fs = new FloorSection(atoi(params[5]), atoi(params[1]), atoi(params[2]), atoi(params[3]), atoi(params[4]));
            fs->initSprite(textureHolder.get(fs->getTexture()));
            world_entities.push_back(fs);
        }
        else if (strncmp(params[0], "3", strlen(params[0])) == 0)//Pickup
        {
            auto pu = new Pickup(atoi(params[1]), atoi(params[2]), atoi(params[3]),
                                 atoi(params[4]), atoi(params[5]), atoi(params[6]), atoi(params[7]));
            pu->initSprite(textureHolder.get(pu->getTexture()));

            world_entities.push_back(pu);
            all_pickups.push_back(pu);
        }

        for (auto& param : params)
        {
            free((void *)param);
        }
    }

}

void World::calculateCamCenter()
{


    auto pVector = (*players);
    if(findPlayer(playerID,players))
    {
        auto playerPosition = pVector[playerID].sprite.getPosition();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        {
            float maxCamMoveFactor = 0.25f;
            auto maxCamMovement = sf::Vector2f( playerPosition.x + window.getSize().x * maxCamMoveFactor,
                                                playerPosition.y + window.getSize().y * maxCamMoveFactor);

            auto minCamMovement = sf::Vector2f( playerPosition.x - window.getSize().x * maxCamMoveFactor,
                                                playerPosition.y - window.getSize().y * maxCamMoveFactor);

            camCenter = window.mapPixelToCoords((sf::Mouse::getPosition(window)));

            if (camCenter.x > maxCamMovement.x)
                camCenter.x = maxCamMovement.x;
            else if (camCenter.x < minCamMovement.x)
                camCenter.x = minCamMovement.x;

            if (camCenter.y > maxCamMovement.y)
                camCenter.y = maxCamMovement.y;
            else if (camCenter.y < minCamMovement.y)
                camCenter.y = minCamMovement.y;
        }
        else
        {
            auto halfWinSize = sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2);
            camCenter = playerPosition;


            if (playerPosition.x < halfWinSize.x)
                camCenter.x = halfWinSize.x;
            else if (playerPosition.x > bounds.width - halfWinSize.x)
                camCenter.x = playerPosition.x;

            if (playerPosition.y < halfWinSize.y)
                camCenter.y = halfWinSize.y;
            else if (playerPosition.y > bounds.height - halfWinSize.y)
                camCenter.y = playerPosition.y;
        }

    }
}

void World::createStaticObjects()
{
    for (auto& entity : world_entities)
    {
        if (entity->type == EntityType::Wall_T)
        {
            for (auto& area : areasForEntity(*entity))
            {
                areas[area]->walls.push_back((Wall*)entity);
            }
        }
        else if (entity->type == EntityType::FloorSection_T)
        {
            for (auto& area : areasForEntity(*entity))
            {
                areas[area]->floors.push_back((FloorSection*)entity);
            }
        }
        else if (entity->type == EntityType::Pickup_T)
        {
            for (auto& area : areasForEntity(*entity))
            {
                printf("pickup id %i %i\n", ((Pickup*)entity)->pickupId, ((Pickup*)entity)->enabled ? 1 : 0);
                areas[area]->pickups.push_back((Pickup*)entity);
            }
        }
    }

}

void World::deleteInvalidProjectiles()
{
    //delete invalid projectiles
    if(projectiles->size() >= 100)
    {
        for (auto it = projectiles->cbegin(); it != projectiles->cend();) {
            if (!(*it).second.valid) {
                //printf("proyectil %d eliminado\n", (*it).first);
                projectiles->erase(it++);
            }
            else {
                ++it;
            }
        }
    }
}

void World::render()
{
    sf::FloatRect visibleRect(camCenter.x - window.getSize().x, camCenter.y - window.getSize().y, window.getSize().x * 2, window.getSize().y * 2);

    for (auto& ent : world_entities)
    {
        if (ent->type == FloorSection_T)
            ent->needsDrawing = true;
    }

    for (auto &area : areas)
    {
        if (area->rect.intersects(visibleRect))
        {
            area->draw(window, debugAreas);
        }
    }




    window.setView(camera);


    for(auto const &projectile : *projectiles)
    {
        if(projectile.second.valid)
            window.draw(projectile.second.sprite);
    }

    for (auto &player : *players)
    {
        if(player.valid)
        {
            double angle = player.rotation * 180 / M_PI;
            sf::IntRect spriteRect = player.sprite.getTextureRect();
            player.sprite.setPosition(player.boundingBox.left + player.boundingBox.width / 2,
                                      player.boundingBox.top + player.boundingBox.height / 2);
            player.sprite.setOrigin(sf::Vector2f(spriteRect.width / 2, spriteRect.height / 2));
            player.sprite.setRotation(angle);

            player.sprite.setColor(sf::Color(255, 255, 255, player.invisible ?
                                                            (player.playerID == playerID ? 128 : 0)
                                                                             : 255));
            window.draw(player.sprite);
            player.nickText.setPosition(player.boundingBox.getPosition().x + player.boundingBox.getSize().x / 2 - player.nickText.getLocalBounds().width / 2,
                                        player.boundingBox.getPosition().y + player.sprite.getTextureRect().height -7 );
            player.nickText.setFont(worldFont);

            player.nickText.setCharacterSize(14);
            player.nickText.setColor(sf::Color::White);
            player.nickText.setString(player.nick);

            player.healthWrapper.setPosition(player.boundingBox.getPosition().x + 6,player.boundingBox.getPosition().y + player.sprite.getTextureRect().height -12);
            player.healthBox.setPosition(player.healthWrapper.getPosition().x,player.healthWrapper.getPosition().y);
            if(player.health > 0 && (!player.invisible || player.playerID == playerID))
            {
                player.healthBox.setSize(sf::Vector2f(player.health * 40 / 100, 2));
                if (player.health >= 75 && player.health <= 100)
                    player.healthBox.setFillColor(sf::Color::Green);
                else if (player.health >= 50 && player.health < 75)
                    player.healthBox.setFillColor(sf::Color::Yellow);
                else
                    player.healthBox.setFillColor(sf::Color::Red);

                window.draw(player.healthWrapper);
                window.draw(player.healthBox);

                sf::RectangleShape radarPos(sf::Vector2f(4,4));
                radarPos.setFillColor(sf::Color::White);
                radarPos.setPosition(radar.getPosition().x + player.boundingBox.left / 20 , radar.getPosition().y + player.boundingBox.top / 20 );
                window.draw(radarPos);
                window.draw(player.nickText);
            }
            else if(player.health == 0)
            {
                player.sprite.setTexture(textureHolder.get(player.team == 0 ? Textures::RED_DEAD : Textures::GREEN_DEAD), true);
                if(!player.death)
                {
                    sfxScream.play();
                    player.death = true;
                }
                window.draw(player.nickText);
            }
            player.nickText.setPosition(player.boundingBox.getPosition().x + player.boundingBox.getSize().x / 2 - player.nickText.getLocalBounds().width / 2,
                                        player.boundingBox.getPosition().y + player.sprite.getTextureRect().height -7 );



        }

    }
    calculateCamCenter();
    camera.setCenter(camCenter);
    window.draw(cursorSprite);

    window.setView(window.getDefaultView());

    auto currentPlayer = getCurrentPlayer();

    if (currentPlayer != nullptr)
    {
        auto ammoString = std::to_string(currentPlayer->ammo);
        ammoText.setString(ammoString);
        window.draw(ammoText);
        window.draw(ammoSpirte);
    }


    window.setView(camera);
    setRadarPosition();
    window.draw(radar);

}

void World::update(sf::Time elapsedTime)
{
    deleteInvalidProjectiles();
    updateCrosshair();

    if(bgMusic->getStatus() == sf::SoundSource::Status::Stopped)
    {
        selectTrack();
    }

    if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mouseIsDown)
    {
        mouseIsDown = true;
        Player* player = getCurrentPlayer();
        if (player != nullptr && player->ammo == 0 && window.hasFocus())
        {
            sfxNoAmmo.play();
        }
    }
    else
    {
        mouseIsDown = false;
    }
}

void World::updateProjectiles(const sf::Time &elapsedTime) {
    indexMovingEntities();
    for (auto &projectile : *projectiles)
    {
        if (projectile.second.valid)
        {
            projectile.second.update(elapsedTime);
            checkProjectileCollisions(projectile.second);
        }
    }
}

void World::indexMovingEntities()
{
    for (auto& area: areas)
    {
        area->moving_entities.clear();
    }

    for (auto& entity : *players)
    {
        for (auto& area : areasForEntity(entity))
        {
            areas[area]->moving_entities.push_back(&entity);
        }
    }
}

void World::checkProjectileCollisions(Projectile &proj)
{
    if (!proj.valid) return;
    for (auto& area: areasForEntity(proj))
    {
        for (auto& other_entity : areas[area]->walls)
        {
            sf::FloatRect intersection;
            if (other_entity->boundingBox.intersects(proj.boundingBox, intersection))
            {
                proj.intersectedWith(other_entity, intersection);
            }
        }
        if (!proj.valid) return;

        for (auto& target : areas[area]->moving_entities)
        {
            sf::FloatRect intersection;
            if (target->boundingBox.intersects(proj.boundingBox, intersection))
            {
                proj.intersectedWith(target, intersection);
                target->intersectedWith(&proj, intersection);
            }
        }
    }
}


std::vector<int16_t> World::areasForEntity(const Entity &entity)
{
    std::vector<int16_t> areaslocal;
    int i = 0;
    for (auto &area : areas)
    {
        if (area->rect.intersects(entity.boundingBox))
        {
            areaslocal.push_back(i);
        }

        i++;
    }

    return areaslocal;
}

bool World::findPlayer(int16_t playerID, std::vector<Player> * players)
{
    for(auto &player : *players)
    {
        if(player.playerID == playerID)
            return true;
    }
    return false;
}

void World::setRadarPosition()
{

    radar.setPosition(camCenter.x + window.getSize().x / 2 - radar.getSize().x - 10,
                      camCenter.y + window.getSize().y / 2 - radar.getSize().y - 10);
}

bool World::gameOver(int16_t winner) {
    calculateCamCenter();
    goverText[0].setFont(worldFont);
    goverText[0].setString("PLAY AGAIN");
    goverText[0].setPosition(camCenter.x - 95, camCenter.y + 20);
    goverText[0].setColor(sf::Color::White);
    goverText[0].setCharacterSize(35);

    goverText[1].setFont(worldFont);
    goverText[1].setString("EXIT");
    goverText[1].setPosition(camCenter.x - 40, camCenter.y+ 90);
    goverText[1].setColor(sf::Color::White);
    goverText[1].setCharacterSize(35);

    goverText[2].setFont(worldFont);
    goverText[2].setString("GAME OVER");
    goverText[2].setPosition(camCenter.x - 270, camCenter.y - 200);
    goverText[2].setColor(sf::Color::White);
    goverText[2].setCharacterSize(90);

    goverText[3].setFont(worldFont);
    goverText[3].setColor(sf::Color::White);
    goverText[3].setCharacterSize(35);

    sf::RectangleShape alphaback(sf::Vector2f(camera.getSize().x,camera.getSize().y));
    alphaback.setPosition(camera.getCenter().x - camera.getSize().x / 2, camera.getCenter().y - camera.getSize().y / 2);



    if (winner == 0) {
        goverText[3].setString("WINNER: RED TEAM");
        goverText[3].setPosition(camCenter.x-170,camCenter.y-80);
        alphaback.setFillColor(sf::Color(255,0,0,50));
    }
    else if (winner == 1)
    {
        goverText[3].setString("WINNER: GREEN TEAM");
        goverText[3].setPosition(camCenter.x-185,camCenter.y-80);
        alphaback.setFillColor(sf::Color(0,255,0,50));
    }
    else {
        goverText[3].setString("DRAW MATCH");
        goverText[3].setPosition(camCenter.x-115,camCenter.y-80);
        alphaback.setFillColor(sf::Color(0,0,255,50));
    }

    int selectedOption =0;
    bool next = false;
    bool response = false;
    while (!next)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    exit(0);
                case sf::Event::KeyPressed:
                    switch (event.key.code)
                    {
                        case sf::Keyboard::Key::Up:
                            if (selectedOption > 0)
                                selectedOption--;
                            break;
                        case sf::Keyboard::Key::Down:
                            if (selectedOption + 1 < 2)
                                selectedOption++;
                            break;
                        case sf::Keyboard::Key::Return:
                            if (selectedOption == 0)
                            {
                                bgMusic->stop();
                                delete(bgMusic);
                                response = true;
                            }
                            next = true;
                    }
                default:
                    break;
            }
        }

        window.clear();
        render();
        window.draw(alphaback);
        for (int i = 0; i < 4; i++) {
            window.draw(goverText[i]);
            if (i == selectedOption) {
                menuIcon.setPosition(goverText[i].getPosition().x - 55, goverText[i].getPosition().y + 2);
            }
        }
        window.draw(menuIcon);
        window.display();
    }
    return response;
}

Player *World::getCurrentPlayer() {
    if (players->size() > 0 && playerID != -1)
        return &(*players)[playerID];

    return nullptr;
}

