#include <iostream>
#include <stack>
#include <structures.h>
#include <costs.h>
#include <constants.h>
#include <print.h>

#pragma once

using namespace std;

void dfs(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size], bool& shouldDraw
);

void bfs();

void a_star();

void dijkstra();

void greedy_search();