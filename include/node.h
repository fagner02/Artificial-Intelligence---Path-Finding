#include <iostream>
#include <vector>
#include <set>
#include <point.h>

#pragma once

struct node;

struct visited_info {
    float cost;
    float heuristic;
    int step;
    node* from;
    bool found_goal = false;
    std::string label = "";
    bool visited = false;
    bool is_path = false;
};

struct node {
    visited_info data;
    std::vector<node*> children;
    node* parent;
    point pos;
};

int count_tree_nodes(node* root);

void print_tree(node* root, int level = 0);

#include <goal_test.h>

node* add_node(std::vector<node*>& tree_nodes, node* parent, point pos, std::set<point> constraints, float cost, float heuristic);