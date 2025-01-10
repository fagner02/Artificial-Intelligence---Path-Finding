#include <structures.h>
#include <math.h>

#pragma once

typedef float (*heuristic_fn)(point, point);

float h1(point a, point b);

float h2(point a, point b);