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
        shape.setFillColor(sf::Color(0,0,0));
        shape.setOutlineColor(sf::Color(0, 255, 18));

        shape.setOutlineThickness(2);
        window.draw(shape);
    }
}

Area::Area(float d, float d1, float d2, float d3)
{
    rect = sf::FloatRect(d, d1, d2, d3);
}
