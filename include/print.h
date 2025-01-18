#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <chrono>
#include <thread>
#include <structures.h>
#include <constants.h>

#pragma once

void print(point target, point next, point prev, block visited[space_size][space_size]);

void animatePath(node target, block visited[space_size][space_size], bool& shouldDraw);

std::vector<node> calculatePath(node target);

void setBlockColors(
    block blocks[space_size][space_size],
    bool& shouldDraw,
    point next = { -1,-1 }
);

std::string generate_log(
    point start,
    point target,
    int visited_qty,
    int generated_qty,
    std::string algorithm,
    int cost_id,
    int heuristic_id,
    float cost,
    std::vector<node> path,
    std::vector<int> order = { 0, 1, 2, 3 },
    std::set<point> constraints = {}
);


void animate_greedy_search_cost(std::vector<node*> path, block blocks[space_size][space_size], bool& shouldDraw);