#include <SFML/Graphics.hpp>
#include <rectshape.h>
#include <functional>
#include <constants.h>
#include <set>
#include <node.h>

#pragma once

struct block {
    sf::RoundedRectangleShape shape;
    sf::Text text;
    visited_info info;
};

void fill_blocks(block blocks[space_size][space_size], std::set<point> constraints = {}, point start = { -1,-1 }, point target = { -1,-1 });

void set_block_data(block blocks[space_size][space_size], node n);

struct label_data {
    sf::Text text;
    sf::RoundedRectangleShape box;
    bool visible = true;
};

struct button {
    label_data* _label;
    std::function<void()> fn;
    bool pressed = false;
};

const point dirs[] = {
    /*0: left*/ {-1, 0},
    /*1: right*/{1, 0},
    /*2: up*/   {0, -1},
    /*3: down*/ {0, 1},
};