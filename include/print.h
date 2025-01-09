#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <chrono>
#include <thread>
#include "structures.h"
#include "constants.h"

void print_path(std::vector<point>& path, visited_info visited[space_size][space_size]);

void print(point target, point next, point prev, visited_info visited[space_size][space_size]);

void calculate_path(point start, point target, visited_info visited[space_size][space_size]);