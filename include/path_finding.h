#include <iostream>
#include <stack>
#include <structures.h>
#include <costs.h>
#include <constants.h>
#include <print.h>
#include <heuristics.h>

#pragma once

using namespace std;

void dfs(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw
);

void bfs(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw
);

void a_star(
    point start,
    point target,
    cost_fn cost,
    heuristic_fn heuristic,
    block nodes[space_size][space_size],
    bool& shouldDraw
);

void dijkstra(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw
);

void greedy_search(
    point start,
    point target,
    cost_fn cost,
    heuristic_fn heuristic,
    block nodes[space_size][space_size],
    bool& shouldDraw
);