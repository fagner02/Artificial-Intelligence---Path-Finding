#include <structures.h>
#include <math.h>

#pragma once

typedef float (*heuristic_fn)(point, point);

float heuristic1(point a, point b);

float heuristic2(point a, point b);