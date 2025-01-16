#include <iostream>
#include <stack>
#include <structures.h>
#include <costs.h>
#include <constants.h>
#include <print.h>
#include <heuristics.h>
#include <queue>
#include <set>

#pragma once

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
    std::vector<int> order
);

std::string dfs(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    int cost_id,
    std::vector<int> order = { 0, 1, 2, 3 },
    bool animate = false
);

std::string bfs(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    int cost_id,
    std::vector<int> order = { 0, 1, 2, 3 },
    bool animate = false
);

std::string a_star(
    point start,
    point target,
    cost_fn cost,
    heuristic_fn heuristic,
    int cost_id,
    int heuristic_id,
    std::set<point> constraints,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    bool animate = false
);

std::string dijkstra(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    int cost_id,
    bool animate = false
);

std::string greedy_search(
    point start,
    point target,
    cost_fn cost,
    heuristic_fn heuristic,
    int cost_id,
    int heuristic_id,
    std::set<point> constraints,
    block nodes[space_size][space_size],
    bool& shouldDraw,
    bool animate = false
);