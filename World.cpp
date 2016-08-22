//
// Created by daniel on 21/08/16.
//

#include <cstring>
#include "World.h"
#include "Wall.h"
#include <fstream>
#include <math.h>

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
            static_entities.push_back(std::vector<Entity*>());
            moving_entities.push_back(std::vector<Entity*>());
        }
    }

    createStaticObjects();
    loadTextures();
    cursorSprite.setTexture(textureHolder.get(Textures::CROSSHAIR));

}

void World::updateCrosshair()
{
    sf::Vector2f mousePositionFloat = window.mapPixelToCoords((sf::Mouse::getPosition(window)));
    sf::IntRect cursorSpriteRect = cursorSprite.getTextureRect();
    cursorSprite.setPosition(mousePositionFloat - sf::Vector2f(cursorSpriteRect.width*1.5f , cursorSpriteRect.height*1.5f));
}

void World::loadTextures()
{
    textureHolder.load(Textures::CROSSHAIR, "files/crosshair.png");
    textureHolder.load(Textures::PLAYER_RED_SG, "files/sprite.png");
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
    }
}

void World::createStaticObjects()
{

    for (auto& entity : world_entities)
    {
        if (entity.isStatic)
        {
            for (auto& area : areasForEntity(entity))
            {
                static_entities[area].push_back(&entity);
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
            window.draw(projectile.second.projectile);
    }

    for (auto &player : *players)
    {
        double angle = player.rotation * 180/M_PI;
        sf::IntRect spriteRect = player.sprite.getTextureRect();
        player.sprite.setPosition(player.boundingBox.left + player.boundingBox.width/2, player.boundingBox.top + player.boundingBox.height/2);
        player.sprite.setOrigin(sf::Vector2f(spriteRect.width/ 2, spriteRect.height / 2));
        player.sprite.setRotation(angle);
        window.draw(player.sprite);

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
    moving_entities.clear();
    for (auto& entity : *players)
    {
        for (auto& area : areasForEntity(entity))
        {
            moving_entities[area].push_back(&entity);
        }
    }
}

void World::checkProjectileCollisions(Projectile &proj)
{
    if (!proj.valid) return;
    for (auto& area: areasForEntity(proj))
    {
        for (auto& other_entity : static_entities[area])
        {
            sf::FloatRect intersection;
            if (other_entity->boundingBox.intersects(proj.boundingBox, intersection))
            {
                proj.intersectedWith(other_entity, intersection);
            }
        }
        if (!proj.valid) return;

        for (auto& target : moving_entities[area])
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