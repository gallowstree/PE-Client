//
// Created by daniel on 8/08/16.
//

#ifndef TEST_CLIENT_SFUTILS_H
#define TEST_CLIENT_SFUTILS_H

#include <SFML/Graphics.hpp>

sf::Vector2f getSpriteCenter(const sf::Sprite& sprite)
{
    sf::IntRect spriteRect = sprite.getTextureRect();
    sf::Vector2f spriteCenter = sprite.getPosition() + sf::Vector2f(spriteRect.width/ 2, spriteRect.height / 2);
    return spriteCenter;
}


#endif //TEST_CLIENT_SFUTILS_H
