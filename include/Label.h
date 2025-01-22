#include <DrawObject.h>
#include <SFML/Graphics.hpp>
#include <rectshape.h>

#pragma once

struct createLabelArgs {
    sf::Font& font;
    sf::Vector2f pos;
    float pad = 10;
    std::wstring text;
    int charSize = 18;
    sf::Color outColor = sf::Color(255, 255, 255);
    sf::Color textColor = sf::Color(255, 255, 255);
    sf::Color boxColor = sf::Color(100, 100, 100);
    sf::Color textOutColor = sf::Color(255, 255, 255);
    int outlineThickness = 2;
    int textOutlineThickness = 0;
    float margin = 0;
    sf::Vector2f size;
};
class Label : public DrawObject {
public:
    sf::RoundedRectangleShape box;
    sf::Text text;
    bool autoSize = true;
    float padding;
    Label(createLabelArgs args) {
        this->text.setFont(args.font);
        this->text.setString(args.text);
        this->text.setCharacterSize(args.charSize);
        this->text.setFillColor(args.textColor);
        this->text.setOutlineColor(args.textOutColor);
        this->text.setOutlineThickness(args.textOutlineThickness);
        this->box.setCornersRadius(10);
        this->box.setCornerPointCount(20);
        this->box.setFillColor(args.boxColor);
        this->box.setOutlineColor(args.outColor);
        this->box.setOutlineThickness(args.outlineThickness);
        this->position = args.pos;
        this->margin = args.margin;
        this->padding = args.pad;
        setPosition(args.pos);
    }
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        if (!visible) return;
        target.draw(box, states);
        target.draw(text, states);
    }
    sf::Vector2f calculateSize() const override {
        return box.getLocalBounds().getSize();
    }
    void setPosition(sf::Vector2f position) override {
        this->position = position;
        position.x += margin;
        position.y += margin;
        updatePosition(position);
    }
    void setSize(sf::Vector2f size) override {
        autoSize = false;
        this->size = sf::Vector2f(size.x, size.y);
        updatePosition(position);
    }
    void updatePosition(sf::Vector2f position) {
        auto textSize = text.getLocalBounds();
        auto boxSize = sf::Vector2f(textSize.width + padding * 2.f, textSize.height + padding * 2.f);
        if (!autoSize) {
            float outline = box.getOutlineThickness();
            boxSize = sf::Vector2f(size.x - outline * 2.f, size.y - outline * 2.f);
        }
        box.setSize(boxSize);
        box.setPosition(position);
        text.setOrigin(textSize.left + textSize.width / 2.f, textSize.top + textSize.height / 2.f);
        text.setPosition(sf::Vector2f(position.x + boxSize.x / 2.f, position.y + boxSize.y / 2.f));
        size = calculateSize();
    }
    void setText(std::wstring text) {
        this->text.setString(text);
        updatePosition(position);
    }
};