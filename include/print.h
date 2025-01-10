#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <chrono>
#include <thread>
#include "structures.h"
#include "constants.h"

void print_path(std::vector<point>& path, block visited[space_size][space_size]);

void print(point target, point next, point prev, block visited[space_size][space_size]);

void calculate_path(point start, point target, block visited[space_size][space_size]);