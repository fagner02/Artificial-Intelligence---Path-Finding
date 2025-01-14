#include <SFML/Graphics.hpp>
#include <rectshape.h>
#include <functional>

#pragma once

struct point {
    int x, y;
};

struct visited_info {
    float cost;
    float heuristic;
    int step;
    point from;
};

struct block {
    sf::RoundedRectangleShape shape;
    sf::Text text;
    visited_info info;
};

struct label {
    sf::Text text;
    sf::RoundedRectangleShape box;
};

struct button {
    label* _label;
    std::function<void()> fn;
    bool pressed = false;
};

bool operator==(const point& lhs, const point& rhs);