//
// Created by daniel on 21/08/16.
//

#include "World.h"
/*

void World::updateCrosshair()
{
    sf::Vector2f mousePositionFloat = mWindow.mapPixelToCoords((sf::Mouse::getPosition(mWindow)));
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
    std::string line;

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

void World::calculateCamCenter()
{
    camCenter = players[playerID].sprite.getPosition();

    if (players[playerID].sprite.getPosition().x < mWindow.getSize().x /2)
        camCenter.x = mWindow.getSize().x /2;
    else if (players[playerID].sprite.getPosition().x > bounds.width - mWindow.getSize().x /2)
        camCenter.x = players[playerID].sprite.getPosition().x;//bounds.width - 300;

    if (players[playerID].sprite.getPosition().y < mWindow.getSize().y /2)
        camCenter.y = mWindow.getSize().y /2;
    else if (players[playerID].sprite.getPosition().y > bounds.height - mWindow.getSize().y /2)
        camCenter.y = players[playerID].sprite.getPosition().y;//bounds.height - 300;
}
*/