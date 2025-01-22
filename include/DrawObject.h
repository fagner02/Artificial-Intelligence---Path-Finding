#include <SFML/Graphics.hpp>

#pragma once

class DrawObject : public sf::Drawable {
public:
    bool visible = true;
    float margin = 0;
    sf::Vector2f position;
    sf::Vector2f size;
    bool childUpdated = false;
    bool parentUpdated = false;
    DrawObject* parent = nullptr;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
    virtual sf::Vector2f calculateSize() const = 0;
    virtual void setPosition(sf::Vector2f position) = 0;
    virtual void setSize(sf::Vector2f size) {
        this->size = size;
    }
    virtual void update() {};

    virtual sf::FloatRect getBounds() {
        return sf::FloatRect(position, size);
    }
};