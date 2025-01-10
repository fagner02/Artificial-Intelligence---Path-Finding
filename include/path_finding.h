#include <iostream>
#include <stack>
#include <structures.h>
#include <costs.h>
#include <constants.h>
#include <print.h>
#include <heuristics.h>

#pragma once

using namespace std;

string generate_log(
    point start,
    point target,
    int visited_qty,
    int generated_qty,
    block blocks[space_size][space_size],
    string algorithm
);

string dfs(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw
);

string bfs(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw
);

string a_star(
    point start,
    point target,
    cost_fn cost,
    heuristic_fn heuristic,
    block nodes[space_size][space_size],
    bool& shouldDraw
);

string dijkstra(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw
);

string greedy_search(
    point start,
    point target,
    cost_fn cost,
    heuristic_fn heuristic,
    block nodes[space_size][space_size],
    bool& shouldDraw
);