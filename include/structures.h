#include <SFML/Graphics.hpp>
#include <rectshape.h>
#include <functional>
#include <constants.h>

#pragma once

struct point {
    int x, y;
};

point operator+(const point& lhs, const point& rhs);

point operator-(const point& lhs, const point& rhs);

bool operator==(const point& lhs, const point& rhs);

bool operator!=(const point& lhs, const point& rhs);

bool operator<(const point& lhs, const point& rhs);

struct node;

struct visited_info {
    float cost;
    float heuristic;
    int step;
    node* from;
    bool found_goal = false;
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

struct node {
    visited_info data;
    std::vector<node*> children;
    node* parent;
    point pos;
};

const point dirs[] = {
    /*0: left*/ {-1, 0},
    /*1: right*/{1, 0},
    /*2: up*/   {0, -1},
    /*3: down*/ {0, 1},
};