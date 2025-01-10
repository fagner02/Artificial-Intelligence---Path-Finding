#include <SFML/Graphics.hpp>
#include <rectshape.h>

#pragma once

struct point {
    int x, y;
};

struct visited_info {
    float cost;
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

bool operator==(const point& lhs, const point& rhs);