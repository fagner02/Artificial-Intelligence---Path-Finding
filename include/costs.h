#include <functional>
#include <math.h>

#pragma once

float c3(float t);

float c4(float t);

float cost_all10(int dir, int step);

float cost_v10h15(int dir, int step);

float cost_v10hc3(int dir, int step);

float cost_v10hc4(int dir, int step);

typedef std::function<float(int, int)> cost_fn;

const cost_fn costs[] = {
    cost_all10,
    cost_v10h15,
    cost_v10hc3,
    cost_v10hc4
};