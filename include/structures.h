#include <SFML/Graphics.hpp>
#include <rectshape.h>
#include <functional>
#include <constants.h>

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

void fill_blocks(block blocks[space_size][space_size]);

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

const point dirs[] = {
    /*0: left*/ {-1, 0},
    /*1: right*/{1, 0},
    /*2: up*/   {0, -1},
    /*3: down*/ {0, 1},
};