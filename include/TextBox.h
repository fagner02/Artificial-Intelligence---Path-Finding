#include <GL/gl.h>
#include <DrawObject.h>
#include <Label.h>
#include <iostream>
#include <cmath>

#pragma once

class Input;
class TextBox : public DrawObject {
public:
    Label* label;
    sf::RectangleShape cursorLine;
    sf::RoundedRectangleShape scrollThumb;
    std::wstring value;
    int cursor = 0;
    bool isMultiline = false;
    bool hasFocus = false;
    float padding = 10;
    Input* inputs;
    std::wstring tooltip = L"";

    TextBox(createLabelArgs args, bool isMultiline, std::wstring tooltip = L"");

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::FloatRect getBounds();

    sf::Vector2f calculateSize() const override;

    void setPosition(sf::Vector2f position) override;

    void setSize(sf::Vector2f size) override;

    void drawCharSelection(sf::RenderTarget& target, sf::RenderStates states, int k) const;

    float getScrollSub() const;

    void setThumbPos(float ypos);

    void deleteSelected(sf::Vector2i selectedChars);

    void setThumbValues(bool set_pos = false);

    void setText(std::wstring text);

    std::wstring getSelect(sf::Vector2i selectedChars);
};