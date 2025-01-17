#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <chrono>
#include <thread>
#include "structures.h"
#include "constants.h"

#pragma once

void print(point target, point next, point prev, block visited[space_size][space_size]);

void animatePath(node target, block visited[space_size][space_size], bool& shouldDraw);

std::vector<node> calculatePath(node target);

void setBlockColors(
    block blocks[space_size][space_size],
    bool& shouldDraw,
    point next = { -1,-1 }
);