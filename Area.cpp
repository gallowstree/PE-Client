//
// Created by daniel on 15/08/16.
//

#include "Area.h"


void Area::draw(sf::RenderTarget &window, bool debugGrid)
{
    if (debugGrid)
    {
        auto shape = sf::RectangleShape(sf::Vector2f(rect.width, rect.height));
        shape.setPosition(rect.left, rect.top);
        shape.setFillColor(sf::Color(0,0,0,0));
        shape.setOutlineColor(sf::Color(0, 255, 18));

        shape.setOutlineThickness(2);
        window.draw(shape);
    }

    for (auto& floor : floors)
    {
        World::textureHolder.get(floor->getTexture()).setRepeated(true);
        if (floor->needsDrawing)
        {
            window.draw(floor->sprite);
            floor->needsDrawing = false;
        }

    }

    for (auto& wall: walls)
    {
        auto drawRect = sf::RectangleShape(sf::Vector2f(wall->boundingBox.width, wall->boundingBox.height));
        drawRect.setPosition(wall->boundingBox.left, wall->boundingBox.top);
        drawRect.setFillColor(sf::Color(144,144,144));
        window.draw(drawRect);
    }

    for (auto& pickup : pickups)
    {

        printf("%f, %f, %i, %i, %i\n", pickup->sprite.getPosition().x, pickup->sprite.getPosition().y,
                                 pickup->sprite.getTextureRect().width, pickup->sprite.getTextureRect().height,
                                 pickup->enabled ? 1 : 0);
        if (pickup->enabled)
            window.draw(pickup->sprite);
    }
}

Area::Area(float d, float d1, float d2, float d3)
{
    rect = sf::FloatRect(d, d1, d2, d3);
}
