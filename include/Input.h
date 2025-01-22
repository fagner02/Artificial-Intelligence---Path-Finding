#include <SFML/Graphics.hpp>

#pragma once
class TextBox;
class Input {
public:
    bool thumbPressed = false;
    bool charPressed = false;
    bool selecting = false;
    bool inputFocused = false;
    bool isHovering = false;
    sf::Vector2i selectedChars = sf::Vector2i(-1, -1);
    std::vector<TextBox*> inputs = {};
    sf::Vector2u size;

    TextBox* add(TextBox* input);

    void MousePressed(sf::Vector2i pos);

    void MouseMove(sf::Vector2i pos);

    void MouseReleased(sf::Vector2i pos);

    void KeyPressed(sf::Event::KeyEvent key);

    void TextEntered(sf::Uint32 unicode);

    void cursorChanged(TextBox*);
};