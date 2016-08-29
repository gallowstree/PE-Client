//
// Created by daniel on 21/08/16.
//

#include <cstring>
#include "World.h"
#include "Wall.h"
#include "FloorSection.h"
#include <fstream>
#include <math.h>


ResourceHolder<sf::Texture, Textures> World::textureHolder;

World::World(sf::RenderWindow&  window, std::vector<Player> * players,  std::map<int16_t, Projectile> *projectiles):
window(window),
players(players),
projectiles(projectiles)
{

    readMap(0);

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
    menuIcon.setTexture(textureHolder.get(Textures::SKULL));
    worldFont.loadFromFile("files/sansation.ttf");
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
    World::textureHolder.load(Textures::SKULL, "files/skull-icon.png");
}

void World::readMap(int map)
{
    std::ifstream mapFile(maps[map]);
    std::string line = "";

    std::getline(mapFile, line);
    bounds.width = this->parseMapParameter(line);
    bounds.height = this->parseMapParameter(line);
    area_size = atoi(line.c_str());


    while (std::getline(mapFile, line))
    {
        int objectType = parseMapParameter(line);
        int left =  parseMapParameter(line);
        int top =  parseMapParameter(line);
        int width =   parseMapParameter(line);
        int height =  atoi(line.c_str());

        if (objectType == 0) //wall
            world_entities.push_back(Wall(left, top, width, height));
        else if (objectType == 1) //floor
            world_entities.push_back(FloorSection(Textures::FLOOR_PURPLE_CHESS));
    }
}

int World::parseMapParameter(std::string & line)
{
    auto commaPos = line.find(',');
    char * parameter = (char *)malloc((commaPos+1)*sizeof(char));
    strcpy(parameter,line.substr(0,commaPos).c_str());
    line.erase(0, line.find(',') + 1);
    int value = atoi(parameter);
    free(parameter);
    return value;
}

void World::calculateCamCenter()
{
    auto pVector = (*players);
    if(findPlayer(playerID,players))
    {
        camCenter = pVector[playerID].sprite.getPosition();

        if (pVector[playerID].sprite.getPosition().x < window.getSize().x / 2)
            camCenter.x = window.getSize().x / 2;
        else if (pVector[playerID].sprite.getPosition().x > bounds.width - window.getSize().x / 2)
            camCenter.x = pVector[playerID].sprite.getPosition().x;//bounds.width - 300;

        if (pVector[playerID].sprite.getPosition().y < window.getSize().y / 2)
            camCenter.y = window.getSize().y / 2;
        else if (pVector[playerID].sprite.getPosition().y > bounds.height - window.getSize().y / 2)
            camCenter.y = pVector[playerID].sprite.getPosition().y;//bounds.height - 300;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            camCenter = window.mapPixelToCoords((sf::Mouse::getPosition(window)));
    }
}

void World::createStaticObjects()
{

    for (auto& entity : world_entities)
    {
        if (entity.type == EntityType::Wall_T)
        {
            for (auto& area : areasForEntity(entity))
            {
                //static_entities[area].push_back(&entity);
                areas[area]->walls.push_back(&entity);
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

    calculateCamCenter();
    sf::FloatRect visibleRect(camCenter.x - window.getSize().x, camCenter.y - window.getSize().y, window.getSize().x * 2, window.getSize().y * 2);

    for (auto &area : areas)
    {
        if (area->rect.intersects(visibleRect))
        {
            area->draw(window, true);
        }
    }

    for (const auto &staticEntity : world_entities)
    {
        auto drawRect = sf::RectangleShape(sf::Vector2f(staticEntity.boundingBox.width, staticEntity.boundingBox.height));
        drawRect.setPosition(staticEntity.boundingBox.left, staticEntity.boundingBox.top);
        drawRect.setFillColor(sf::Color(144,144,144));

        window.draw(drawRect);
    }

    window.setView(camera);
    camera.setCenter(camCenter);

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
            window.draw(player.sprite);
            player.nickText.setPosition(player.boundingBox.getPosition().x + player.boundingBox.getSize().x / 2 - player.nickText.getLocalBounds().width / 2,
                                        player.boundingBox.getPosition().y + player.sprite.getTextureRect().height -7 );
            player.nickText.setFont(worldFont);

            player.nickText.setCharacterSize(14);
            player.nickText.setColor(sf::Color::White);

            player.healthWrapper.setPosition(player.boundingBox.getPosition().x + 6,player.boundingBox.getPosition().y + player.sprite.getTextureRect().height -12);
            player.healthBox.setPosition(player.healthWrapper.getPosition().x,player.healthWrapper.getPosition().y);

            player.healthBox.setSize(sf::Vector2f(player.health * 40 / 100,2));
            if(player.health >= 75 && player.health <= 100)
                player.healthBox.setFillColor(sf::Color::Green);
            else if(player.health >= 50 && player.health < 75)
                player.healthBox.setFillColor(sf::Color::Yellow);
            else
                player.healthBox.setFillColor(sf::Color::Red);
            window.draw(player.nickText);
            window.draw(player.healthWrapper);
            window.draw(player.healthBox);
        }

    }
    window.draw(cursorSprite);
}

void World::update(sf::Time elapsedTime)
{
    deleteInvalidProjectiles();
    updateCrosshair();
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